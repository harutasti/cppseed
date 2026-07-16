#!/usr/bin/env bash

set -euo pipefail

readonly TARGET="x86_64-unknown-linux-gnu"
readonly SYFT_VERSION="1.46.0"
readonly SYFT_ARCHIVE="syft_${SYFT_VERSION}_linux_amd64.tar.gz"
readonly SYFT_SHA256="d654f678b709eb53c393d38519d5ed7d2e57205529404018614cfefa0fb2b5ca"
readonly SPDX_TOOLS_VERSION="0.8.5"
readonly SPDX_LOCK_FILE=".github/requirements/spdx-tools-0.8.5-linux-py310.txt"

repo_root=$(git rev-parse --show-toplevel)
cd "$repo_root"

output_dir=${1:-supply-chain-evidence}
if [[ "$output_dir" != /* ]]; then
  output_dir="$repo_root/$output_dir"
fi

work_parent=${RUNNER_TEMP:-"$repo_root/.tmp"}
work_dir="$work_parent/cppseed-supply-chain-g2"
rm -rf "$output_dir" "$work_dir"
mkdir -p "$output_dir" "$work_dir"
trap 'rm -rf "$work_dir"' EXIT

if [[ "$(uname -s)" != "Linux" || "$(uname -m)" != "x86_64" ]]; then
  echo "G2 feasibility is pinned to Linux x86-64" >&2
  exit 1
fi

for command in \
  cmake ctest curl cut find g++ git gzip jq ldd paste python3 sed sha1sum \
  sha256sum sort tar tee; do
  command -v "$command" >/dev/null
done

python_version=$(python3 -c \
  'import sys; print(f"{sys.version_info.major}.{sys.version_info.minor}")')
if [[ "$python_version" != "3.10" ]]; then
  echo "expected CPython 3.10, found $python_version" >&2
  exit 1
fi

version=$(sed -nE \
  's/^project\(cppseed VERSION ([0-9]+\.[0-9]+\.[0-9]+) LANGUAGES CXX\)$/\1/p' \
  CMakeLists.txt)
test -n "$version"

source_sha=$(git rev-parse HEAD)
source_epoch=$(git show -s --format=%ct HEAD)
source_ref=${GITHUB_REF:-"$(git symbolic-ref -q HEAD || git rev-parse HEAD)"}
repository=${GITHUB_REPOSITORY:-harutasti/cppseed}
server_url=${GITHUB_SERVER_URL:-https://github.com}

build_dir="$work_dir/build"
staging_dir="$work_dir/staging"
unpacked_dir="$work_dir/unpacked"

cmake -S . -B "$build_dir" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++ \
  -DCPPSEED_BUILD_TESTS=ON \
  -DCPPSEED_WARNINGS_AS_ERRORS=ON
cmake --build "$build_dir" --config Release --parallel
ctest --test-dir "$build_dir" -C Release --output-on-failure
test "$("$build_dir/cppseed" --version)" = "cppseed $version"

mkdir "$staging_dir"
cp "$build_dir/cppseed" LICENSE README.md "$staging_dir/"
chmod 755 "$staging_dir/cppseed"

dependency_report="$output_dir/dependency-report-$TARGET.txt"
ldd "$staging_dir/cppseed" | tee "$dependency_report"
if grep -q 'not found' "$dependency_report"; then
  echo "unresolved dynamic dependency" >&2
  exit 1
fi

archive_name="cppseed-g2-v${version}-${TARGET}.tar.gz"
archive_path="$output_dir/$archive_name"
tar --sort=name \
  --mtime="@$source_epoch" \
  --owner=0 \
  --group=0 \
  --numeric-owner \
  -C "$staging_dir" \
  -cf - cppseed LICENSE README.md | gzip -n >"$archive_path"

mkdir "$unpacked_dir"
tar -xzf "$archive_path" -C "$unpacked_dir"
entries=$(find "$unpacked_dir" -mindepth 1 -maxdepth 1 -type f \
  -exec basename {} \; | LC_ALL=C sort | paste -sd ' ' -)
test "$entries" = "LICENSE README.md cppseed"
test -x "$unpacked_dir/cppseed"
test "$("$unpacked_dir/cppseed" --version)" = "cppseed $version"
"$unpacked_dir/cppseed" --help >/dev/null

(
  cd "$output_dir"
  sha256sum "$archive_name" >SHA256SUMS
  sha256sum --check SHA256SUMS
)
archive_sha1=$(sha1sum "$archive_path" | cut -d ' ' -f1)
archive_sha256=$(sha256sum "$archive_path" | cut -d ' ' -f1)
package_verification_code=$(printf '%s' "$archive_sha1" | sha1sum | cut -d ' ' -f1)

tools_dir="$work_dir/tools"
mkdir "$tools_dir"
syft_download="$tools_dir/$SYFT_ARCHIVE"
curl --fail --location --proto '=https' --tlsv1.2 \
  "https://github.com/anchore/syft/releases/download/v${SYFT_VERSION}/${SYFT_ARCHIVE}" \
  --output "$syft_download"
echo "$SYFT_SHA256  $syft_download" | sha256sum --check --strict
tar -xzf "$syft_download" -C "$tools_dir" syft
syft="$tools_dir/syft"
test "$("$syft" version -o json | jq -r .version)" = "$SYFT_VERSION"

spdx_venv="$work_dir/spdx-venv"
python3 -m venv "$spdx_venv"
"$spdx_venv/bin/python" -m pip install \
  --disable-pip-version-check \
  --no-deps \
  --only-binary=:all: \
  --require-hashes \
  --requirement "$SPDX_LOCK_FILE"
test "$("$spdx_venv/bin/python" -c \
  'import importlib.metadata; print(importlib.metadata.version("spdx-tools"))')" = \
  "$SPDX_TOOLS_VERSION"

raw_sbom="$work_dir/$archive_name.syft-raw.spdx.json"
sbom_path="$output_dir/$archive_name.spdx.json"
SYFT_CHECK_FOR_APP_UPDATE=false \
SYFT_FILE_METADATA_SELECTION=all \
SYFT_FILE_METADATA_DIGESTS=sha1,sha256 \
  "$syft" scan "dir:$staging_dir" \
    --source-name cppseed \
    --source-version "$version" \
    --output "spdx-json=$raw_sbom"

jq \
  --arg archive "$archive_name" \
  --arg archive_sha1 "$archive_sha1" \
  --arg archive_sha256 "$archive_sha256" \
  --arg package_verification_code "$package_verification_code" \
  --arg target "$TARGET" \
  --arg version "$version" \
  '
    .name = ("cppseed-" + $version + "-" + $target)
    | .packages[0].name = "cppseed"
    | .packages[0].versionInfo = $version
    | .packages[0].supplier = "Person: Haruki Goto"
    | .packages[0].licenseDeclared = "MIT"
    | .packages[0].licenseConcluded = "MIT"
    | .packages[0].filesAnalyzed = true
    | .packages[0].packageVerificationCode = {
        packageVerificationCodeValue: $package_verification_code
      }
    | .packages[0].primaryPackagePurpose = "APPLICATION"
    | .packages[0].comment = (
        "Feasibility target: " + $target
        + "; distribution archive: " + $archive
      )
    | .files = ((.files // []) | map(select(.fileName != "")))
    | .files += [{
        fileName: $archive,
        SPDXID: "SPDXRef-File-DistributionArchive",
        fileTypes: ["ARCHIVE"],
        checksums: [
          {algorithm: "SHA1", checksumValue: $archive_sha1},
          {algorithm: "SHA256", checksumValue: $archive_sha256}
        ],
        licenseConcluded: "MIT",
        licenseInfoInFiles: ["MIT"],
        copyrightText: "Copyright (c) 2026 Haruki Goto"
      }]
    | (.packages[0].SPDXID) as $package_id
    | ([.files[] | select(.fileName != $archive) | .SPDXID]) as $contents
    | .relationships = (
        (.relationships // [])
        + [{
            spdxElementId: $package_id,
            relatedSpdxElement: "SPDXRef-File-DistributionArchive",
            relationshipType: "CONTAINS"
          }]
        + ($contents | map({
            spdxElementId: "SPDXRef-File-DistributionArchive",
            relatedSpdxElement: .,
            relationshipType: "CONTAINS"
          }))
      )
  ' "$raw_sbom" >"$sbom_path"

"$spdx_venv/bin/pyspdxtools" -i "$sbom_path"
jq -e \
  --arg archive "$archive_name" \
  --arg digest "$archive_sha256" \
  '
    .spdxVersion == "SPDX-2.3"
    and .packages[0].name == "cppseed"
    and .packages[0].licenseDeclared == "MIT"
    and ([.files[].fileName] | sort)
      == (["LICENSE", "README.md", "cppseed", $archive] | sort)
    and any(
      .files[];
      .fileName == $archive
      and any(.checksums[]; .algorithm == "SHA256" and .checksumValue == $digest)
    )
  ' "$sbom_path" >/dev/null

recorded_sha256=$(jq -r \
  --arg archive "$archive_name" \
  '.files[] | select(.fileName == $archive) | .checksums[]
    | select(.algorithm == "SHA256") | .checksumValue' \
  "$sbom_path")
test "$recorded_sha256" = "$archive_sha256"

compiler_version=$(g++ --version | sed -n '1p')
cmake_version=$(cmake --version | sed -n '1s/^cmake version //p')
kernel_version=$(uname -sr)
created_at=$(date -u +%Y-%m-%dT%H:%M:%SZ)
run_url=""
if [[ -n "${GITHUB_RUN_ID:-}" ]]; then
  run_url="$server_url/$repository/actions/runs/$GITHUB_RUN_ID"
fi
workflow_path=".github/workflows/supply-chain-feasibility.yml"

metadata_path="$output_dir/$archive_name.build-metadata.json"
jq -n \
  --arg archive "$archive_name" \
  --arg archive_sha256 "$archive_sha256" \
  --arg target "$TARGET" \
  --arg repository "$repository" \
  --arg repository_url "$server_url/$repository" \
  --arg source_sha "$source_sha" \
  --arg source_ref "$source_ref" \
  --arg workflow_path "$workflow_path" \
  --arg workflow_run_id "${GITHUB_RUN_ID:-}" \
  --arg workflow_run_attempt "${GITHUB_RUN_ATTEMPT:-}" \
  --arg run_url "$run_url" \
  --arg compiler "$compiler_version" \
  --arg cmake "$cmake_version" \
  --arg kernel "$kernel_version" \
  --arg image_os "${ImageOS:-unknown}" \
  --arg image_version "${ImageVersion:-unknown}" \
  --arg syft "$SYFT_VERSION" \
  --arg spdx_tools "$SPDX_TOOLS_VERSION" \
  --arg created_at "$created_at" \
  '
    {
      schemaVersion: 1,
      artifact: {
        name: $archive,
        target: $target,
        sha256: $archive_sha256
      },
      source: {
        repository: $repository,
        repositoryUrl: $repository_url,
        commit: $source_sha,
        ref: $source_ref
      },
      workflow: {
        path: $workflow_path,
        ref: $source_ref,
        runId: $workflow_run_id,
        runAttempt: $workflow_run_attempt,
        runUrl: $run_url
      },
      build: {
        type: "Release",
        compiler: $compiler,
        cmake: $cmake,
        options: [
          "CMAKE_BUILD_TYPE=Release",
          "CMAKE_CXX_COMPILER=g++",
          "CPPSEED_BUILD_TESTS=ON",
          "CPPSEED_WARNINGS_AS_ERRORS=ON"
        ]
      },
      runner: {
        provider: "GitHub-hosted",
        imageOS: $image_os,
        imageVersion: $image_version,
        kernel: $kernel,
        architecture: "x86_64"
      },
      tools: {
        syft: $syft,
        spdxTools: $spdx_tools
      },
      evidence: {
        dependencyReport: ("dependency-report-" + $target + ".txt")
      },
      createdAt: $created_at
    }
  ' >"$metadata_path"

tool_versions="$output_dir/tool-versions.txt"
{
  cmake --version | sed -n '1p'
  g++ --version | sed -n '1p'
  "$syft" version | sed -n '1,8p'
  "$spdx_venv/bin/python" --version
  "$spdx_venv/bin/python" -m pip list --format=freeze | LC_ALL=C sort
} >"$tool_versions"

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
  {
    echo "archive=$archive_path"
    echo "archive-name=$archive_name"
    echo "archive-sha256=$archive_sha256"
    echo "sbom=$sbom_path"
    echo "metadata=$metadata_path"
    echo "evidence-dir=$output_dir"
  } >>"$GITHUB_OUTPUT"
fi

echo "G2 feasibility evidence created for $archive_name ($archive_sha256)"
