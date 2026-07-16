# <INITIATIVE_NAME> 内部設計書

- 文書バージョン: 0.1
- ステータス: Draft
- 文書オーナー: `<OWNER>`
- 対象Issue: `<ISSUE_URL>`
- 上位設計: `<EXTERNAL_DESIGN_LINK>`
- 作成日: `<YYYY-MM-DD>`

## 1. Component structure

| Component/job/file | Responsibility | Inputs | Outputs | Failure contract |
|---|---|---|---|---|
| `<NAME>` | `<RESPONSIBILITY>` | `<INPUT>` | `<OUTPUT>` | `<FAILURE>` |

## 2. Internal interfaces

### DD-<PREFIX>-001: <TITLE>

- Related design: `<ED IDs>`
- Interface: `<COMMAND_FUNCTION_JOB>`
- Preconditions: `<PRECONDITIONS>`
- Postconditions: `<POSTCONDITIONS>`
- Error handling: `<ERROR_HANDLING>`

## 3. Algorithms and processing order

<DETAILED_STEPS_AND_ATOMICITY>

## 4. Validation rules

| Input/artifact | Rule | Error | Test ID |
|---|---|---|---|
| `<ITEM>` | `<RULE>` | `<ERROR>` | `<TC-ID>` |

## 5. Logging and evidence

<LOG_FIELDS_REDACTION_RETENTION>

## 6. Resource and performance design

<TIMEOUT_CONCURRENCY_STORAGE_LIMITS>

## 7. Dependencies

| Dependency | Version/pin | License | Trust reason | Update method |
|---|---|---|---|---|
| `<NAME>` | `<VERSION>` | `<LICENSE>` | `<REASON>` | `<METHOD>` |

## 8. Failure cleanup and retry

<CLEANUP_RETRY_IDEMPOTENCY_PARTIAL_STATE>

## 9. Design coverage

| External design | Detailed design | Implementation location |
|---|---|---|
| `<ED-ID>` | `<DD-ID>` | `<PATH_OR_JOB>` |

## 10. Approval

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Technical | `<NAME>` | Pending | — | — |
