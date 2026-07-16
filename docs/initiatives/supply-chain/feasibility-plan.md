# G2 SBOM・build provenance技術検証計画書

- 文書バージョン: 0.1
- 対象Gate: G2
- ステータス: In review
- Owner: `harutasti`
- 対象Issue: [#11](https://github.com/harutasti/cppseed/issues/11)
- 上位文書: [要件定義書](requirements.md)
- 作成日: 2026-07-16

## 1. 目的

Linux x86-64の単一archiveを使い、SBOM生成からattestationのonline/offline検証までを
実行する。結果はG3以降の設計入力であり、本検証workflowを正式release実装とは
みなさない。

## 2. 検証範囲

| ID | 検証項目 | 対応要件 | 成功条件 |
|---|---|---|---|
| `FZ-SC-001` | SPDX JSON生成・validation | `FR-SC-003`、`FR-SC-004` | SPDX 2.3を生成し、SPDX Toolsがerrorなしで終了する |
| `FZ-SC-002` | digest対応 | `FR-SC-001`、`FR-SC-005` | archive、SBOM内archive、`SHA256SUMS` が同一SHA-256を示す |
| `FZ-SC-003` | build provenance | `FR-SC-006`、`SR-SC-002` | mainへmerge済みworkflowの非release tag実行でprovenanceを発行できる |
| `FZ-SC-004` | online verification | `FR-SC-010` | repository、workflow、source ref/commit、runner種別を制約して成功する |
| `FZ-SC-005` | offline verification | `FR-SC-002`、`FR-SC-010` | bundleとtrusted rootだけを使い、通信不能proxy下で成功する |
| `FZ-SC-006` | SBOM attestation同一性 | `FR-SC-008` | attestation predicateと保存したSPDX JSONが正規化後に一致する |
| `FZ-SC-007` | PR権限分離 | `FR-SC-011`、`SR-SC-001` | PRで実行されるjobは `contents: read` だけを持つ |
| `FZ-SC-008` | build metadata | `FR-SC-007`、`SR-SC-005` | allowlist方式で環境、tool、flag、source、digestを秘密情報なしに記録する |
| `FZ-SC-009` | 実行時間 | `NFR-SC-004` | trusted jobが20分以内、追加処理の設計目標が10分以内である |

## 3. 固定する検証条件

| 項目 | 固定値 | 検証・選定根拠 |
|---|---|---|
| Runner | `ubuntu-22.04`, x86-64 | 現行Linux release targetと一致させ、Python 3.10 wheelを固定する |
| SBOM形式 | SPDX JSON 2.3 | Syftの既定SPDX JSON出力と要件の機械可読性を満たす |
| SBOM生成 | [Syft v1.46.0](https://github.com/anchore/syft/releases/tag/v1.46.0) | release archive SHA-256をrepository内で固定して検証する |
| SPDX validation | [SPDX Tools v0.8.5](https://github.com/spdx/tools-python/releases/tag/v0.8.5) | 全dependencyとLinux wheel hashをlock fileへ固定する |
| Attestation | [actions/attest v4.1.1](https://github.com/actions/attest/releases/tag/v4.1.1) | 完全commit SHA `a1948c3...` へ固定する |
| Online/offline検証 | GitHub CLI `gh attestation verify` | repository、signer workflow、source、predicateを明示する |

Syftは一般的な配布用tar.gzをfilesystemとして直接走査しない。このため最終archiveを
先に作成し、同じstaging内容をSyftで走査した後、archive自身のSHA-1/SHA-256と包含関係を
SPDXへ追加する。この変換後documentを再validationする。

## 4. Trust boundaryと権限

同じworkflow内でも、PRとattestation発行を別jobに分離する。

- `safe-validation`: PR専用。`contents: read` のみでbuild、SBOM、validationを行う
- `trusted-attestation`: `spike/supply-chain-g2-YYYYMMDD` 形式のannotated tag専用
- trusted jobだけが `id-token: write`、`attestations: write`、
  `artifact-metadata: write` を持つ
- tagのcommitが `origin/main` の祖先であることを発行前に検証する
- tagはGitHub Releaseを作らず、正式な `v*.*.*` release workflowを起動しない
- buildとattestationは同じtrusted jobで実行し、job間artifact差し替え境界を作らない

`artifact-metadata: write` はactions/attest v4.1.1がartifact storage recordを作るために
現在要求する権限である。正式設計ではGitHub側仕様と必要最小権限を再確認する。

## 5. 実行手順

1. 本計画、lock file、検証script、検証workflowをPRでreviewする
2. PRの `safe-validation` と通常CIが成功したことを確認する
3. squash merge後のmain commitへannotated非release tagを作成してpushする
4. `trusted-attestation` でbuild provenanceとSBOM attestationを発行する
5. online verificationと通信不能proxy下のoffline verificationを実行する
6. workflow run、artifact digest、各判定、所要時間およびdeviationを結果報告書へ転記する
7. 別PRで技術・testability・security観点のself-reviewを行い、G2を判定する

## 6. 証跡

workflow artifactへ次を30日保持し、長期証跡は結果報告書へ要約する。

- 検証用archive、`SHA256SUMS`、SPDX JSON
- build metadata、dependency report、tool version
- provenance bundle、SBOM bundle、trusted root
- online/offline verificationのJSON結果
- attestationから抽出したSBOMと全evidenceのSHA-256

Actions artifactの期限切れ後も、attestation service、tag、workflow runおよびrepository内の
結果報告書から判定根拠を追跡できる形にする。

## 7. 失敗時の扱い

- validation、digest、attestationまたはverificationが1件でも失敗した場合、G2は承認しない
- 原因を検証script、要件不足、外部service、tool incompatibilityへ分類する
- 要件変更が必要ならG1へ戻し、承認済み要件を同じPRで暗黙変更しない
- 実験tagは正式releaseではないため、GitHub Releaseや既存assetを変更しない
- 検証完了後、workflowを残すか削除するかをG2結果PRで明示する

## 8. Review checklist

- [ ] Technical: toolの入出力、SPDX変換、digest対応を再現できる
- [ ] Testability: 各 `FZ-SC-*` に自動判定または明示的な手動判定がある
- [ ] Security: PRとtrusted jobの権限、ref validation、Action pinが妥当である
- [ ] Operations: 正式releaseを作らず、失敗時に安全に停止できる
- [ ] Evidence: run URL、commit、artifact digest、所要時間を結果報告書へ残せる
