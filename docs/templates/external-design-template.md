# <INITIATIVE_NAME> 外部設計書

- 文書バージョン: 0.1
- ステータス: Draft
- 文書オーナー: `<OWNER>`
- 対象Issue: `<ISSUE_URL>`
- 上位要件: `<REQUIREMENTS_LINK>`
- 作成日: `<YYYY-MM-DD>`

## 1. Design objective

<OBJECTIVE_AND_REQUIREMENT_COVERAGE>

## 2. System context

<ACTORS_SYSTEMS_AND_BOUNDARIES>

## 3. Public contract

### ED-<PREFIX>-001: <TITLE>

- Related requirements: `<FR/SR/NFR IDs>`
- Inputs: `<INPUTS>`
- Outputs: `<OUTPUTS>`
- Errors: `<ERRORS>`
- Compatibility: `<COMPATIBILITY>`

## 4. Workflow and state transition

<SEQUENCE_STATES_RETRY_IDEMPOTENCY>

## 5. Data and artifact formats

| Artifact | Format/schema | Naming | Retention | Integrity |
|---|---|---|---|---|
| `<NAME>` | `<FORMAT>` | `<RULE>` | `<PERIOD>` | `<CONTROL>` |

## 6. Permission and trust boundary

| Component/job | Read | Write | Identity/secret | Justification |
|---|---|---|---|---|
| `<COMPONENT>` | `<SCOPE>` | `<SCOPE>` | `<IDENTITY>` | `<WHY>` |

## 7. Failure behavior

| Failure | Detection | User/operator impact | Recovery | Release blocking |
|---|---|---|---|---|
| `<FAILURE>` | `<DETECTION>` | `<IMPACT>` | `<RECOVERY>` | Yes/No |

## 8. Compatibility and migration

<BACKWARD_COMPATIBILITY_ROLLOUT_MIGRATION>

## 9. Requirement coverage

| Requirement | External design | Notes |
|---|---|---|
| `<ID>` | `<ED-ID>` | `<NOTES>` |

## 10. Approval

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Technical | `<NAME>` | Pending | — | — |
| Security | `<NAME>` | Pending | — | — |
