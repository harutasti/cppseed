# <INITIATIVE_NAME> テスト計画書

- 文書バージョン: 0.1
- ステータス: Draft
- Test owner: `<OWNER>`
- 対象Issue: `<ISSUE_URL>`
- 上位要件・設計: `<LINKS>`
- 作成日: `<YYYY-MM-DD>`

## 1. Test objective

<QUALITY_CLAIMS_TO_PROVE>

## 2. Scope

### In scope

- <ITEM>

### Out of scope

- <ITEM_AND_REASON>

## 3. Test levels and types

| Type | Purpose | Environment | Automation | Gate |
|---|---|---|---|---|
| Unit | `<PURPOSE>` | `<ENV>` | Yes/No | `<GATE>` |
| Integration | `<PURPOSE>` | `<ENV>` | Yes/No | `<GATE>` |
| End-to-end | `<PURPOSE>` | `<ENV>` | Yes/No | `<GATE>` |
| Negative/fault | `<PURPOSE>` | `<ENV>` | Yes/No | `<GATE>` |
| Security | `<PURPOSE>` | `<ENV>` | Yes/No | `<GATE>` |
| Compatibility | `<PURPOSE>` | `<ENV>` | Yes/No | `<GATE>` |
| Performance | `<PURPOSE>` | `<ENV>` | Yes/No | `<GATE>` |

## 4. Environment matrix

| Environment ID | OS/CPU | Toolchain | Purpose | Evidence captured |
|---|---|---|---|---|
| `<ENV-ID>` | `<OS_CPU>` | `<VERSIONS>` | `<PURPOSE>` | `<EVIDENCE>` |

## 5. Test cases

### TC-<PREFIX>-001: <TITLE>

- Related requirements: `<IDs>`
- Related threats: `<IDs_OR_NONE>`
- Preconditions: `<PRECONDITIONS>`
- Inputs/fixtures: `<INPUTS>`
- Steps: `<STEPS>`
- Expected result: `<MEASURABLE_RESULT>`
- Evidence: `<LOG_ARTIFACT_REPORT>`
- Automation location: `<PATH_JOB_OR_MANUAL>`

## 6. Entry and exit criteria

### Entry

- [ ] requirementsとdesignがreview済み
- [ ] test environmentとfixtureが再現可能

### Exit

- [ ] planned testが実施済み
- [ ] release blocker defectが0件
- [ ] deviationとwaiverが承認済み
- [ ] traceability coverageが100%

## 7. Defect and retest policy

<SEVERITY_RETEST_REGRESSION_POLICY>

## 8. Approval

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Test | `<NAME>` | Pending | — | — |
| Technical | `<NAME>` | Pending | — | — |
