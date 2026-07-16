# SBOM and build provenance initiative

- Initiative ID: `SC`
- Overall status: In progress
- Owner: `harutasti`
- Primary Issue: [#11](https://github.com/harutasti/cppseed/issues/11)
- Roadmap: [#15](https://github.com/harutasti/cppseed/issues/15)
- Target release: Undecided; first release after implementation
- Risk classification: High
- Process: [開発・品質保証プロセス](../../development-process.md)

## 1. Objective

cppseedの各配布アーカイブについて、構成要素、source、builderおよびrelease workflowを
利用者が検証できるSBOMとbuild provenanceを提供する。

## 2. Scope

### In scope

- 既存4platformのrelease archive
- SPDX SBOM
- GitHub artifact attestationsとSLSA provenance
- online/offline verification
- 最小権限、tamper detectionおよびrelease evidence
- release runbookと公開後確認

### Out of scope

- code signingとnotarization（Issue #10）
- 新しいOS/CPU target（Issue #13）
- bit-for-bit reproducible buildの保証

## 3. Roles

個人運営のため、現時点ではmaintainerが各roleを兼務する。各Gateでは観点別の
self-review結果をPRへ分けて記録し、未実施reviewを承認済みとして扱わない。

| Role | Assignee | Status |
|---|---|---|
| Author | `harutasti` | Assigned |
| Technical reviewer | `harutasti` | G5 review in progress |
| Test reviewer | `harutasti` | G5 review in progress |
| Security reviewer | `harutasti` | G5 review in progress |
| Release approver | `harutasti` | Pending |

## 4. Gate register

| Gate | Deliverable | Status | PR/commit/evidence | Approval |
|---:|---|---|---|---|
| G0 | Initiative | Approved | [PR #18](https://github.com/harutasti/cppseed/pull/18) | Approved 2026-07-16 |
| G1 | [Requirements](requirements.md) | Approved | [PR #18](https://github.com/harutasti/cppseed/pull/18) | Approved 2026-07-16 |
| G2 | [Feasibility report](feasibility-report.md) | Approved | [PR #19](https://github.com/harutasti/cppseed/pull/19), [PR #20](https://github.com/harutasti/cppseed/pull/20), [Run 29467892590](https://github.com/harutasti/cppseed/actions/runs/29467892590) | Approved 2026-07-16 |
| G3 | [External design](external-design.md) | Approved | [PR #21](https://github.com/harutasti/cppseed/pull/21), [CI run 29471390567](https://github.com/harutasti/cppseed/actions/runs/29471390567) | Approved 2026-07-16 |
| G4 | [ADR-0001](adr/0001-spdx-2-3-toolchain.md)、[ADR-0002](adr/0002-signed-build-metadata-predicate.md)、[ADR-0003](adr/0003-offline-bundle-and-trusted-root.md) | Approved | [PR #22](https://github.com/harutasti/cppseed/pull/22), [CI run 29472918679](https://github.com/harutasti/cppseed/actions/runs/29472918679) | Approved 2026-07-16 |
| G5 | [Detailed design](detailed-design.md) | In review | Pending PR | Pending |
| G6 | Threat model | Not started | — | — |
| G7 | Test plan and traceability | Not started | — | — |
| G8 | Implementation | Not started | — | — |
| G9 | Test report | Not started | — | — |
| G10 | Release runbook | Not started | — | — |
| G11 | Acceptance report | Not started | — | — |
| G12 | Merge and release | Not started | — | — |
| G13 | Post-release verification | Not started | — | — |

## 5. Planned feasibility checks

G2では、Linux x86-64の単一archiveを用いて次を検証する。結果は後続設計の入力とし、
要件変更が必要ならG1へ戻す。

1. SPDX JSONの生成とschema validation
2. archive、SBOM、`SHA256SUMS` のdigest対応
3. protected workflowからのbuild provenance発行
4. repositoryとworkflowを制約したonline verification
5. bundleとtrusted rootを用いたoffline verification
6. PR jobがOIDC/attestation発行権限を持たないこと
7. compiler、CMake、runner、build flagsの安全な記録方法

## 6. Open decisions and risks

| ID | Type | Description | Owner | Due | Status |
|---|---|---|---|---|---|
| `DEC-SC-001` | Decision | SPDX versionと生成・validation tool | Technical reviewer | G4 | [ADR-0001](adr/0001-spdx-2-3-toolchain.md) Accepted |
| `DEC-SC-002` | Decision | provenanceへ補助build metadataを結び付ける方法 | Security reviewer | G4 | [ADR-0002](adr/0002-signed-build-metadata-predicate.md) Accepted |
| `DEC-SC-003` | Decision | offline bundleとtrusted rootの命名・配布・更新 | Security reviewer | G4 | [ADR-0003](adr/0003-offline-bundle-and-trusted-root.md) Accepted |
| `RISK-SC-001` | Risk | trusted rootが更新・revocation情報に追随しない可能性 | Security reviewer | G6 | Open |
| `RISK-SC-002` | Risk | SBOM scannerがsystem dependencyを完全に検出できない | Technical reviewer | G3 | Control selected; verify G7 |
| `RISK-SC-003` | Risk | SPDX post-processingのsemantic誤り | Technical reviewer | G7 | Open |
| `RISK-SC-004` | Risk | dependency lockのenvironment marker漏れ | Technical reviewer | G3 | Control selected; verify G7 |

## 7. Approval record

| Gate | Role | Reviewer | Result | Date | PR/commit |
|---:|---|---|---|---|---|
| G0-G1 | Technical | `harutasti` | Approved | 2026-07-16 | [PR #18](https://github.com/harutasti/cppseed/pull/18) |
| G0-G1 | Testability | `harutasti` | Approved | 2026-07-16 | [PR #18](https://github.com/harutasti/cppseed/pull/18) |
| G0-G1 | Security | `harutasti` | Approved | 2026-07-16 | [PR #18](https://github.com/harutasti/cppseed/pull/18) |
| G2 | Technical | `harutasti` | Approved | 2026-07-16 | [PR #20](https://github.com/harutasti/cppseed/pull/20) |
| G2 | Testability | `harutasti` | Approved | 2026-07-16 | [PR #20](https://github.com/harutasti/cppseed/pull/20) |
| G2 | Security | `harutasti` | Approved | 2026-07-16 | [PR #20](https://github.com/harutasti/cppseed/pull/20) |
| G3 | Technical | `harutasti` | Approved | 2026-07-16 | [PR #21](https://github.com/harutasti/cppseed/pull/21) |
| G3 | Testability | `harutasti` | Approved | 2026-07-16 | [PR #21](https://github.com/harutasti/cppseed/pull/21) |
| G3 | Security | `harutasti` | Approved | 2026-07-16 | [PR #21](https://github.com/harutasti/cppseed/pull/21) |
| G4 | Technical | `harutasti` | Approved | 2026-07-16 | [PR #22](https://github.com/harutasti/cppseed/pull/22) |
| G4 | Testability | `harutasti` | Approved | 2026-07-16 | [PR #22](https://github.com/harutasti/cppseed/pull/22) |
| G4 | Security | `harutasti` | Approved | 2026-07-16 | [PR #22](https://github.com/harutasti/cppseed/pull/22) |

## 8. Change history

| Date | Change | PR/commit | Author |
|---|---|---|---|
| 2026-07-16 | Initiative開始、要件定義とprocess frameworkを作成 | [PR #18](https://github.com/harutasti/cppseed/pull/18) | `harutasti` |
| 2026-07-16 | G0・G1の観点別self-reviewを完了 | [PR #18](https://github.com/harutasti/cppseed/pull/18) | `harutasti` |
| 2026-07-16 | G2技術検証計画と権限分離した検証基盤を作成 | [PR #19](https://github.com/harutasti/cppseed/pull/19) | `harutasti` |
| 2026-07-16 | G2全項目をPassし、結果報告と一時write経路cleanupを作成 | [PR #20](https://github.com/harutasti/cppseed/pull/20) | `harutasti` |
| 2026-07-16 | G3外部設計のreviewを開始 | [PR #21](https://github.com/harutasti/cppseed/pull/21) | `harutasti` |
| 2026-07-16 | G3外部設計の観点別self-reviewを完了しbaselineを承認 | [PR #21](https://github.com/harutasti/cppseed/pull/21) | `harutasti` |
| 2026-07-16 | G4で3件のArchitecture Decision Recordのreviewを開始 | [PR #22](https://github.com/harutasti/cppseed/pull/22) | `harutasti` |
| 2026-07-16 | G4の3件のADRを観点別self-reviewしAcceptedとして承認 | [PR #22](https://github.com/harutasti/cppseed/pull/22) | `harutasti` |
| 2026-07-16 | G5内部設計のreviewを開始 | Pending PR | `harutasti` |
