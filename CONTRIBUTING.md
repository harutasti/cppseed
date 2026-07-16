# Contributing to cppseed

Thanks for helping improve cppseed.

## Development setup

You need a C++20 compiler and CMake 3.20 or newer.

```console
cmake -S . -B build -DCPPSEED_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

To use the same warning policy as CI:

```console
cmake -S . -B build-werror \
  -DCPPSEED_BUILD_TESTS=ON \
  -DCPPSEED_WARNINGS_AS_ERRORS=ON
cmake --build build-werror
ctest --test-dir build-werror --output-on-failure
```

## Pull requests

- Keep changes focused and include tests for observable behavior.
- Update the relevant design document when changing the public contract.
- Add user-visible changes to the `Unreleased` section of
  [CHANGELOG.md](CHANGELOG.md). Use the appropriate `Added`, `Changed`,
  `Deprecated`, `Removed`, `Fixed`, or `Security` heading and describe the
  impact from a user's perspective. Purely internal or documentation-only
  changes do not need an entry unless they change user-facing behavior.
- Do not add a runtime or third-party library dependency without prior design
  discussion.
- Confirm that generated projects build and pass CTest.

## Release documentation

For each release, maintainers move the relevant `Unreleased` entries into a
versioned section with the release date, update the comparison links at the end
of `CHANGELOG.md`, and prepare matching GitHub Release notes. Published release
entries are historical records and must not be rewritten to describe later
changes.

## GitHub Actions dependencies

Dependabot checks GitHub Actions dependencies monthly and groups routine
updates into one pull request. Security updates may be handled sooner.

When reviewing an Actions update:

1. Read the official release notes and identify breaking changes and runner
   requirements.
2. Confirm that the release commit is verified and that `action.yml` uses a
   supported Node.js runtime.
3. Pin `uses:` to the release's full 40-character commit SHA and update the
   adjacent version comment. Do not merge a tag-only or shortened SHA reference.
4. Confirm that workflow permissions remain least-privilege and that no new
   secrets are exposed to pull-request jobs.
5. Run YAML parsing and `actionlint`, then require the normal CI matrix and the
   four-platform release packaging dry run to pass.
6. Inspect the completed workflow annotations for runtime-deprecation or action
   migration warnings before merging.
