# cppseed リリース・サプライチェーン要件定義書

- 文書バージョン: 0.1
- 対象: Issue #11（SBOMおよびビルド来歴）
- 対象リリース: 本要件の実装後に公開する最初のcppseedリリース
- ステータス: In review
- 文書オーナー: repository maintainer
- 技術レビュー: Pending
- セキュリティレビュー: Pending
- リリース承認: Pending
- 作成日: 2026-07-16
- Initiative: [SBOM and build provenance](README.md)
- 上位文書: [v0.1.0リリース設計書](../../release-plan.md)、
  [1.0ロードマップ](https://github.com/harutasti/cppseed/issues/15)

## 1. 目的

本書は、cppseedの配布アーカイブについて、利用者が次の事項を検証できる状態を
作るための要件を定義する。

1. どのソースとリリースworkflowから生成されたか
2. 配布物にどのファイル、コンポーネントおよび依存関係が含まれるか
3. GitHub Actionsが生成した真正なビルド来歴であるか
4. ダウンロードしたファイルが、来歴およびSBOMの対象と同一であるか

SHA-256チェックサムはファイル同一性を確認できるが、生成主体やビルド入力を
説明しない。本対応では既存の `SHA256SUMS` を維持し、SBOMとartifact
attestationを追加して相互に補完する。

## 2. 用語

| 用語 | 本書での意味 |
|---|---|
| 配布アーカイブ | cppseed実行ファイル、LICENSE、READMEを含むOS・CPU別のtar.gzまたはzip |
| 対象成果物 | 来歴とSBOMを生成する配布アーカイブ。本要件では4個の既存配布アーカイブ |
| SBOM | 対象成果物の構成要素を機械可読形式で記録したSoftware Bill of Materials |
| ビルド来歴 | 対象成果物のdigest、ソース、workflowおよびbuilderを結び付けるSLSA provenance |
| attestation | GitHub ActionsのOIDC identityで署名され、対象成果物に関連付けられた検証可能な声明 |
| offline bundle | GitHubへ接続せずにattestationを検証するための署名・証明書・来歴情報の束 |

## 3. 対象と対象外

### 3.1 対象

- macOS arm64、macOS x86-64、Linux x86-64、Windows x86-64の既存4アーカイブ
- 対象成果物ごとのSPDX JSON SBOM
- 対象成果物ごとのSLSA provenance形式のGitHub artifact attestation
- offline verificationに必要なattestation bundleとtrusted root
- source commit、workflow、runner、compiler、CMakeおよび主要build flagsの記録
- onlineおよびofflineの検証方法
- GitHub Releaseへの検証用ファイル添付
- release workflowの権限分離と失敗時の停止
- 再現可能性の境界の文書化

### 3.2 対象外

- bit-for-bitの再現可能ビルドの保証
- macOSおよびWindowsのコード署名・notarization（Issue #10）
- 対応OS・CPUの追加（Issue #13）
- 脆弱性スキャン結果をリリース可否へ自動反映すること
- GitHub以外のattestationサービスまたは独自PKIの運用
- GitHubが自動生成するsource code zip/tar.gzへのSBOM・来歴付与
- `SHA256SUMS`、SBOM、来歴bundleなどのメタデータ自身を再帰的にattestすること

## 4. 利用者とユースケース

### UC-SC-01: ビルド来歴をonlineで検証する

利用者はGitHub Releaseから取得した配布アーカイブに対して、GitHub CLIを用い、
次を検証できなければならない。

- owner/repositoryが `harutasti/cppseed` である
- attestationのsubject digestが手元のアーカイブと一致する
- 想定したrelease workflowから生成された
- attestationの署名とcertificate chainが有効である

### UC-SC-02: ビルド来歴をofflineで検証する

利用者はReleaseへ添付されたoffline bundleと必要なtrusted rootを事前取得することで、
GitHubへ接続できない環境でも対象成果物の来歴を検証できなければならない。

### UC-SC-03: SBOMを確認する

利用者または自動化ツールは、対象成果物と同じReleaseからSBOMを取得し、
対象アーカイブ、内包ファイル、cppseed version、licenseおよび検出可能な依存関係を
確認できなければならない。

### UC-SC-04: リリース担当者が公開前に検証する

リリース担当者は、draft Release作成前に全対象成果物についてSBOM validationと
attestation verificationが成功したことをworkflow結果から確認できなければならない。

## 5. 機能要件

### FR-SC-001: 対象成果物の一意な識別

4個の対象成果物を、ファイル名とSHA-256 digestの組で一意に識別しなければならない。
SBOM、provenanceおよび `SHA256SUMS` が示すdigestは同一でなければならない。

### FR-SC-002: SBOMの生成単位

対象成果物ごとに1個のSBOMを生成しなければならない。複数platformの情報を1個の
SBOMへ混在させてはならない。

### FR-SC-003: SBOM形式

SBOMは機械可読なSPDX JSONとし、採用するSPDX specification versionを外部設計で
固定しなければならない。各SBOMはそのversionのschemaまたは公式validation手段で
検証可能でなければならない。

### FR-SC-004: SBOMの必須情報

各SBOMは最低限、次を記録しなければならない。

- 一意なdocument namespace
- cppseedの名称とversion
- 対象アーカイブのファイル名とSHA-256 digest
- アーカイブに含まれる実行ファイル、LICENSEおよびREADME
- cppseed本体のMIT License
- SBOM生成ツールの名称とversion
- SBOM生成日時
- platform/architectureを判別できる情報
- 自動検出できた静的・動的依存関係と関係種別

OSが提供するruntimeまたはsystem libraryを完全列挙できない場合は、既存の
dependency inspection結果と互換性境界への参照を含め、未検出を「依存関係なし」と
誤表示してはならない。

### FR-SC-005: SBOMと成果物の対応

SBOM生成は最終アーカイブの作成後に行い、SBOMに記録した対象成果物のdigestを
その後変更してはならない。変更が生じた場合は、SBOMとattestationを再生成する。

### FR-SC-006: ビルド来歴attestation

各対象成果物について、GitHub ActionsがSLSA provenance predicateを含むartifact
attestationを生成しなければならない。subjectは最終アーカイブのファイル名と
SHA-256 digestでなければならない。

### FR-SC-007: ビルド情報

来歴または来歴と結び付いた検証用metadataに、最低限、次を記録しなければならない。

- tagが指すsource commit SHA
- repository URL
- release workflowのpath、refおよびrun identity
- GitHub-hosted runnerのOS、architectureおよびimage情報
- compilerの名称とversion
- CMake version
- Release buildへ影響する主要なCMake optionsおよびbuild flags
- 対象成果物の作成日時とSHA-256 digest

### FR-SC-008: SBOM attestation

各対象成果物のSBOMを、その対象成果物へ結び付けたSBOM attestationとしても
公開しなければならない。attestation内のSBOMとRelease assetとして添付するSBOMは
意味的に同一でなければならない。

### FR-SC-009: Release assets

正式リリースでは、4個の配布アーカイブと `SHA256SUMS` に加えて、最低限、次を
GitHub Releaseへ添付しなければならない。

- 対象成果物ごとのSPDX JSON SBOM
- 対象成果物ごとのoffline verification bundle
- release作成時点のtrusted root
- 対象成果物ごとのbuild metadata（provenanceだけでFR-SC-007を満たせない場合）

ファイル命名規則は対象アーカイブとの対応が機械的に判別でき、versionとtargetを
含まなければならない。

補助build metadataを使用する場合は、そのdigestを署名済みpredicateへ含めるか、
metadata自体を別のattestation subjectとして検証可能にしなければならない。

### FR-SC-010: 公開前の自動検証

draft Release作成前に、全対象成果物について次を自動検証しなければならない。

1. SBOMの構文とschema
2. SBOM記載の名称、version、targetおよびdigest
3. `SHA256SUMS` とのdigest一致
4. build provenance attestationの署名、repository、workflowおよびsubject
5. SBOM attestationとRelease asset SBOMの対応
6. offline bundleを用いた検証
7. trusted rootを用いたoffline verification
8. 補助build metadataを使用する場合、その署名済みdigestとの一致
9. 必須release assetsの個数と命名

1件でも失敗した場合はdraft Releaseを作成してはならない。

### FR-SC-011: dry run

pull requestおよび手動dry runでは、SBOM生成、schema validation、命名、digest対応、
asset集約までを検証しなければならない。

forkを含むpull request workflowにはattestation発行権限またはOIDC token発行権限を
与えてはならない。attestationの発行とend-to-end verificationは、保護されたtagの
正式release jobまたは同等に信頼できるmaintainer起動経路だけで行う。

### FR-SC-012: 利用者向け検証手順

READMEおよびRelease notesへ、コピーして実行できる次の手順を記載しなければならない。

- `SHA256SUMS` の確認
- GitHub CLIによるonline attestation verification
- offline bundleによるverification
- SBOMファイルと対象アーカイブの対応確認
- 検証失敗時に実行を中止し、Issueまたはsecurity reporting経路へ報告する方法

### FR-SC-013: 証跡保持

一時的なActions artifactが期限切れになった後も、GitHub Release、attestation service、
tagおよびworkflow runから、公開済み成果物のSBOMと来歴を検証可能でなければならない。

### FR-SC-014: 既存リリースとの関係

公開済みv0.1.0の成果物を差し替えてはならない。本要件は実装後に新しく作成する
versioned releaseから適用する。過去リリースに後付けする場合は、元の成果物を変更せず、
後日追加したmetadataであることを明記する。

## 6. セキュリティ要件

### SR-SC-001: 最小権限

- workflow全体の既定権限は `contents: read` とする
- attestationを発行するjobだけに `id-token: write` と `attestations: write` を与える
- draft Releaseを作成するjobだけに `contents: write` を与える
- SBOM生成・validation jobには不要なwrite権限を与えない
- `pull_request_target` をattestation発行経路として使用しない

### SR-SC-002: 信頼境界

attestationを発行するjobは、検証済みのmain commitを指す保護されたSemVer tagだけを
処理しなければならない。pull requestのコード、任意refまたは未検証artifactをsubjectに
してはならない。

### SR-SC-003: Actionの固定

SBOMおよびattestation関連Actionは、他のGitHub Actions依存と同様、review済みの
完全なcommit SHAへ固定し、隣接コメントへversionを記載しなければならない。

### SR-SC-004: 生成物の不変性

attestation発行後に対象成果物を再圧縮、改名または変更してはならない。draft Releaseへ
uploadするファイルは、検証済みdigestの同一ファイルでなければならない。

### SR-SC-005: 秘密情報

SBOM、provenance、build metadata、workflow logおよびoffline bundleへ、token、secret、
署名鍵、不要なローカルpathまたは個人情報を含めてはならない。

## 7. 非機能要件

### NFR-SC-001: 標準準拠と相互運用性

SBOMはSPDX、provenanceはSLSA/in-toto、署名bundleはGitHub artifact attestationsが
採用する標準形式を使用する。cppseed独自形式だけに依存してはならない。

### NFR-SC-002: 可読性

SBOMおよび補助metadataはUTF-8 JSONとし、人間がdiffと内容確認を行える形式で
Releaseへ添付する。

### NFR-SC-003: 決定性

同じ入力から生成したSBOMについて、時刻、一意IDおよび生成ツールが許容する項目を
除き、構成要素と関係が安定していなければならない。並び順など制御可能な差異は
抑制する。

### NFR-SC-004: 実行時間

SBOM生成、attestation発行および全検証の追加時間は、通常のtag release workflowで
合計10分以内を目標とする。超過する場合は、原因と運用上許容できる根拠をPRへ記録する。

### NFR-SC-005: 保守性

生成ツール、schema version、Action versionおよびverification commandを設計文書へ
明記し、更新時に影響範囲を確認できなければならない。

## 8. 再現可能性の境界

本対応は、同じsource commitから常に同一digestのアーカイブを再生成できることを
保証しない。少なくとも次を非決定要因として調査し、設計書とRelease notesへ記載する。

- GitHub-hosted runner imageと導入済みtoolchainの更新
- compiler、linker、system SDKおよびsystem libraryの差異
- tar/zip metadata、timestamp、file orderingおよびpermission
- build pathやbuild timestampのbinaryへの混入
- SBOM生成時刻とdocument namespace

「provenanceが存在すること」と「reproducible buildであること」を同義として説明しては
ならない。

## 9. 受け入れ条件

- [ ] 4個すべての配布アーカイブに個別のSPDX JSON SBOMがある
- [ ] 全SBOMが選定したSPDX versionのvalidationを通過する
- [ ] SBOM、`SHA256SUMS`、provenanceのsubject digestが一致する
- [ ] 4個すべての配布アーカイブにbuild provenance attestationがある
- [ ] provenanceがtagのsource commitとrelease workflowを識別する
- [ ] compiler、CMake、runnerおよび主要build flagsを追跡できる
- [ ] online verificationがrepositoryとworkflowを制約した状態で成功する
- [ ] offline bundleによるverificationが成功する
- [ ] pull request workflowが `id-token: write` と `attestations: write` を持たない
- [ ] 検証失敗時にdraft Releaseが作成されない
- [ ] GitHub ReleaseへSBOM、offline bundle、trusted rootおよび必要なbuild metadataが
  添付される
- [ ] READMEとRelease notesにコピー可能な検証手順がある
- [ ] 再現可能・非再現可能な入力境界が文書化されている
- [ ] v0.1.0の公開済み成果物が変更されていない

## 10. 後続工程で決定する事項

次の事項は本要件を変更せず、外部設計で具体化する。

- SPDX specification versionとSBOM生成・validation tool
- SBOM、offline bundle、build metadataのファイル命名規則
- GitHub artifact attestationを発行・download・verifyするActionとCLI version
- 4platformのSBOM生成位置とaggregate jobの責務分担
- build metadataをprovenanceへ含める方法
- trusted rootの取得、固定および更新方法
- 既存release workflowへ追加するjob graph

## 11. 参照資料

- [GitHub Docs: Artifact attestations](https://docs.github.com/en/actions/concepts/security/artifact-attestations)
- [GitHub Docs: Using artifact attestations to establish provenance for builds](https://docs.github.com/en/actions/how-tos/secure-your-work/use-artifact-attestations/use-artifact-attestations)
- [GitHub Docs: Verifying attestations offline](https://docs.github.com/en/actions/how-tos/secure-your-work/use-artifact-attestations/verify-attestations-offline)
- [SPDX Specifications](https://spdx.dev/use/specifications/)
- [SLSA Provenance](https://slsa.dev/spec/v1.2/provenance)
- [Issue #11](https://github.com/harutasti/cppseed/issues/11)

## 12. 変更履歴

| Date | Version | Change | PR |
|---|---:|---|---|
| 2026-07-16 | 0.1 | Initial requirements baseline candidate | [#18](https://github.com/harutasti/cppseed/pull/18) |

## 13. 承認

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Technical | `harutasti` | Pending | — | [PR #18](https://github.com/harutasti/cppseed/pull/18) |
| Security | `harutasti` | Pending | — | [PR #18](https://github.com/harutasti/cppseed/pull/18) |
| Release | `harutasti` | Pending | — | [PR #18](https://github.com/harutasti/cppseed/pull/18) |
