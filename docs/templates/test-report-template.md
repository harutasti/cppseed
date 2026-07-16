# <INITIATIVE_NAME> テスト結果報告書

- 文書バージョン: 0.1
- ステータス: Draft
- Test owner: `<OWNER>`
- 対象commit: `<FULL_SHA>`
- 対象release: `<VERSION_OR_NONE>`
- 実施期間: `<START>` — `<END>`
- Test plan: `<LINK>`

## 1. Executive result

- Overall result: `<PASS|FAIL|BLOCKED>`
- Release recommendation: `<GO|NO-GO|CONDITIONAL>`
- Planned/executed/passed/failed/blocked: `<COUNTS>`
- Open release blockers: `<COUNT_AND_LINKS>`

## 2. Environment evidence

| Environment ID | OS/CPU/runner image | Compiler/CMake/tools | Workflow run |
|---|---|---|---|
| `<ENV-ID>` | `<VERSIONS>` | `<VERSIONS>` | `<URL>` |

## 3. Test results

| Test case | Environment | Result | Evidence | Defect/deviation |
|---|---|---|---|---|
| `<TC-ID>` | `<ENV-ID>` | Pass/Fail/Blocked/Not run | `<URL_PATH>` | `<ID_OR_NONE>` |

## 4. Quality metrics

| Metric | Gate | Result | Status | Evidence |
|---|---:|---:|---|---|
| Line coverage | `<TARGET>` | `<RESULT>` | Pass/Fail | `<LINK>` |
| Branch coverage | `<TARGET>` | `<RESULT>` | Pass/Fail | `<LINK>` |
| Performance | `<TARGET>` | `<RESULT>` | Pass/Fail | `<LINK>` |

## 5. Artifact evidence

| Artifact | SHA-256 | Producing job | Verification |
|---|---|---|---|
| `<NAME>` | `<DIGEST>` | `<JOB_URL>` | `<RESULT>` |

## 6. Defects, deviations, and residual risks

| ID | Severity | Description | Disposition | Owner | Approval |
|---|---|---|---|---|---|
| `<ID>` | `<SEVERITY>` | `<DESCRIPTION>` | `<FIX/WAIVE/BLOCK>` | `<OWNER>` | Pending |

## 7. Approval

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Test | `<NAME>` | Pending | — | — |
| Release | `<NAME>` | Pending | — | — |
