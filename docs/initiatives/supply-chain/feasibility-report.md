# G2 SBOM・build provenance技術検証結果報告書

- 文書バージョン: 0.1
- 対象Gate: G2
- ステータス: Approved
- Owner: `harutasti`
- 対象Issue: [#11](https://github.com/harutasti/cppseed/issues/11)
- 計画書: [G2技術検証計画書](feasibility-plan.md)
- 機械可読証跡: [feasibility-evidence.json](feasibility-evidence.json)
- 検証基盤: [PR #19](https://github.com/harutasti/cppseed/pull/19)
- 結果承認PR: Under review
- 実施日: 2026-07-16

## 1. 結論

計画した `FZ-SC-001` から `FZ-SC-009` はすべてPassした。Linux x86-64の単一archiveで、
SPDX 2.3 SBOM生成・validation、digest対応、SLSA build provenance、SBOM attestation、
repository/workflow/sourceを制約したonline verification、bundleとtrusted rootによる
offline verificationが実現可能である。

G2をApprovedとし、G3外部設計へ進める。これは技術方式の成立判定であり、4platformの
正式release workflowへ実装済み、またはIssue #11を完了したという判定ではない。

## 2. 対象と環境

| 項目 | 実績 |
|---|---|
| Source commit | `b74235fff27d1f34f809ba1d31f61515b5c8f473` |
| Annotated tag | `spike/supply-chain-g2-20260716` |
| Tag object | `aaf7d50144357de4721cdf3db7125e03e61f4693` |
| Main CI | [Run 29467829471](https://github.com/harutasti/cppseed/actions/runs/29467829471)、成功後にtag作成 |
| Feasibility run | [Run 29467892590](https://github.com/harutasti/cppseed/actions/runs/29467892590) |
| Trusted job | [Job 87524812395](https://github.com/harutasti/cppseed/actions/runs/29467892590/job/87524812395)、40秒、Success |
| Runner | GitHub-hosted `ubuntu-22.04` / x86-64、image `20260705.219.1` |
| Compiler / CMake | GCC 11.4.0 / CMake 3.31.6 |
| SBOM | Syft 1.46.0、SPDX JSON 2.3 |
| Validation | SPDX Tools 0.8.5 / Python 3.10.12 |
| Attestation / verification | actions/attest 4.1.1 / GitHub CLI 2.96.0 |

tagは正式releaseの `v*.*.*` と一致せず、GitHub Releaseを作成していない。公開済み
v0.1.0とそのassetに変更はない。

## 3. 検証結果

| ID | Result | 実測結果・証跡 |
|---|---|---|
| `FZ-SC-001` | Pass | SyftでSPDX 2.3 JSONを生成し、変換後documentがSPDX Tools 0.8.5でerrorなし |
| `FZ-SC-002` | Pass | archive、SPDX内archive、`SHA256SUMS` が `b105684b...fd08` で一致 |
| `FZ-SC-003` | Pass | mainへmerge済みworkflowのannotated tagからSLSA provenanceを発行 |
| `FZ-SC-004` | Pass | repository、signer workflow、source ref、source digest、GitHub-hosted runnerを制約してonline検証成功 |
| `FZ-SC-005` | Pass | tokenを除去し、全proxyを通信不能endpointへ設定した状態でlocal bundleとtrusted rootによる検証成功 |
| `FZ-SC-006` | Pass | SBOM attestationから抽出したpredicateと保存SPDXをkey sort後にbyte比較して一致 |
| `FZ-SC-007` | Pass | PR runでは `safe-validation` だけが実行され、write権限を持つtrusted jobはSkip |
| `FZ-SC-008` | Pass | allowlist方式でsource、workflow、runner、compiler、CMake、flags、tool、digestを記録し、credentialとworkspace pathなし |
| `FZ-SC-009` | Pass | PR job 35秒、trusted job 40秒。10分の設計目標以内 |

## 4. Digestと成果物

対象archiveは
`cppseed-g2-v0.1.0-x86_64-unknown-linux-gnu.tar.gz`、SHA-256は
`b105684b30073ee148423939cffb0c98f998e996d862df9ce6714f7a1280fd08` である。

| Evidence | SHA-256 |
|---|---|
| SPDX JSON | `29e2249ef10ec66a30a96310d347107e1e71640444fea6c4ad6ad5fdbb31455b` |
| Build metadata | `6519e687b7bce72b69063efbae10e6a879824af32e890885523c7a035d4547cb` |
| Provenance bundle | `4fd8bd16b7fd9b1e959f587b1a61bdae3f379be49febb6ba1ef06069e3c1dac3` |
| SBOM bundle | `cfdf88cbf08b035c0cf7333c9c792088a6c38967c56cb876e3eb39c67019bdf5` |
| Trusted root | `65ca537f6ed8a47fd0e560c421baa1f6c1efb8b25fc200d8c5c02c0e92eb2b9c` |
| Online/offline provenance verification | 両方 `883dcd96...5180` |
| Online/offline SBOM verification | 両方 `266d2821...a829` |

workflow artifactはID `8363640681`、artifact自体のSHA-256は
`efb199fa33bd6abdad606e71645aec29031fa24e3d480b7026783fb5422ebe2f` である。

## 5. Attestation identity

| 項目 | 検証値 |
|---|---|
| Provenance | [Attestation 35567643](https://github.com/harutasti/cppseed/attestations/35567643) |
| SBOM | [Attestation 35567646](https://github.com/harutasti/cppseed/attestations/35567646) |
| Signer workflow | `harutasti/cppseed/.github/workflows/supply-chain-feasibility.yml` |
| Source ref | `refs/tags/spike/supply-chain-g2-20260716` |
| Source/workflow digest | `b74235fff27d1f34f809ba1d31f61515b5c8f473` |
| Subject digest | `b105684b30073ee148423939cffb0c98f998e996d862df9ce6714f7a1280fd08` |
| Runner environment | `github-hosted` |
| OIDC issuer | `https://token.actions.githubusercontent.com` |
| Transparency log | Rekor、2026-07-16 03:00:06Z / 03:00:07Z |

GitHub Actions内のonline/offline検証後、取得したarchiveを別processのGitHub CLIからも
同じrepository、workflow、source ref/digest、runnerおよびpredicate type制約で再検証した。

## 6. 権限とtrust boundary

- workflow既定権限とPR jobは `contents: read` のみ
- PR runでは `trusted-attestation` が実行されず、OIDC tokenを要求するstepが存在しない
- tag runでは `safe-validation` がSkipし、trusted jobだけが発行権限を持った
- trusted jobはtag形式、annotated tag、public repositoryおよびmain ancestryを発行前に確認した
- build、SBOM、attestationおよびverificationを同じtrusted jobで行い、未信頼artifactのjob間受け渡しを避けた
- actions/attest、checkout、upload-artifactは完全commit SHAへ固定した
- Syft archiveと全Python wheelはSHA-256を固定した

actions/attest 4.1.1では `id-token: write`、`attestations: write` に加えて
`artifact-metadata: write` が必要だった。正式設計でこの権限とstorage recordの関係を固定する。

## 7. Deviationと原因分析

### DEV-SC-G2-001: Python 3.10限定dependencyのlock漏れ

初回PR run [29467586457](https://github.com/harutasti/cppseed/actions/runs/29467586457) は、
`ModuleNotFoundError: isodate` で失敗した。

- 原因: lock生成環境はPython 3.14だったが、runnerはPython 3.10だった
- 詳細: rdflib 7.6.0はPython 3.11未満でのみisodateを要求する
- 是正: wheel metadataを全件監査し、isodate 0.7.2とwheel SHA-256をlockへ追加した
- 再発防止入力: production lockは対象runner/Python環境で生成し、environment markerをCIで検証する
- 再実行: [Run 29467691033](https://github.com/harutasti/cppseed/actions/runs/29467691033) で成功

## 8. G3以降への設計入力

1. SPDX 2.3、Syft 1.46.0、SPDX Tools 0.8.5の組合せを候補として採用できる
2. Syftは一般的な配布tar.gzを展開済みfilesystemとして直接走査しないため、最終archive作成後にstagingを走査し、archive file、digest、包含関係をSPDXへ追加する必要がある
3. 独自SPDX変換は専用testを持ち、archive、内部file、package verification codeの意味を外部設計で固定する
4. 補助build metadataはallowlist生成できるが、metadata自身をattestする方式を `DEC-SC-002` で決める必要がある
5. offline bundleとtrusted rootはActions artifactではなく、正式Releaseへ長期保存する必要がある
6. trusted root snapshotの取得時点、更新、revocationおよび利用者への配布規則を `DEC-SC-003` とthreat modelで扱う
7. SBOMのpackage検出だけでsystem dependency完全性を主張せず、`ldd` 等のplatform検査を併用する
8. productionでは保護されたSemVer tagだけを許可し、同じtrusted job内でbuildからattestationまで完結させる

## 9. 残存risk

| ID | 内容 | G2判定 | 次の処置 |
|---|---|---|---|
| `RISK-SC-001` | trusted rootが更新・revocationへ追随しない | Open、G2 blockerではない | G3/G6で取得・更新・incident手順を設計 |
| `RISK-SC-002` | scannerがsystem dependencyを完全検出しない | Open、実測で再確認 | G3でSBOMとplatform dependency reportの役割を分離 |
| `RISK-SC-003` | SPDX post-processingのsemantic誤り | New、G2 blockerではない | G3/G7でdata modelとnegative testを定義 |
| `RISK-SC-004` | dependency lockのenvironment marker漏れ | Mitigated for spike | 対象環境lock生成とCI dependency auditを設計 |

Critical/Highの未処置blockerはない。残存riskを受容して実装へ進むのではなく、G3から
G7の設計・threat・test工程でcontrolを定義する。

## 10. 証跡保持

- repository: 本報告書、機械可読証跡、計画書、PR #19、source/tag
- GitHub: workflow run、2件のattestation、transparency log entry
- 一時artifact: 2026-08-15 03:00:18Zまで保持

G2の一時artifact期限切れは正式要件 `FR-SC-013` の実装ではない。正式releaseではarchive、
SBOM、bundle、trusted rootをRelease assetとして保持し、本検証の一時artifactへ依存しない。

## 11. Gate review

個人運営のため、結果承認PR上で観点別self-reviewとして記録する。

| Role | Result | 根拠 |
|---|---|---|
| Technical | Approved | tool互換、SPDX model、digest、attestation identityを再検証 |
| Testability | Approved | 全 `FZ-SC-*` の自動判定、failure記録、再実行を確認 |
| Security | Approved | 権限分離、ref/source制約、Action/tool固定、offline境界を確認 |
| Release | Not applicable | 非release技術検証であり、製品公開を行っていない |

## 12. Cleanup判定

G2で使用したtag-only write経路は目的を完了したため、本結果PRでmainから削除する。
`spike/supply-chain-g2-20260716` はhistorical evidenceとして保持する。

PR専用の `contents: read` に限定したSBOM生成・validation job、scriptおよびlock fileは、
G3以降の設計変更に対する実行可能な回帰checkとして残す。production releaseへの統合は、
承認済み外部設計、ADR、内部設計、threat modelおよびtest planが揃うまで行わない。
