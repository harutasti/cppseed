# <INITIATIVE_NAME> 受け入れ判定書

- 文書バージョン: 0.1
- ステータス: Draft
- Decision owner: `<OWNER>`
- 対象commit: `<FULL_SHA>`
- 対象release: `<VERSION>`
- 判定日: `<YYYY-MM-DD>`

## 1. Decision

- Decision: `<GO|NO-GO|CONDITIONAL>`
- Scope accepted: `<SCOPE>`
- Conditions: `<CONDITIONS_OR_NONE>`

## 2. Gate status

| Gate | Required evidence | Result | Evidence | Approver |
|---:|---|---|---|---|
| G1 Requirements | Approved requirements | `<RESULT>` | `<LINK>` | `<NAME>` |
| G3-G6 Design/security | Approved design, ADR, threat model | `<RESULT>` | `<LINKS>` | `<NAME>` |
| G7-G9 Test | Traceability and test report | `<RESULT>` | `<LINKS>` | `<NAME>` |
| G10 Operations | Rehearsed runbook | `<RESULT>` | `<LINK>` | `<NAME>` |

## 3. Acceptance criteria

| Criterion | Result | Evidence | Notes |
|---|---|---|---|
| `<CRITERION>` | Pass/Fail/Blocked | `<LINK>` | `<NOTES>` |

## 4. Unresolved items and risk acceptance

| ID | Severity | Description | Decision | Owner | Review/expiry |
|---|---|---|---|---|---|
| `<ID>` | `<SEVERITY>` | `<DESCRIPTION>` | `<ACCEPT/BLOCK>` | `<OWNER>` | `<DATE>` |

## 5. Approval

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Technical | `<NAME>` | Pending | — | — |
| Test | `<NAME>` | Pending | — | — |
| Security | `<NAME>` | Pending | — | — |
| Release | `<NAME>` | Pending | — | — |
