# cppseed リリース・サプライチェーントレーサビリティ・マトリクス

- 文書バージョン: 0.1
- 対象Gate: G7
- ステータス: In review
- Owner: `harutasti`
- 技術レビュー: Pending
- テスト可能性レビュー: Pending
- セキュリティレビュー: Pending
- 対象commit/release: G8 implementation candidate / first release after implementation
- 対象Issue: [#11](https://github.com/harutasti/cppseed/issues/11)
- Test plan: [テスト計画書](test-plan.md)
- 上位文書: [要件定義](requirements.md)、[外部設計](external-design.md)、
  [内部設計](detailed-design.md)、[脅威分析](threat-model.md)
- Review PR: [#25](https://github.com/harutasti/cppseed/pull/25)
- 更新日: 2026-07-16

## 1. Reading and status rules

- `Planned` はG7でtest/evidence pathを承認した状態であり、test成功を意味しない。
- `Result evidence` はG8で実装場所を確定し、G9/G10/G13でURL、full SHA、digest、report rowへ置き換える。
- 複数caseのうち1件がPassしてもrequirement全体をPassにしない。列挙したmandatory caseとmanual evidenceが
  すべて成立した場合だけG11でSatisfiedにする。
- `RISK-SC-003/004/005` は本matrixにtest pathがあってもopen release blockerのままである。
- `/` で並べたIDは個別IDを省略した範囲ではなく、すべてmandatoryな対応先を表す。

## 2. Requirement coverage

| Requirement | Approved design | Threat / risk | Mandatory test cases | Planned execution | Result evidence | G7 status |
|---|---|---|---|---|---|---|
| `FR-SC-001` | `ED-SC-001/002/004`; `DD-SC-003/012/015` | `TH-SC-013/026` | `TC-SC-005/012/019/033/034` | component + 4platform integration | G9 digest-chain rows | Planned |
| `FR-SC-002` | `ED-SC-002/005`; `DD-SC-001/004/006` | `TH-SC-017/018` | `TC-SC-001/005/009` | unit + 4platform | G9 manifest/archive/SPDX rows | Planned |
| `FR-SC-003` | `ED-SC-005`; `DD-SC-005/006/008` | `TH-SC-017`, `RISK-SC-003` | `TC-SC-008/036` | component + independent oracle | G9 SPDX validation rows | Planned |
| `FR-SC-004` | `ED-SC-003/005`; `DD-SC-004/006/008` | `TH-SC-017/018`, `RISK-SC-002/003` | `TC-SC-006/009/010/036/038` | 4platform + semantic/fault | G9 entity/dependency rows | Planned |
| `FR-SC-005` | `ED-SC-002/005/011`; `DD-SC-004/006/008/011/015` | `TH-SC-012/013` | `TC-SC-012/033/034` | integration + boundary tamper | G9 handoff/digest rows | Planned |
| `FR-SC-006` | `ED-SC-007/008`; `DD-SC-009/013` | `TH-SC-012/014/022` | `TC-SC-003/013/032` | trusted tag integration | G9 provenance/identity rows | Planned |
| `FR-SC-007` | `ED-SC-006/007`; `DD-SC-007/008/009/019` | `TH-SC-019/020` | `TC-SC-002/011/012/039/042` | 4platform observation + schema | G9 metadata comparison rows | Planned |
| `FR-SC-008` | `ED-SC-005/007/012`; `DD-SC-006/008/009/013/014` | `TH-SC-017/021/022` | `TC-SC-013/015/016/032/036` | semantic + online/offline | G9 SBOM attestation rows | Planned |
| `FR-SC-009` | `ED-SC-003/007/009`; `DD-SC-003/010/012/015` | `TH-SC-015/021/026/027` | `TC-SC-001/014/018/019/023/033` | aggregate + tamper | G9 exact-set/checksum rows | Planned |
| `FR-SC-010` | `ED-SC-010/011/012/013`; `DD-SC-002/008/009/010/011/012/013/014/015/016/017` | `TH-SC-003/004/013/014/015/021/022/025/027` | `TC-SC-004/012/013/014/015/016/018/019/020/021/023/027/029/032/033/034/035/043` | integration + trusted E2E + fault | G9 pre-draft verification rows | Planned |
| `FR-SC-011` | `ED-SC-010/011`; `DD-SC-002/004/011/015/020` | `TH-SC-001/002` | `TC-SC-001/017/022/028` | fork/dispatch dry-run | G9 permission/dry-run rows | Planned |
| `FR-SC-012` | `ED-SC-008/009/014`; `DD-SC-013/014/019` | `TH-SC-022/023/025/026/031`, `RISK-SC-006/010` | `TC-SC-015/016/019/040/043/046` | consumer E2E + negative docs | G9/G10/G13 transcripts | Planned |
| `FR-SC-013` | `ED-SC-003/009/014`; `DD-SC-010/012/019` | `TH-SC-030` | `TC-SC-024/044/045/046` | audit + incident + post-release | G10/G13 inventory | Planned |
| `FR-SC-014` | `ED-SC-001/014`; `DD-SC-002/016/019` | `TH-SC-027/031` | `TC-SC-002/024/040` | read-only historical audit | G9/G10 API inventory | Planned |
| `SR-SC-001` | `ED-SC-010`; `DD-SC-002/009/016/020` | `TH-SC-001/002/005`, `RISK-SC-007` | `TC-SC-017/022/025/028/041` | static + fork + manual audit | G9/G10 permission evidence | Planned |
| `SR-SC-002` | `ED-SC-001/010`; `DD-SC-002/009/013/014` | `TH-SC-003/004/010/011/012`, `RISK-SC-005/008` | `TC-SC-003/004/022/029/032/041/042` | tag/identity/settings audit | G9/G10 trust-boundary rows | Planned |
| `SR-SC-003` | `ED-SC-005/007`; `DD-SC-005/020` | `TH-SC-007/008/009`, `RISK-SC-004` | `TC-SC-007/025/030/031` | static + acquisition/lock tamper | G9 pin/dependency rows | Planned |
| `SR-SC-004` | `ED-SC-002/011/012`; `DD-SC-004/008/011/015` | `TH-SC-013/015` | `TC-SC-012/018/024/033/034` | digest boundary + audit | G9 immutability rows | Planned |
| `SR-SC-005` | `ED-SC-006/010`; `DD-SC-007/008/018/019` | `TH-SC-016/020` | `TC-SC-011/026/037/040` | schema/path/canary scan | G9 privacy/safe-path rows | Planned |
| `NFR-SC-001` | `ED-SC-005/007`; `DD-SC-006/009/013/014` | `TH-SC-017/022` | `TC-SC-008/015/016/036` | standard tools + semantic oracle | G9 interoperability rows | Planned |
| `NFR-SC-002` | `ED-SC-005/006`; `DD-SC-006/007/018` | `TH-SC-016/020/021` | `TC-SC-008/011/014/037` | strict UTF-8 JSON tests | G9 parser/schema rows | Planned |
| `NFR-SC-003` | `ED-SC-005`; `DD-SC-006` | `TH-SC-017`, `RISK-SC-003` | `TC-SC-009/010/036` | two-run normalized semantic diff | G9 determinism report | Planned |
| `NFR-SC-004` | `ED-SC-011/014`; `DD-SC-017/019`, detailed design section 6 | `TH-SC-016/029` | `TC-SC-027/038` | boundary + 3-run median | G9 metrics/fault rows | Planned |
| `NFR-SC-005` | `ED-SC-005/007`; `DD-SC-005/020`, detailed design section 7 | `TH-SC-007/008/009/010` | `TC-SC-007/025/031/042` | pin/lock/platform impact audit | G9 dependency inventory | Planned |

## 3. External design coverage

| External design | Detailed design | Mandatory test cases | Planned implementation/evidence | G7 status |
|---|---|---|---|---|
| `ED-SC-001` | `DD-SC-002/016` | `TC-SC-002/003/020/024/029` | context validator、draft API evidence | Planned |
| `ED-SC-002` | `DD-SC-001/003/004` | `TC-SC-001/005/018/019` | target/package/asset assertions | Planned |
| `ED-SC-003` | `DD-SC-003/012/015` | `TC-SC-018/019/045` | aggregate manifest、retention inventory | Planned |
| `ED-SC-004` | `DD-SC-003/012/015` | `TC-SC-019/034` | checksum and boundary digest results | Planned |
| `ED-SC-005` | `DD-SC-005/006/008` | `TC-SC-007/008/009/010/025/030/036` | tool/SPDX/semantic test suites | Planned |
| `ED-SC-006` | `DD-SC-007/008/018` | `TC-SC-011/012/026/037/039` | metadata/schema/privacy results | Planned |
| `ED-SC-007` | `DD-SC-009/010` | `TC-SC-013/014/025/032/033` | attestation/bundle results | Planned |
| `ED-SC-008` | `DD-SC-005/013` | `TC-SC-007/015/032/046` | pinned CLI online verification | Planned |
| `ED-SC-009` | `DD-SC-010/012/014` | `TC-SC-014/016/018/019/043/046` | bundle/root/offline/consumer evidence | Planned |
| `ED-SC-010` | `DD-SC-001/002/009/020` | `TC-SC-001/003/004/017/022/025/028/029/041` | workflow lint、fork run、settings audit | Planned |
| `ED-SC-011` | `DD-SC-016`, detailed design sections 1.2/3 | `TC-SC-012/020/021/023/027/034/035` | job graph、state/fault evidence | Planned |
| `ED-SC-012` | `DD-SC-008/013/014/015` | `TC-SC-012/013/014/015/016/018/019/032/033/034` | pre-publication verifier results | Planned |
| `ED-SC-013` | `DD-SC-016/017` | `TC-SC-020/021/023/024/035` | draft/idempotency/cleanup evidence | Planned |
| `ED-SC-014` | `DD-SC-019/020`, detailed design section 5 | `TC-SC-024/026/027/040/044/045/046` | report、docs、runbook、consumer evidence | Planned |

## 4. Internal design coverage

| Internal design | Contract under test | Mandatory test cases | G7 status |
|---|---|---|---|
| `DD-SC-001` | closed target manifest and matrix | `TC-SC-001/017/018` | Planned |
| `DD-SC-002` | event/source/version/tag/check context | `TC-SC-002/003/004/017/022/028/029` | Planned |
| `DD-SC-003` | generated names and exact sets | `TC-SC-001/005/018/019` | Planned |
| `DD-SC-004` | platform build/package/dependency | `TC-SC-005/006/009/012/034/038` | Planned |
| `DD-SC-005` | pinned safe tool acquisition | `TC-SC-007/025/030/031` | Planned |
| `DD-SC-006` | SPDX transformation and semantics | `TC-SC-008/009/010/036` | Planned |
| `DD-SC-007` | build metadata generation | `TC-SC-011/012/026/039` | Planned |
| `DD-SC-008` | target evidence validation | `TC-SC-008/009/010/011/012/018/033/036/039` | Planned |
| `DD-SC-009` | attestation generation and permission | `TC-SC-013/022/025/032` | Planned |
| `DD-SC-010` | bundle assembly | `TC-SC-013/014/033/043` | Planned |
| `DD-SC-011` | Actions artifact handoff | `TC-SC-017/018/023/033/034` | Planned |
| `DD-SC-012` | release aggregation and trusted root | `TC-SC-018/019/033/043` | Planned |
| `DD-SC-013` | online identity verification | `TC-SC-015/032/046` | Planned |
| `DD-SC-014` | offline verification and isolation | `TC-SC-016/025/032/034/043/046` | Planned |
| `DD-SC-015` | final exact release-set verification | `TC-SC-012/018/019/033/034` | Planned |
| `DD-SC-016` | draft creation and ownership | `TC-SC-020/021/024/035` | Planned |
| `DD-SC-017` | idempotency and rerun identity | `TC-SC-021/023/024/035` | Planned |
| `DD-SC-018` | strict data and safe path | `TC-SC-011/014/018/026/037` | Planned |
| `DD-SC-019` | diagnostics, exit, summary | `TC-SC-021/026/027/035/038/040/044` | Planned |
| `DD-SC-020` | workflow pins and change control | `TC-SC-022/025/028/029/031/041/042/045` | Planned |

## 5. Threat coverage

| Threat | Verification cases | Evidence type | Residual decision gate | G7 status |
|---|---|---|---|---|
| `TH-SC-001` | `TC-SC-022/028` | static permission + fork run | G9 | Planned |
| `TH-SC-002` | `TC-SC-028` | injection fixture matrix | G9 | Planned |
| `TH-SC-003` | `TC-SC-003/029` | tag fixture + live settings | G10 | Planned |
| `TH-SC-004` | `TC-SC-004/029` | check API fixture + ruleset diff | G10 | Planned |
| `TH-SC-005` | `TC-SC-041` | maintainer security audit | G11 | Planned |
| `TH-SC-006` | `TC-SC-025/041` | pin/change-control audit | G11 | Planned |
| `TH-SC-007` | `TC-SC-025` | Action pin lint/review | G9 | Planned |
| `TH-SC-008` | `TC-SC-007/030` | acquisition tamper matrix | G9 | Planned |
| `TH-SC-009` | `TC-SC-025/031` | lock/marker/offline install | G9 | Planned |
| `TH-SC-010` | `TC-SC-042/044` | runner claim + incident drill | G11 | Planned |
| `TH-SC-011` | `TC-SC-022/032` | runner label/policy mutation | G9 | Planned |
| `TH-SC-012` | `TC-SC-003/033` | SHA/ref/replay matrix | G9 | Planned |
| `TH-SC-013` | `TC-SC-012/013/034` | boundary digest tamper | G9 | Planned |
| `TH-SC-014` | `TC-SC-013/023/032` | subject/run/policy replay | G9 | Planned |
| `TH-SC-015` | `TC-SC-018/033/035` | artifact/draft state matrix | G9/G10 | Planned |
| `TH-SC-016` | `TC-SC-018/037` | path/parser/resource fixtures | G9 | Planned |
| `TH-SC-017` | `TC-SC-008/009/010/036` | official validator + independent oracle | G9 | Planned |
| `TH-SC-018` | `TC-SC-006/038` | independent dependency inspection | G9 | Planned |
| `TH-SC-019` | `TC-SC-011/012/039` | metadata independent observation | G9 | Planned |
| `TH-SC-020` | `TC-SC-026/040` | canary and forbidden-pattern scan | G9 | Planned |
| `TH-SC-021` | `TC-SC-014/033` | strict bundle/replay matrix | G9 | Planned |
| `TH-SC-022` | `TC-SC-015/016/032` | online/offline policy mutation | G9 | Planned |
| `TH-SC-023` | `TC-SC-043/046` | root substitution + consumer E2E | G11 | Planned |
| `TH-SC-024` | `TC-SC-043/044` | stale root/clock + incident drill | G11 | Planned |
| `TH-SC-025` | `TC-SC-016/034` | offline network observation | G9 | Planned |
| `TH-SC-026` | `TC-SC-019/040/046` | tampered checksum pair + user E2E | G11/G13 | Planned |
| `TH-SC-027` | `TC-SC-020/021/024/035` | draft/public/cleanup state matrix | G10 | Planned |
| `TH-SC-028` | `TC-SC-042/044` | platform boundary + incident drill | G11 | Planned |
| `TH-SC-029` | `TC-SC-038/044` | outage fault injection/runbook | G11 | Planned |
| `TH-SC-030` | `TC-SC-045` | long-term evidence inventory | G13 | Planned |
| `TH-SC-031` | `TC-SC-040/046` | documentation negative + clean consumer | G13 | Planned |

## 6. Risk disposition coverage

| Risk | Owner | Verification / mitigation evidence | Decision required | Current disposition |
|---|---|---|---|---|
| `RISK-SC-001` | Security owner | `TC-SC-043/044`; root digest/version、clock/rotation、incident drill | G10/G11 and every release | Conditional acceptance only; open |
| `RISK-SC-002` | Technical owner | `TC-SC-006/038`; 4platform independent dependency report | G9 and tool/runner update | Accepted limitation; verify disclosure |
| `RISK-SC-003` | Technical owner | `TC-SC-008/009/010/036`; official + independent semantic oracle | G9 | **Not accepted; release blocker** |
| `RISK-SC-004` | Technical owner | `TC-SC-025/031`; full hashes、markers、platform offline install | G9 and dependency update | **Not accepted; release blocker** |
| `RISK-SC-005` | Security owner | `TC-SC-003/004/029/041`; live tag/main ruleset and bypass audit | G10 before trusted tag path | **Not accepted; release blocker** |
| `RISK-SC-006` | Security owner | `TC-SC-043/046`; root bootstrap limitation and clean consumer guidance | G10/G11/every release | Conditional acceptance deferred |
| `RISK-SC-007` | Security owner | `TC-SC-041`; account/repository control checklist | G10/G11 | Residual acceptance deferred |
| `RISK-SC-008` | Security owner | `TC-SC-042/044`; platform claims/status/incident drill | G11/every release | Inherited integrity risk deferred |
| `RISK-SC-009` | Security owner | `TC-SC-032/040/046`; identity policy and safety-claim wording | G10/G11 | Accepted scope boundary; disclose |
| `RISK-SC-010` | Documentation owner | `TC-SC-040/046`; wrong-source/weak-policy and clean consumer session | G10/G11/G13 | Residual acceptance deferred |

## 7. Test exceptions

G7 baselineにdirect test、automated assertion、manual audit、fault drillのいずれも持たないrequirementはない。
したがって、現在のtest exceptionは0件である。後続で実行不能caseが生じた場合、`Not run`をPassへ変更せず、
次をすべて満たす行を追加する。

| Requirement / case | Reason no direct execution | Alternative control/evidence | Risk owner | Expiry | Approval |
|---|---|---|---|---|---|
| None | — | — | — | — | — |

Critical/High security risk、P0 case、exact 22 assets、permission、attestation identity、draft fail-closedにはexceptionを
認めない。Medium/Lowの例外もG11承認なしでreleaseできない。

## 8. Coverage summary

| Metric | Target | G7 actual | Status |
|---|---:|---:|---|
| Approved requirements traced to design | 100% | 24/24 (100%) | Complete |
| Approved requirements traced to test/evidence path | 100% | 24/24 (100%) | Complete |
| Security requirements traced to threats and tests | 100% | 5/5 (100%) | Complete |
| External designs traced to DD and tests | 100% | 14/14 (100%) | Complete |
| Internal designs traced to tests | 100% | 20/20 (100%) | Complete |
| Threats traced to verification and decision gate | 100% | 31/31 (100%) | Complete |
| Risks traced to evidence, owner, and decision gate | 100% | 10/10 (100%) | Complete |
| Planned test cases defined without gaps | 100% | 46/46 (100%) | Complete |
| Planned test cases executed | 100% at G9/G10/G13 | 0/46 (0%) | Not due at G7 |
| Unapproved test exceptions | 0 | 0 | Complete |

## 9. Change control and downstream handoff

- G8は各caseに実file/function/jobを追記する。設計名と異なる場合はG5/G7を同じPRでrevisionする。
- G9はcase行を上書きして結果を隠さず、`test-report.md` に対象full SHA、environment、result、evidence、
  defect/deviationを記録する。
- G10は `TC-SC-021/024/029/035/040/041/043/044` をrunbook手順へ対応させる。
- G11はcoverage 100%だけでGOにせず、case resultと全open riskを個別判断する。
- G13は `TC-SC-015/016/019/024/045/046` を実際のpublished assetsに対して再実行する。
- requirement、ED、DD、threat、risk、caseを追加・削除・意味変更した場合は全ID集合とcoverage summaryを再計算する。

## 10. Approval

個人運営のため、同一maintainerが観点別self-reviewを分けて記録する。G7承認はmatrixの完全性と
test/evidence pathの妥当性に対するもので、実行結果やrelease readinessの承認ではない。

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Technical | `harutasti` | Pending | — | — |
| Testability | `harutasti` | Pending | — | — |
| Security | `harutasti` | Pending | — | — |

### Review checklist

- 24 requirements、14 ED、20 DD、31 threats、10 risksのID集合に欠落・余分がない
- 46 caseがtest planの内容、priority、environment、evidence、Gateと一致する
- test exceptionとmanual testが自動化済みに見せかけられていない
- Critical/High threatと未受容riskが明示的なP0/blocker pathを持つ
- G8/G9/G10/G11/G13の責務とbaseline更新条件が一意である
