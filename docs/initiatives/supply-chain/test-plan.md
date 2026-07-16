# cppseed リリース・サプライチェーンテスト計画書

- 文書バージョン: 0.1
- 対象Gate: G7
- ステータス: In review
- Test owner: `harutasti`
- 技術レビュー: Pending
- テスト可能性レビュー: Pending
- セキュリティレビュー: Pending
- リリース承認: Not applicable at G7
- 対象Issue: [#11](https://github.com/harutasti/cppseed/issues/11)
- 上位文書: [要件定義](requirements.md)、[外部設計](external-design.md)、
  [内部設計](detailed-design.md)、[脅威分析](threat-model.md)
- 対応表: [トレーサビリティ・マトリクス](traceability.md)
- Review PR: [#25](https://github.com/harutasti/cppseed/pull/25)
- 作成日: 2026-07-16

## 1. Test objective

本計画は、4platformのrelease supply chainが、承認済みsourceからexactなarchive、SBOM、metadata、
attestation、offline bundle、checksumを作り、検証済み22 assetsだけをdraftへ渡すことを証明するための
test contractを定義する。特に次を測定可能にする。

1. 24件の承認済み要件、14件の外部設計、20件の内部設計、31件の脅威にtestまたは明示的な
   audit/fault drillを対応させる。
2. 正常系だけでなく、permission、tamper、replay、parser、network、publication failureをfail-closedにする。
3. Linux、macOS、Windowsのarchiveとsystem dependency境界を各runnerで確認する。
4. online/offline verificationが同じidentity policyを使い、offline時にnetworkへfallbackしないことを確認する。
5. G9で対象commit、環境、artifact digest、case単位の結果を再現可能なtest reportへ残す。

G7で承認するのはtestの範囲、方法、期待結果およびtraceabilityである。実装済み・実行済み・risk解消済みとは
扱わない。全caseの初期結果は `Planned` とし、G8で自動化、G9/G10/G13で実行・reviewする。

## 2. Scope

### 2.1 In scope

- 4platform target manifest、version、tag、source SHA、required checkのvalidation
- archiveの名称、内容、smoke test、dependency report
- Syft、GitHub CLI、Python validator、GitHub Actionsのpinと安全な取得
- SPDX 2.3のsyntax、model、entity、relationship、verification code、決定性
- build metadata schema、privacy、archive/SBOM/dependency evidenceとのdigest対応
- 3 predicate、online/offline identity policy、target別3-line JSONL bundle
- dry-run 16 assets、release 22 assets、21-line `SHA256SUMS`
- event/job permission、artifact handoff、rerun、draft/cleanup/public immutability
- path、symlink、duplicate JSON key、resource exhaustion、secret canary
- external service outage、trusted root、retention、clean consumer verification

### 2.2 Out of scope

- bit-for-bit reproducible buildの達成。差分境界を確認するが同一digestは要求しない
- source/dependencyに脆弱性や悪意がないことの証明
- macOS/Windows code signingとnotarization（Issue #10）
- GitHub/Sigstore/TUF platform自体へのpenetration test
- v0.1.0 assetの変更。historical immutabilityのread-only確認だけを行う
- production Releaseの自動publish。G9はdraftまで、publishはG11承認後のG12に限定する

## 3. Result, priority, and execution model

### 3.1 Result semantics

| Result | Meaning |
|---|---|
| Planned | G7で方法と期待結果を承認済み、未実装または未実行 |
| Pass | 固定commit・環境で期待結果を満たし、G9/G10/G13 evidenceがある |
| Fail | 観測値が期待結果と異なり、defectを起票した |
| Blocked | environmentまたは外部serviceにより実行不能で、代替Passを主張しない |
| Not run | 計画対象だが未実行。G11 release判断では未達として扱う |

warning、retry後の一部成功、過去runのsuccess、別commitの結果でPassを代替しない。caseの全assertionが同じ
対象commit/runで成立した場合だけPassにする。

### 3.2 Priority

| Priority | Rule |
|---|---|
| P0 | authenticity、integrity、permission、exact asset、draft publicationに関わり、Fail/Blocked/Not runはrelease blocker |
| P1 | compatibility、privacy、availability、retention、consumer safetyに関わり、waiverなしでrelease不可 |
| P2 | performanceまたは運用品質。閾値未達は原因とG11判断を必要とする |

### 3.3 Test levels and types

| Type | Purpose | Automation | Primary gate |
|---|---|---|---|
| Unit/component | strict parser、schema、naming、digest、transformのsmall fixture検証 | G8で自動化 | G8/G9 |
| Integration | build、SBOM、metadata、artifact handoff、aggregateのcomponent間contract | G8で自動化 | G9 |
| End-to-end | protected tag相当からverified draft/consumer verificationまで | trusted environmentのみ | G9/G13 |
| Negative/tamper | 1 field/byte/path/permissionを変更し、対象stageで拒否する | fixture matrix | G8/G9 |
| Security audit | repository settings、workflow permissions、account/platform boundary | API取得 + manual review | G9/G10 |
| Fault drill | outage、partial upload、cleanup、rotation、incident response | sandbox/draft、production変更なし | G9/G10 |
| Compatibility | 4platform archive/dependency/smoke、clean consumer | matrix + manual補助 | G9/G13 |
| Performance/resource | time、size、statement count、timeout境界 | 3run中央値 + boundary fixture | G9 |

## 4. Environment matrix

実行時はrunner labelだけでなく、実際のimage version、OS、CPU、compiler、CMake、Python、Syft、GitHub CLI、
SPDX Tools、Action commit SHAをtest reportへ記録する。

| Environment ID | OS/CPU | Planned toolchain | Purpose | Required evidence |
|---|---|---|---|---|
| `ENV-SC-001` | `ubuntu-24.04` / x86-64 | GCC、CMake、Python 3.10、pinned supply-chain tools | Linux package、validator、aggregate、offline isolation | runner image、全version、command、exit、artifact digest |
| `ENV-SC-002` | `ubuntu-24.04` / x86-64 | Clang、CMake | compiler compatibilityとarchive smoke | compiler/CMake version、ctest、archive digest |
| `ENV-SC-003` | `macos-15` / arm64 | Apple Clang、CMake、`otool` | macOS arm64 package/dependency | image、SDK、compiler、dependency report、digest |
| `ENV-SC-004` | `macos-15-intel` / x86-64 | Apple Clang、CMake、`otool` | macOS x86-64 package/dependency | image、SDK、compiler、dependency report、digest |
| `ENV-SC-005` | `windows-2025` / x86-64 | MSVC、CMake、PowerShell、`dumpbin` | Windows package/dependency | image、MSVC/CMake、dependency report、digest |
| `ENV-SC-006` | fresh GitHub-hosted runner | no repository write、no inherited token where applicable | fork/permission、clean consumer、retention | event、effective permissions、network/token state、commands |
| `ENV-SC-007` | local deterministic fixture harness | pinned Python and no external network | parser、schema、tamper、fault fixtures | fixture digest、test output、coverage |
| `ENV-SC-008` | GitHub API/settings read-only audit | authenticated maintainer read | ruleset、check、account/repository control | redacted API response、audit date、reviewer |

## 5. Fixture and evidence rules

| Fixture ID | Contents | Mutation rule |
|---|---|---|
| `FX-SC-GOOD` | 4 targets、16 dry-run assets、22 release assetsの最小正常集合 | G9 candidateからdigest固定して派生fixtureを作る |
| `FX-SC-CTX` | PR、dispatch、annotated/lightweight tag、source/check API response | 1 fieldずつ変更し、同時mutationで原因を曖昧にしない |
| `FX-SC-SPDX` | raw Syft、final SPDX、archive staging、independent expected model | entity、relationship、checksum、license、namespaceを個別変更 |
| `FX-SC-ATT` | 3 predicates、online response、bundle、trusted roots | subject、source、workflow、runner、predicate、order、rootを個別変更 |
| `FX-SC-PATH` | regular file、symlink、hardlink、FIFO、traversal、duplicate JSON、oversize data | workspace外実fileを参照せずtemporary root内で生成 |
| `FX-SC-PUB` | Release API stateとupload failureのrecord/replay response | production Releaseを変更せずmockまたはunpublished draftだけを使用 |
| `FX-SC-SECRET` | 無効なcanary token/email/path/URL userinfo | 実credentialをfixtureへ入れない |
| `FX-SC-FAULT` | API timeout、rate limit、OIDC/Sigstore/TUF停止、clock/root change | verification skipやcached successを許可しない |

- fixtureは生成script、source file、SHA-256、期待結果をrepositoryへ保持する。
- Actions logだけを長期証跡にせず、G9 `test-report.md` とmachine-readable resultをcommitする。
- secret、token、個人情報、不要なabsolute pathはevidenceへ保存しない。redaction後もcase判定に必要なfieldは残す。
- manual/audit caseは実施者、日時、取得command、観測値、判断理由を記録する。

## 6. Core contract test cases

| ID / title | Coverage | Type / priority | Environment | Preconditions and input | Procedure | Expected result | Planned automation / evidence | Execute |
|---|---|---|---|---|---|---|---|---|
| `TC-SC-001` Target manifest | `FR-SC-002/009/011`, `DD-SC-001` | Unit, P0 | 007 | `FX-SC-CTX`のvalid/unknown/duplicate/missing target | closed schemaでloadし、4 targetとnameを列挙 | validはexact 4、未知key・重複・欠落・未対応runnerはnon-zero | manifest unit result + fixture digest | G8/G9 |
| `TC-SC-002` Version and tag identity | `FR-SC-007/009/014`, `DD-SC-002` | Component, P0 | 001/007 | SemVer、CMake version、notes、mismatch fixture | 全version sourceを比較 | `vX.Y.Z`とCMake/asset/notesが一致した場合だけaccept | context result JSON | G8/G9 |
| `TC-SC-003` Formal tag and source | `FR-SC-006`, `SR-SC-002`, `TH-SC-003/012` | Integration, P0 | 001/008 | annotated/lightweight、main内外tag | tag objectをdereferenceし、HEAD/event/source/main ancestryを比較 | annotated SemVer、exact SHA、main ancestorだけaccept | validation log + ref/API evidence | G9 |
| `TC-SC-004` Required CI identity | `FR-SC-010`, `SR-SC-002`, `TH-SC-004` | Integration, P0 | 007/008 | 8 checksのsuccess/pending/fail/missing/wrong app/stale response | source SHAに対するlatest checkとapp identityを評価 | exact 8件がexpected appでcompleted successの場合だけaccept | API fixture result + ruleset diff | G8/G9 |
| `TC-SC-005` Archive contract | `FR-SC-001/002`, `DD-SC-004` | Integration, P0 | 001–005 | 4 target buildと欠落/余分/permission違いarchive | name、member、version/help、smokeを検査 | target別exact archive、binary/LICENSE/README、smoke成功 | matrix job、member listing、digest | G9 |
| `TC-SC-006` Dependency report | `FR-SC-004`, `RISK-SC-002` | Compatibility, P1 | 001/003/004/005 | platform archiveとallowlist | `ldd`/`otool`/`dumpbin`を独立実行しreportと比較 | unresolved/forbiddenなし、system dependency limitationを明示 | dependency report + command output | G9 |
| `TC-SC-007` Pinned tool acquisition | `SR-SC-003`, `NFR-SC-005`, `TH-SC-008` | Security, P0 | 001/003/004/005/007 | approved URL/version/digest/memberとmutation | download policy、size、digest、safe member、`--version`を確認 | 全constraint一致だけatomic install、異常時fileを残さない | acquisition unit/integration result | G8/G9 |
| `TC-SC-008` SPDX syntax and model | `FR-SC-003`, `NFR-SC-001/002` | Component, P0 | 001/007 | valid/invalid UTF-8/JSON/SPDX fixture | strict parse、SPDX Tools model/schemaでvalidate | SPDX 2.3 UTF-8 JSONだけaccept、warningをPassにしない | validator result + tool versions | G8/G9 |
| `TC-SC-009` SPDX entity contract | `FR-SC-002/004`, `TH-SC-017/018` | Component, P0 | 001/007 | `FX-SC-SPDX` entity/relationship mutation | package/archive/content/dependency集合と関係を独立導出 | package 1、archive 1、content 3、target分離、必須relationship一意 | semantic assertion report | G8/G9 |
| `TC-SC-010` SPDX verification code | `FR-SC-001/004`, `TH-SC-017` | Component, P0 | 001/007 | archive File SHA-1とrelationship mutation | direct archive content File集合からverification code再計算 | 独立再計算値とSPDX値が一致し、集合変更を拒否 | oracle result + fixture digest | G8/G9 |
| `TC-SC-011` Metadata schema and privacy | `FR-SC-007`, `NFR-SC-002`, `SR-SC-005` | Component, P0 | 007 | closed-schema valid/unknown/type/pattern/canary fixture | strict parse、schema、allowlist、privacy patternを実行 | valid v1だけaccept、unknown/secret/path/PIIを拒否 | schema result JSON | G8/G9 |
| `TC-SC-012` Metadata evidence equality | `FR-SC-001/005/007`, `SR-SC-004`, `TH-SC-013/019` | Integration, P0 | 001–005/007 | archive、SBOM、dependency report、build context | filename、size、digest、target、tool/versionを相互比較 | 全evidenceが同じfinal archive/contextを指す | target evidence result | G8/G9 |
| `TC-SC-013` Three attestations | `FR-SC-006/008`, `TH-SC-013/014` | Integration, P0 | 001–005 | provenance、SPDX、metadata predicateとmutation | subject/source/predicate/digestを検証 | targetごとexact 3 predicate、同一archive subject/sourceだけaccept | attestation record | G9 |
| `TC-SC-014` Target JSONL bundle | `FR-SC-009`, `TH-SC-021` | Component, P0 | 007 | `FX-SC-ATT`の欠落/重複/order/unknown/invalid line | strict JSONL parseとpredicate順を検査 | exactly 3 valid lines、固定順、同一subject、unknownなし | bundle assertion result | G8/G9 |
| `TC-SC-015` Online verification | `FR-SC-010/012`, `NFR-SC-001`, `TH-SC-022` | E2E, P0 | 001/006 | 4 archive、12 predicates、expected identity | repository/workflow/source/ref/predicate/runnerを指定してverify | 12/12 successかつsubject digest一致。1 constraint違反でnon-zero | command/result JSON + run URL | G9/G13 |
| `TC-SC-016` Offline verification | `FR-SC-010/012`, `NFR-SC-001`, `TH-SC-022/025` | E2E, P0 | 001/006 | local archive/bundle/root、token unset、blocked network | onlineと同じidentity policyでbundle/rootだけをverify | 12/12 success、network/API accessなし、normalized resultがonline同義 | command/result + network observation | G9/G13 |
| `TC-SC-017` Dry-run exact set | `FR-SC-011`, `SR-SC-001` | Integration, P0 | 001–005 | PR/dispatchの4 target artifacts | exact names、regular file、digest、16 count、side effectを集約 | exact 16、attestation/OIDC/Release writeなし | dry-run manifest + permission summary | G9 |
| `TC-SC-018` Release exact set | `FR-SC-009/010`, `TH-SC-015/016` | Integration, P0 | 001/007 | verified 4 target artifacts、extra/missing/link fixture | basename、regular file、root confinement、exact setを検査 | exact 22だけverified distへatomic copy | release manifest + negative results | G8/G9 |
| `TC-SC-019` SHA256SUMS | `FR-SC-001/009/010`, `TH-SC-026` | Component, P0 | 001/007 | 21 assets、duplicate/unsorted/wrong digest fixture | line count/name/order/unique/digestを独立検査 | 21 sorted unique lines、全asset digest一致、自身を含めない | checksum result + file digest | G8/G9/G13 |
| `TC-SC-020` Draft contract | `FR-SC-010`, `DD-SC-016` | E2E, P0 | 008 + unpublished draft | verified artifact、tag/source、notes | write jobがrepository codeを実行せず22 pathをupload後API確認 | unpublished draft、exact tag/source/22 assets、自動publishなし | draft URL + redacted API snapshot | G9 |
| `TC-SC-021` Partial failure cleanup | `FR-SC-010`, `TH-SC-027` | Fault, P0 | 007/008 | upload N件目失敗、current-run ownership | failure注入後cleanup対象とretry条件を確認 | current-run incomplete draftだけ削除、既存state不変、failureはnon-zero | record/replay log + API snapshot | G9/G10 |
| `TC-SC-022` Effective permissions | `FR-SC-011`, `SR-SC-001`, `TH-SC-001/011` | Security, P0 | 006/008 | fork PR、dispatch、tag job definition | workflow/job permission、event、runner label、token/OIDC attemptを監査 | PR/dispatch read-only、tag attest jobだけOIDC/attest、writerだけcontents write | static lint + fork run + audit | G8/G9 |
| `TC-SC-023` Rerun isolation | `FR-SC-010`, `TH-SC-014/015` | Integration, P0 | 007 | attempt 1/2、orphan、同名artifact fixture | run ID/attempt/artifact digestを固定してaggregate | current attemptだけaccept、old/orphan/name collisionを拒否 | handoff manifest result | G8/G9 |
| `TC-SC-024` Published immutability | `FR-SC-013/014`, `SR-SC-004` | Audit, P0 | 008 | v0.1.0とtest public-state response | asset/delete/update pathとworkflowをread-only監査 | public asset差替え/削除なし、修正はnew version手順のみ | API inventory + workflow lint | G9/G10/G13 |
| `TC-SC-025` Dependency and Action pins | `SR-SC-003`, `NFR-SC-005`, `TH-SC-006/007/009` | Static/security, P0 | 007/008 | workflow、tool config、Python lock | full SHA/comment、binary digest、all transitive hash/marker/licenseを監査 | mutable ref/hash欠落/unsupported markerを拒否、impact review対象を列挙 | pin lint + lock audit | G8/G9 |
| `TC-SC-026` Log and summary confidentiality | `SR-SC-005`, `TH-SC-020` | Security, P1 | 006/007 | `FX-SC-SECRET` | success/failure log、summary、assetsをforbidden pattern scan | canary、credential、email、home/work/temp absolute path、URL userinfoなし | scan result + redacted sample | G8/G9 |
| `TC-SC-027` Resource and performance | `NFR-SC-004`, `TH-SC-016/029` | Boundary/performance, P2 | 001/007 | normal x3、limit±1、statement 29/30/31 | size/time/memory/statement limitとcritical pathを測定 | sizeは上限以下だけ成功、statementは29成功・30以上fail、追加時間3run中央値10分以内またはG11判断 | metrics JSON + job timings | G9 |

## 7. Security, abuse, and operational test cases

| ID / title | Coverage | Type / priority | Environment | Preconditions and input | Procedure | Expected result | Planned automation / evidence | Execute |
|---|---|---|---|---|---|---|---|---|
| `TC-SC-028` Event permission and injection matrix | `FR-SC-011`, `SR-SC-001`, `TH-SC-001/002` | Negative/security, P0 | 006/007 | event matrixとshell metachar/newline payload | context値をenv/argvで渡し、effective permissionとcommandを観測 | payloadはdataとして拒否/保持され、PRにOIDC/write/secretなし | matrix result + fork run | G8/G9 |
| `TC-SC-029` Tag, ruleset, and check audit | `FR-SC-010`, `SR-SC-002`, `TH-SC-003/004`, `RISK-SC-005` | Security audit, P0 | 008 | live settingsとAPI fixtures | tag ruleset、main required checks、app、freshness、bypassを比較 | approved contractと完全一致。差分はrelease blocker | settings snapshot + diff | G9/G10 |
| `TC-SC-030` Tool tamper and extraction | `SR-SC-003`, `TH-SC-008` | Tamper, P0 | 007 | 1-byte change、redirect、absolute/`..`/symlink/duplicate/large member | acquire toolへ各fixtureを個別入力 | 全mutationを実行前に拒否し、temporary executableを残さない | negative matrix | G8/G9 |
| `TC-SC-031` Hash lock and marker matrix | `SR-SC-003`, `NFR-SC-005`, `TH-SC-009`, `RISK-SC-004` | Dependency/security, P0 | 001/007 | missing hash/marker/transitive、wrong platform wheel、offline index | `--require-hashes` offline installとlock completeness audit | approved Python/runnerだけ解決し、全mutationはinstall前失敗 | lock audit + install log | G8/G9 |
| `TC-SC-032` Attestation policy mutation | `FR-SC-006/008/010`, `SR-SC-002`, `TH-SC-011/014/022` | Tamper, P0 | 001/007 | repo/workflow/source/ref/predicate/runner/subjectを1つずつ変更 | online/offlineの同じpolicyで全mutationをverify | valid baselineだけ成功、各mutationは特定constraintでnon-zero | 12-field mutation result | G8/G9 |
| `TC-SC-033` Handoff, bundle, and replay | `FR-SC-005/009/010`, `SR-SC-004`, `TH-SC-012/014/015/021` | Tamper, P0 | 007 | cross-target/run/attempt、duplicate/unknown/old bundle | producer/consumer manifestとexpected contextを比較 | current source/target/attempt/exact bundleだけaccept | replay matrix + manifests | G8/G9 |
| `TC-SC-034` Archive TOCTOU and offline isolation | `FR-SC-005/010`, `SR-SC-004`, `TH-SC-013/025` | Tamper/E2E, P0 | 001/006/007 | SBOM後/attest後/verify後のbyte mutation、blocked proxy | 各boundaryでdigest再計算しoffline commandを監視 | mutationは次boundaryで失敗、offlineはnetwork attemptなし | boundary result + network trace | G8/G9 |
| `TC-SC-035` Draft state and cleanup fault matrix | `FR-SC-010`, `TH-SC-015/027` | Fault, P0 | 007/008 | no state、same-tag draft/public、partial upload、cleanup failure | stateごとwriter/recoveryを実行またはrecord/replay | new unpublished draftだけ作成可、既存/public不変、cleanup failureを隠さない | state table + API evidence | G9/G10 |
| `TC-SC-036` Independent SPDX semantic oracle | `FR-SC-003/004`, `NFR-SC-003`, `TH-SC-017`, `RISK-SC-003` | Component/tamper, P0 | 007 | independent expected entity graphと2回生成、semantic mutation | SPDX Toolsとは別のassertionで集合/関係/license/digest/安定fieldを比較 | 全semantic一致、許容field以外は2runで安定、mutation拒否 | oracle JSON + diff | G8/G9 |
| `TC-SC-037` Strict parser and safe path boundary | `SR-SC-005`, `DD-SC-018`, `TH-SC-016` | Boundary/security, P0 | 007 | `FX-SC-PATH`、24/25/25+ MiB、deep JSON | strict parse、`lstat`、root confinement、resource limitを実行 | regular basename/limit内だけaccept、escape/link/duplicate/oversizeを拒否 | fuzz/fixture result + coverage | G8/G9 |
| `TC-SC-038` Dependency boundary and outage | `FR-SC-004`, `TH-SC-018/029`, `RISK-SC-002` | Compatibility/fault, P1 | 001/003/004/005/007 | platform allowlist、API/tool outage | dependency independent scanと各external failureを注入 | limitationを誤表示せず、outage時はdraftへ進まず診断可能 | dependency diff + fault result | G9 |
| `TC-SC-039` Independent metadata observation | `FR-SC-007`, `TH-SC-019` | Integration/security, P0 | 001–005 | signed metadataとrunner/build outputs | compiler/CMake/runner/options/source/time/digestを独立commandと比較 | allowlist fieldが観測値と一致し、未知/虚偽値を拒否 | comparison JSON | G9 |
| `TC-SC-040` Secret canary and consumer misuse | `FR-SC-012`, `SR-SC-005`, `TH-SC-020/026/031` | Security/usability, P1 | 006/007 | canary、tampered checksum pair、wrong target/source archive | assets/log/docs scan後、copyable commandsを誤対象にも実行 | leakなし。wrong/tampered/unsupported targetは明確に失敗し実行中止を案内 | scan + documentation session | G9/G10 |
| `TC-SC-041` Maintainer and repository control audit | `SR-SC-001/002`, `TH-SC-005/006`, `RISK-SC-005/007` | Manual security audit, P0 | 008 | account/repository/ruleset/workflow inventory | MFA/session/credential scope、direct push/bypass、workflow diff controlをreview | required controlが有効、例外はowner/date付きblocker。credential値は保存しない | signed checklist + redacted snapshot | G10/G11 |
| `TC-SC-042` Hosted runner identity and platform boundary | `FR-SC-007`, `SR-SC-002`, `TH-SC-010/011/028`, `RISK-SC-008` | Audit/integration, P0 | 001–005/008 | provenance claims、runner labels、provider status | hosted runner/source claimを比較しself-hosted mutationをverify | approved labels/hosted claimだけaccept、platform assumptionをreportへ明記 | claim result + platform review | G9/G11 |
| `TC-SC-043` Trusted root substitution and staleness | `FR-SC-010/012`, `TH-SC-023/024`, `RISK-SC-001/006` | Tamper/fault, P0 | 001/006/007 | correct/other/old/corrupt/attacker root、clock skew | bundleを各root/clock条件でoffline verify | wrong/corrupt rootを拒否。old/clock結果とlimitationを記録し無条件Passにしない | root matrix + version/digest | G9/G10 |
| `TC-SC-044` External incident and outage drill | `FR-SC-013`, `TH-SC-010/024/028/029`, `RISK-SC-008` | Fault/manual drill, P1 | 007/008 | GitHub/OIDC/Sigstore/TUF outage/compromise scenario | detection、publish stop、revoke/advisory/retry decisionをtabletop実行 | verification skip/auto publishなし、owner/communication/recovery/evidenceが揃う | drill record | G10/G11 |
| `TC-SC-045` Long-term evidence inventory | `FR-SC-013`, `TH-SC-030` | Audit, P1 | 008 | test release/tag/run/attestation/Release/test report | Actions artifact期限に依存せずsourceからconsumer verifyまで辿る | required evidenceのURL/digest/commitがrepository reportから復元可能 | retention inventory | G9/G13 |
| `TC-SC-046` Clean consumer online/offline E2E | `FR-SC-012/013`, `TH-SC-023/026/031`, `RISK-SC-006/010` | User acceptance, P0 | 006 + Linux/macOS/Windows shell | fresh machine、Release notesだけ、4 target assets | checksum、online、offline、SBOM対応、smokeをcopy/paste実行 | supported OSで手順成功。失敗時は実行中止/reportingへ到達しsource archiveを選ばない | session transcript + downloaded digests | G9 rehearsal / G13 published assets |

## 8. Entry and exit criteria

### 8.1 G7 entry

- [x] G1 requirements、G3 external design、G4 ADR、G5 detailed design、G6 threat modelがApproved
- [x] 24 requirements、14 ED、20 DD、31 threats、10 risksのID集合が固定されている
- [x] `RISK-SC-003/004/005` が未受容release blockerとして記録されている
- [x] Test ownerと3 review観点がinitiative READMEに割り当てられている

### 8.2 G7 exit

- [ ] `TC-SC-001`–`TC-SC-046` が重複・欠番なく定義されている
- [ ] 全requirement、ED、DD、threat、riskにcaseまたは明示的なmanual evidenceがある
- [ ] environment、fixture、expected result、evidence、実行Gateがcaseごとに一意である
- [ ] test exceptionが0件、またはowner/代替control/承認を持つ
- [ ] technical、testability、security self-reviewとCIが成功している
- [ ] test-plan/traceability/initiative READMEが同一PRでApprovedへ更新される

### 8.3 G9 verification exit

- [ ] 対象full commit SHAと全tool/environment versionが固定されている
- [ ] G9実行対象のP0/P1は全件Passし、G10で実行するcaseにはownerと実行条件がある
- [ ] planned 46件のPass/Fail/Blocked/Not runとcase単位evidenceがtest reportにある
- [ ] `RISK-SC-003/004` のblocking evidenceがPassし、`RISK-SC-005` はrelease blockerのままG10へ引き継がれている
- [ ] 4platform archive、22 assets、attestation、draftのdigest chainが一致する
- [ ] defect、deviation、waiver、residual riskがG11へ引き継がれている

### 8.4 G11 pre-release test exit

- [ ] G9/G10までに実行すべきP0/P1が全件Passし、Blocked/Not run/未承認waiverがない
- [ ] `RISK-SC-003/004/005` がevidence付きでclosedし、その他のresidual riskが個別判断されている
- [ ] `TC-SC-046` のpre-release rehearsalがPassし、G13でpublished assetsへ再実行する手順がある
- [ ] P2未達は影響、owner、期限、accept/reject理由がacceptance reportにある

## 9. Defect, retest, and change policy

| Severity | Example | Disposition |
|---|---|---|
| Critical | untrusted codeがOIDC/writeを得る、wrong artifactがvalidとして公開される | 即時停止、release禁止、G3–G6を再review、全security regression再実行 |
| High | tamper/parser/pin/semantic validation迂回、secret漏えい、exact set違反 | release blocker、修正後にcaseと同componentのP0 regressionを再実行 |
| Medium | availability、diagnostic、compatibility、retention不備 | owner/期限付き修正。waiverはG11承認必須 |
| Low | claimを変えない表示・文書不備 | defect記録し、関連documentation/consumer caseを再実行 |

- test code/fixtureの欠陥とproduction codeの欠陥を別IDで管理する。
- fix commitが変わるたび、影響caseとupstream/downstream digest chainを再実行する。
- expected resultを実測へ合わせて弱める場合はG7 baseline変更としてPR reviewする。
- external tool/runner/Action version更新時は `TC-SC-007/025/031/042` と影響caseを再実行する。
- waiverはCritical/Highに使用しない。Medium/Lowも理由、代替control、owner、expiry、G11承認を必要とする。

## 10. Approval

個人運営のため、同一maintainerが観点別self-reviewを分離して記録する。承認対象はcoverage、再現可能な
手順、測定可能な期待結果、evidence設計であり、caseのPassではない。

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Technical | `harutasti` | Pending | — | — |
| Testability | `harutasti` | Pending | — | — |
| Security | `harutasti` | Pending | — | — |

### Review checklist

- 4platform、data format、workflow、permissions、publication、consumerのcontractを検証できる
- 各caseのprecondition、mutation、expected failure stage、evidenceが曖昧でない
- 全Critical/High threatがP0 caseまたは明示的なG10/G11 auditへ対応する
- 実装前のPlannedと実行後のPassを混同していない
- release blockerをtest plan承認だけでclosedしていない
- G8 automation locationとG9/G10/G13実行責務が明確である
