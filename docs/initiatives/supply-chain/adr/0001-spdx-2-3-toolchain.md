# ADR-0001: SPDX 2.3生成・検証toolchainを採用する

- Status: Proposed
- Date: 2026-07-16
- Decision owners: `harutasti`
- Related initiative: [SBOM and build provenance](../README.md)
- Related decision: `DEC-SC-001`
- Related requirements: `FR-SC-002`、`FR-SC-003`、`FR-SC-004`、`FR-SC-005`、
  `FR-SC-010`、`SR-SC-003`、`NFR-SC-001`、`NFR-SC-003`、`NFR-SC-005`
- Related design: `ED-SC-005`、`ED-SC-012`
- Feasibility evidence: [G2技術検証結果報告書](../feasibility-report.md)
- Review PR: [#22](https://github.com/harutasti/cppseed/pull/22)
- Supersedes: None

## Context

cppseedは4platformの最終配布archiveごとに、archive自身と内包する実行ファイル、LICENSE、
README、検出可能な依存関係を表す機械可読SBOMを公開する必要がある。SBOMは最終archiveの
SHA-256と対応し、標準toolでvalidationでき、GitHub artifact attestationのSBOM predicateとして
利用できなければならない。

G2ではLinux x86-64の単一archiveについて、Syft 1.46.0でstaging directoryをscanし、
archive fileと包含関係を制御された変換で追加したSPDX 2.3 JSONを、SPDX Tools 0.8.5で
validationできた。archive、SBOM、checksumおよびSBOM attestationのdigest/内容も一致した。

一方、次の制約がある。

- Syftのraw outputだけでは「最終配布archiveそのもの」をFile entityとして表せない
- system runtime libraryをSBOM scannerだけで完全に列挙できない
- 独自のpost-processingはSPDX semanticを壊すriskを持つ
- toolやPython dependencyを固定しないと、同じ設計でも出力・validation結果が変化し得る
- 4platformで同じcontractを満たしつつ、platform固有dependency inspectionも保持する必要がある

## Decision drivers

- SPDXという公開標準を使い、cppseed独自formatだけへ依存しないこと
- archiveごとにversion、target、archive digest、内包3 filesを機械検証できること
- GitHub artifact attestationsのSBOM modeで利用できること
- schemaだけでなくSPDX modelのsemantic validationができること
- G2で得た実行可能なevidenceを再利用できること
- toolとdependencyをhash固定し、更新をreview可能にすること
- system dependencyの検出限界を「依存関係なし」と誤表示しないこと
- 将来のSPDX/tool更新を明示的な判断として扱えること

## Considered options

### Option 1: SPDX 2.3 + Syft 1.46.0 + controlled transformation + SPDX Tools 0.8.5

- Benefits:
  - G2で生成、validation、attestationおよびsemantic equalityまで実測済み
  - SPDX 2.3 JSONとして一般的なSBOM consumerが処理できる
  - Syftのfile/package検出を利用しつつ、cppseed固有のarchive contractを補完できる
  - generatorとvalidatorを分け、変換側の誤りを別実装で検出できる
  - tool version、binary SHA-256、Python wheel hashを固定できる
- Costs/risks:
  - post-processing codeとそのunit/negative testを保守する必要がある
  - SyftとSPDX Toolsの両方のupdate影響を評価する必要がある
  - scannerだけではsystem dependencyを完全には表現できない
  - SPDX 2.3のFile checksum要件によりSHA-1も計算・保持する

### Option 2: Syftのraw SPDX JSONを変更せず公開する

- Benefits:
  - 独自変換codeを持たず、実装と保守が小さい
  - Syftが生成したmodelをそのまま扱える
- Costs/risks:
  - 最終archive自身のfilenameとSHA-256をSBOM entityとして保証できない
  - archiveとstaging scan結果の対応が間接的になり、`FR-SC-001` と `FR-SC-005` を満たしにくい
  - target、dependency report、archive包含関係のpublic contractを固定できない

### Option 3: SPDX JSONをcppseed専用generatorで一から生成する

- Benefits:
  - 必要field、順序、namespace、relationshipを完全に制御できる
  - Syftのraw modelへ依存しない
- Costs/risks:
  - package/dependency検出を独自実装する必要がある
  - SPDX specificationの解釈・追随範囲が大きく、semantic error riskが高い
  - 既存scannerが持つfile metadataとecosystem検出能力を失う
  - 初期OSSの保守範囲として過大である

### Option 4: 別のSPDX versionまたはCycloneDXへ変更する

- Benefits:
  - 将来のtoolingやdata modelによっては、より自然にartifact/dependencyを表現できる可能性がある
  - CycloneDXを選ぶ場合、別ecosystemのconsumerと整合する可能性がある
- Costs/risks:
  - 承認済み要件とG3 public contractの変更が必要になる
  - G2の生成、validation、SBOM attestation evidenceをそのまま利用できない
  - 4platform、online/offline attestationを含む再spikeが必要になる

## Decision

Option 1を採用する。

1. Release assetのSBOM形式をSPDX 2.3 JSON（`SPDX-2.3`）とする
2. Syft 1.46.0で最終archive作成時のstaging directoryをscanする
3. raw outputへ制御された変換を加え、次を明示する
   - document namespace、creator、tool version、creation time
   - cppseed packageのversion、supplier、MIT license、purpose
   - 最終archiveのFile entity、SHA-1、SHA-256
   - 実行ファイル、LICENSE、READMEのFile entityとchecksum
   - document `DESCRIBES` package、package `CONTAINS` archive、archive `CONTAINS` content files
   - target、dependency report filename、scannerの検出限界
4. `packageVerificationCode` の対象File集合、除外集合、sort/concatenationをG5で一意に定義する
5. SPDX Tools 0.8.5でparse、model validationおよびschema validationを実施する
6. Syft配布物はversionとSHA-256、SPDX Toolsと全dependencyは対象Python環境のhash lockで固定する
7. SBOM generation、変換、validationまたはsemantic checkの失敗はRelease blockerとする
8. system dependencyはSBOM scannerだけで完全性を主張せず、platform別の `ldd`、`otool -L`、
   `dumpbin /dependents` reportをRelease assetとして併用する

versionは互換範囲ではなく完全一致でbaseline化する。Dependabot等による更新提案も自動採用せず、
生成差分、validation、4platform dry runおよび必要な再spikeをreviewしてから変更する。

## Consequences

### Positive

- archive、SBOM、checksum、SBOM attestationを同じSHA-256へ直接結び付けられる
- 標準SPDX consumerとGitHub attestation verificationを利用できる
- generatorとvalidatorの実装を分け、変換誤りを検出するcontrolを持てる
- G2 evidenceをG5以降の実装・testへ具体的に引き継げる
- system dependencyの不完全性を明示し、誤った安全性主張を避けられる

### Negative

- 専用transformer、schema、semantic oracle、dependency lockの保守が必要になる
- SBOM tool更新ごとにraw modelと変換の互換性確認が必要になる
- SPDX 2.3が必要とするSHA-1を、完全性用途のSHA-256とは別に扱う必要がある
- GitHub-hosted runnerやscanner差異により、許容field以外の安定性検証が複雑になる
- SBOMだけでsystem runtime dependencyの完全性を保証できない

## Validation and review trigger

- Validation:
  - 4platformすべてでSPDX Tools 0.8.5のparse/model/schema validationを通す
  - document/package/archive/content files/relationshipsのpositive/negative testをG7で定義する
  - archiveのSHA-256がSBOM、metadata、checksum、attestation subjectで一致することを検証する
  - `packageVerificationCode` を独立計算し、不正なFile集合・checksum・relationshipで失敗させる
  - created、namespace等の許容差分を除いたnormalized SBOMの安定性を検証する
  - target別dependency reportとの参照・digest・未解決dependencyを検証する
  - tool binary、wheel hash、environment marker lockのtamper/omission testを行う
- Revisit when:
  - Syft 1.46.0またはSPDX Tools 0.8.5に修正不能なsecurity/compatibility問題が見つかる
  - GitHub artifact attestationsがSPDX 2.3 predicateを扱えなくなる
  - 採用toolのsupport終了またはrunner/Pythonとの非互換が発生する
  - post-processingなしでG3 contractを満たす標準toolが実測で利用可能になる
  - 新しいSPDX versionへ移行する明確なinteroperability/security上の利益があり、再spikeが成功する
  - dependency表現の不足が利用者の検証を妨げる重大issueになる

見直し時は本ADRを書き換えず、新しいADRでsupersedeし、要件、外部設計、schema、test、
README/Release notesおよび過去Releaseとの互換性を同時に評価する。

## Approval

個人運営のため、PR上で観点別self-reviewとして記録する。

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Technical | `harutasti` | Pending | — | — |
| Testability | `harutasti` | Pending | — | — |
| Security | `harutasti` | Pending | — | — |
