# ADR-0003: target別offline bundleとrelease全体のtrusted root snapshotを配布する

- Status: Proposed
- Date: 2026-07-16
- Decision owners: `harutasti`
- Related initiative: [SBOM and build provenance](../README.md)
- Related decision: `DEC-SC-003`
- Related requirements: `FR-SC-002`、`FR-SC-009`、`FR-SC-010`、`FR-SC-012`、
  `FR-SC-013`、`SR-SC-004`、`NFR-SC-001`、`NFR-SC-005`
- Related design: `ED-SC-003`、`ED-SC-007`、`ED-SC-009`、`ED-SC-012`、
  `ED-SC-013`、`ED-SC-014`
- Feasibility evidence: [G2技術検証結果報告書](../feasibility-report.md)
- Supersedes: None

## Context

利用者はGitHub APIへ接続できない環境でも、download済みarchiveのprovenance、SBOMおよび
build metadata attestationを検証できる必要がある。offline verificationには、対象statementの
Sigstore bundleと、署名・certificate・transparency log materialを検証するtrusted rootが必要になる。

cppseedは4 archiveそれぞれに3 attestationを発行するため、release全体で12 bundleが生じる。
これを個別assetにすると利用者はtargetごとに3 filesを選ぶ必要があり、1つのrelease-wide fileへ
まとめると不要なplatformのstatementもdownloadし、targetとの対応を追加処理で選別する必要がある。

G2では単一archiveについて、local bundleと `gh attestation trusted-root` で取得したrootを使い、
tokenを除去してnetworkを通信不能にした状態でonlineと同じidentity/source/predicate policyを
検証できた。ただし、release時点のroot snapshotは将来のtrust updateやrevocation情報を自動的に
反映せず、rootをReleaseと同じchannelから初回取得するbootstrap riskも残る。

## Decision drivers

- GitHub APIなしで3 predicateを検証できること
- 利用者が自分のtargetに必要なbundleをfilenameから機械的に選べること
- Release asset数とverification commandの複雑さを抑えること
- online/offlineで同じrepository、workflow、source、runner、predicate policyを維持すること
- Actions artifact期限切れ後もReleaseから検証materialを取得できること
- 公開済みRelease assetを差し替えず、historical verificationを維持すること
- trusted root snapshotの限界を安全性保証と誤認させないこと
- GitHub/Sigstoreのroot update、revocation、incidentに対応できる見直し点を残すこと

## Considered options

### Option 1: target別3-line JSONL bundle + release全体のversioned trusted root

- Benefits:
  - archive filenameをprefixとして必要なbundleを一意に選べる
  - 1 target当たり1 bundleで3 predicateを検証できる
  - trusted rootを4回重複配布せず、release時点をversioned filenameで保持できる
  - `gh attestation verify --bundle --custom-trusted-root` の標準interfaceを利用できる
  - bundle内のpredicate順・件数を公開contractとして検証できる
- Costs/risks:
  - JSONLの連結順、件数、predicate typeを検証するaggregationが必要になる
  - root snapshotが将来のrevocation/trust updateを自動反映しない
  - Release経由のroot初回取得だけでは独立したtrust bootstrapにならない
  - GitHub bundle/root format変更時にCLI互換性確認が必要になる

### Option 2: 12 attestation bundleを個別Release assetとして配布する

- Benefits:
  - `actions/attest` のbundle outputを連結せず、そのまま保持できる
  - predicateごとのfileが明示的で、単独検証しやすい
- Costs/risks:
  - 4 target × 3 predicateで12 filesとなり、asset選択とdownloadが煩雑になる
  - filename、asset数、checksum、Release UIの複雑さが増える
  - 利用者が3件のうち1件をdownloadし忘れる可能性が高くなる

### Option 3: 12 bundleを1つのrelease-wide JSONLへまとめる

- Benefits:
  - Release asset数とdownload操作が最小になる
  - release全statementを一括保存できる
- Costs/risks:
  - 1 targetだけを使う利用者も全platformのbundleを取得する
  - archiveとbundleの対応がfilenameだけでは一意にならず、subject検索が必要になる
  - target別の件数・predicate完全性を利用者が判断しにくい
  - 将来targetが増えるほどfileとverification outputが大きくなる

### Option 4: offline materialを配布せずonline verificationだけを提供する

- Benefits:
  - bundle/root asset、aggregation、offline testが不要になる
  - verification時に最新trust materialを取得できる
- Costs/risks:
  - network制限環境と長期保存用途を満たせない
  - GitHub API/service availabilityへ常時依存する
  - `FR-SC-002`、`FR-SC-009`、`FR-SC-010`、`FR-SC-012` を満たさない

## Decision

Option 1を採用する。

1. targetごとに、次の順でSigstore bundle JSON objectをcompact JSON Linesへ連結する
   1. SLSA provenance
   2. SPDX 2.3 SBOM
   3. cppseed build metadata v1
2. filenameを `<ARCHIVE>.attestations.jsonl` とし、exactly 3 linesを要求する
3. 各lineのstatement subjectは対応archiveのfilename/SHA-256と一致させる
4. 全12 attestation発行後、公開前検証と同じworkflow runでtrusted rootを1回取得する
5. root filenameを `cppseed-v<VERSION>-trusted-root.jsonl` とし、4 targetで共用する
6. 4 bundleとrootをGitHub Releaseへ長期保存し、`SHA256SUMS` の対象に含める
7. offline verificationでもonlineと同じ次のpolicyを省略しない
   - repository `harutasti/cppseed`
   - signer workflow pathとdigest
   - source tag refとcommit digest
   - predicate type
   - GitHub-hosted runner
8. 公開前testではtokenを除去し、networkを通信不能にして3 predicate × 4 targetを検証する
9. Release同梱rootを「release時点のhistorical snapshot」と説明し、最新revocation/trust情報の
   自動反映や独立したtrust bootstrapを保証しない
10. 高保証用途ではonline verification、最新trusted root、GitHub/Sigstore/cppseed security advisoryも
    確認するようREADMEとRelease notesへ記載する
11. 公開済みbundle/rootは差し替えない。incident時はRelease撤回表示、advisory、新versionで対応する

`SHA256SUMS` はdownload errorと同一性を確認するが署名ではない。bundleは内包署名を検証し、
trusted rootの初回信頼は取得channelと運用controlに依存する。このbootstrap、root update、revocation、
TUF/clock/expiry failureはG6 threat modelとG10 runbookで具体化する。

## Consequences

### Positive

- 利用者はarchiveと同じprefixの1 bundle、およびrelease共通rootだけを選べる
- GitHub API停止・遮断時でも、download済みmaterialから3 claimを検証できる
- online/offlineのpolicy差をなくし、検証手順とtest oracleを共有できる
- Actions artifact期限切れ後もRelease assetとしてhistorical materialを保持できる
- root重複を避けながら、どのrelease時点のsnapshotかfilenameで判断できる

### Negative

- root snapshotの陳腐化、revocation、bootstrap riskを別controlで扱う必要がある
- bundle aggregation、exact line count、predicate order/identityのvalidationが増える
- Release assetが4 bundle + 1 root増え、`SHA256SUMS` とpublic手順が長くなる
- GitHub CLIまたはSigstore format変更時に過去/current Releaseの両方を検証する必要がある
- offline verificationは「完全に外部trust不要」ではなく、事前に安全にmaterialを取得する工程が必要になる

## Validation and review trigger

- Validation:
  - targetごとのJSONLがexactly 3 valid JSON objectsで、期待predicate順・subjectを持つことを確認する
  - bundle/root/`SHA256SUMS` のfilename、version、target、SHA-256をexact setで検証する
  - tokenとnetworkを除いた状態で12 offline verificationを成功させる
  - repository、workflow path/digest、source ref/digest、predicate、runnerを1項目ずつ改変して失敗させる
  - bundle欠落、重複、未知predicate、別target subject、別root、corrupt JSONを失敗させる
  - online/offlineのverified statement subject/predicateを比較する
  - Release asset取得後のcopy可能なLinux/macOS/Windows手順をG7/G9で実行する
  - root取得不能、Sigstore/GitHub outage、clock skew、expired/invalid materialをfault testまたはrunbook drillする
- Revisit when:
  - GitHub CLIが現在のJSON/JSONL bundleまたはcustom trusted root formatを扱えなくなる
  - Sigstore/GitHubのroot distribution、revocation、TUF contractが変更される
  - security incidentによりrelease同梱snapshotでは安全な判断ができないことが判明する
  - target/predicate数の増加でtarget別bundleが運用・performance目標を満たさなくなる
  - 利用者が独立したtrust bootstrapまたは別配布channelを必要とする
  - GitHub Releaseのretention/immutability contractが長期証跡要件を満たさなくなる

見直し時は過去Release assetを変更せず、新しいADR、verification tool/version、migration手順および
security advisoryを用意する。既存bundleを検証できない場合は、その範囲を明示してreleaseを撤回する。

## Approval

個人運営のため、PR上で観点別self-reviewとして記録する。

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Technical | `harutasti` | Pending | — | — |
| Testability | `harutasti` | Pending | — | — |
| Security | `harutasti` | Pending | — | — |
