# ADR-0002: 配布archiveをsubjectとする署名済みbuild metadata predicateを採用する

- Status: Proposed
- Date: 2026-07-16
- Decision owners: `harutasti`
- Related initiative: [SBOM and build provenance](../README.md)
- Related decision: `DEC-SC-002`
- Related requirements: `FR-SC-001`、`FR-SC-006`、`FR-SC-007`、`FR-SC-008`、
  `FR-SC-009`、`FR-SC-010`、`SR-SC-004`、`SR-SC-005`、`NFR-SC-001`、`NFR-SC-002`
- Related design: `ED-SC-006`、`ED-SC-007`、`ED-SC-008`、`ED-SC-012`
- Feasibility evidence: [G2技術検証結果報告書](../feasibility-report.md)
- Review PR: [#22](https://github.com/harutasti/cppseed/pull/22)
- Supersedes: None

## Context

GitHub Actionsが生成するSLSA provenanceは、archive digest、source repository、workflow、builder、
run identityを標準形式で結び付ける。一方、cppseed要件はcompiler、CMake、runner image、
build options、dependency evidence、artifact作成時刻など、利用者とrelease担当者がbuild条件を
追跡するための追加情報も要求する。

追加metadataを単なるRelease assetとして公開すると、downloadしたJSONが信頼されたworkflowで
作られたものか、どのarchiveに対応するかをchecksum以外では検証できない。反対に、metadata
file自体を別subjectとしてattestするとmetadataの真正性は確認できるが、archiveとの結び付きは
JSON内の自己申告だけになる。

G2ではallowlist方式で秘密情報やworkspace pathを含まないmetadataを生成できた。G3では、
SLSA provenance、SPDX SBOMおよびbuild metadataの3 statementで同じ最終archiveをsubjectにし、
Release asset JSONと署名済みpredicateを意味的に比較するpublic contractを定めた。

## Decision drivers

- build metadataと最終archiveを署名で直接結び付けること
- SLSA provenanceを独自formatへ置き換えず、標準predicateを維持すること
- Release assetとして人間が読めるJSONと、機械検証可能なsigned statementを両立すること
- compiler、CMake、runner、build option、evidence digestを追跡できること
- token、secret、個人情報、不要なabsolute pathを公開しないこと
- schema evolutionとpredicate typeの互換性を明示できること
- online/offlineの同じidentity policyで検証できること

## Considered options

### Option 1: archiveを共通subjectとするcustom build metadata predicate

- Benefits:
  - archive digestがcustom predicateのin-toto subjectとして暗号学的に結び付く
  - SLSA provenanceとSBOM attestationを標準predicateのまま併用できる
  - metadata assetとsigned predicateをsemantic comparisonできる
  - `gh attestation verify` のrepository、workflow、source、predicate policyを再利用できる
  - metadata schemaとpredicate typeをcppseed側でversion管理できる
- Costs/risks:
  - custom predicate URIとschemaを長期保守する必要がある
  - predicate内容はworkflowが供給するため、署名は値の真実性を自動保証しない
  - Release assetとpredicateのsemantic equality checkが必要になる
  - 1 archive当たりのattestationが3件になる

### Option 2: 自動生成SLSA provenanceへ独自fieldを埋め込む

- Benefits:
  - statement数を増やさず、1つのprovenanceで情報を確認できる
  - archive subjectとの対応はprovenance内で維持できる
- Costs/risks:
  - `actions/attest` が生成する標準provenanceを任意field付きで安全に拡張するcontractがない
  - 独自provenance generatorを持つとGitHub builder identityとの整合が複雑になる
  - SLSA consumerとのinteroperabilityを損なう可能性がある
  - tool updateで独自fieldが消失・無視される可能性がある

### Option 3: build metadata file自体を別のattestation subjectにする

- Benefits:
  - downloadしたmetadata fileそのもののdigestと署名を検証できる
  - Release assetとattestation subjectのbyte一致が単純になる
- Costs/risks:
  - metadata内のarchive digestがworkflowの自己申告となり、archiveとの直接の共通subjectを失う
  - 利用者はarchive用attestationとmetadata用attestationを別々に辿る必要がある
  - subjectとasset数が増え、offline bundleとverification policyが複雑になる

### Option 4: unsigned metadata assetをSHA256SUMSだけで保護する

- Benefits:
  - 実装と利用者手順が最も小さい
  - custom predicate/schema運用が不要になる
- Costs/risks:
  - `SHA256SUMS` 自体は署名ではなく、metadataの生成主体を証明できない
  - archiveとmetadataの対応をtrusted workflowへ結び付けられない
  - `FR-SC-007`、`FR-SC-009`、`FR-SC-010` のsecurity intentを満たさない

## Decision

Option 1を採用する。

1. 各最終archiveを共通subjectとして、次の3 attestationを個別に発行する
   - `https://slsa.dev/provenance/v1`
   - `https://spdx.dev/Document/v2.3`
   - `https://github.com/harutasti/cppseed/attestations/build-metadata/v1`
2. custom predicateのsubject nameはarchive filename、subject digestは最終archiveのSHA-256とする
3. predicate bodyは `ED-SC-006` の `schemaVersion: 1` build metadata objectとする
4. 同じJSON objectを `<ARCHIVE>.build-metadata.json` としてReleaseへ公開する
5. 公開前に両JSONをstrict parseしてduplicate keyを拒否し、署名済みpredicateとRelease assetを
   object key順・空白を無視して意味的に比較する
6. metadataは明示allowlistだけから生成し、次を禁止する
   - token、secret、credential、署名鍵
   - email、不要な個人情報
   - workspace、home、temporary directoryのabsolute path
   - environment全件dump、任意の未審査workflow input
7. metadataの `artifact.sha256` はsubject、SPDX、`SHA256SUMS` と一致させる
8. `evidence.sbom.sha256` と `evidence.dependencyReport.sha256` で補助assetもsigned predicateへ結び付ける
9. online/offline verificationはrepository、signer workflow path/digest、source ref/digest、
   predicate type、GitHub-hosted runnerをすべて制約する
10. `schemaVersion: 1` は未知fieldを拒否するclosed contractとする。fieldの追加・削除・意味変更・
    必須性変更はpredicate type `/v2`、`schemaVersion: 2` および新しいADRを必要とする

署名は「このGitHub identityを持つworkflowがこのstatementを発行した」ことを証明する。
predicate内のcompiler version等が現実と一致することは、収集方法、allowlist、独立commandとの比較、
testおよびworkflow保護で保証する。署名の存在だけを内容の客観的真実性と説明しない。

## Consequences

### Positive

- archive、provenance、SBOM、build metadataを同じsubject digestで検索・検証できる
- SLSA/SPDXの標準predicateを変更せず、cppseed固有の追跡情報を追加できる
- 人間向けRelease JSONと署名済み機械可読predicateを同じ内容にできる
- metadataのarchive/SBOM/dependency report対応を署名へ含められる
- online/offlineで同じGitHub CLI identity policyを使える

### Negative

- custom predicate URI、schema、collector、semantic comparisonを保守する必要がある
- 署名済みであっても、compromised trusted workflowが虚偽metadataを作るriskは残る
- attestation発行、verification、bundleの件数と所要時間が増える
- 同一tagのrerunでは複数statementが生じ得るため、sourceとsubject policyによる選別が必要になる
- schema evolution時にRelease notes、verifier、test、consumer互換性の調整が必要になる

## Validation and review trigger

- Validation:
  - 4 archiveすべてで3 predicateと同一subject name/SHA-256を確認する
  - metadata assetをJSON schemaで検証し、duplicate keyを拒否してsigned predicateとの
    normalized equalityを確認する
  - artifact、source、workflow、runner、build、tools、evidenceの必須fieldと型をpositive/negative testする
  - subject、`artifact.sha256`、SBOM archive digest、`SHA256SUMS` の不一致を必ず失敗させる
  - SBOM/dependency report assetのSHA-256とmetadata evidenceを比較する
  - token pattern、credential、absolute workspace/home path、未知top-level fieldの混入を失敗させる
  - online/offlineでcustom predicate typeと全identity constraintを付けて検証する
  - metadata収集値をcompiler/CMake/runnerの独立command・GitHub contextと比較する
- Revisit when:
  - GitHubまたはSLSAが必要なbuild metadataを標準predicateで表現・検証できるようになる
  - custom predicateが主要consumerとのinteroperabilityを妨げる
  - metadata schemaのfield意味、必須性、privacy boundaryを破壊的に変更する必要がある
  - `actions/attest` のcustom predicateまたはbundle contractが変わる
  - predicate size、発行回数、verification時間がrelease目標を継続的に超える
  - metadata収集経路に虚偽値またはsecret漏えいincidentが発生する

見直し時は既存Releaseのpredicate/assetを変更せず、新しいpredicate typeとverifierを追加し、
移行期間中に旧versionを検証できる状態を維持する。

## Approval

個人運営のため、PR上で観点別self-reviewとして記録する。

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Technical | `harutasti` | Pending | — | — |
| Testability | `harutasti` | Pending | — | — |
| Security | `harutasti` | Pending | — | — |
