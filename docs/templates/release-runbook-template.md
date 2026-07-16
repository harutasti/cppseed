# <INITIATIVE_OR_RELEASE> リリースrunbook

- 文書バージョン: 0.1
- ステータス: Draft
- Runbook owner: `<OWNER>`
- 対象release: `<VERSION>`
- 最終dry run: `<DATE_AND_EVIDENCE>`

## 1. Preconditions

- [ ] acceptance reportがGo
- [ ] required CIとrelease dry runが成功
- [ ] release blockerが0件
- [ ] version、CHANGELOG、release notesが一致
- [ ] rollback/revocation担当者が明確

## 2. Required access

| Operation | Role | Permission | Credential/identity | Verification |
|---|---|---|---|---|
| `<OPERATION>` | `<ROLE>` | `<PERMISSION>` | `<IDENTITY>` | `<CHECK>` |

## 3. Release procedure

| Step | Command/action | Expected result | Evidence | Stop condition |
|---:|---|---|---|---|
| 1 | `<ACTION>` | `<RESULT>` | `<EVIDENCE>` | `<CONDITION>` |

## 4. Post-release verification

- [ ] Release asset count、name、digestを確認
- [ ] clean environmentでinstall/verify/smoke test
- [ ] support boundaryとknown issuesを確認
- [ ] initiative READMEとIssueを更新

## 5. Rollback and withdrawal

| Scenario | Immediate action | User communication | Recovery release | Owner |
|---|---|---|---|---|
| `<SCENARIO>` | `<ACTION>` | `<COMMUNICATION>` | `<PLAN>` | `<OWNER>` |

公開済みtagまたは同一versionのartifactを置き換えない。修正は新versionで行う。

## 6. Security incident and revocation

<COMPROMISED_IDENTITY_ATTESTATION_KEY_ACTION_DEPENDENCY_RESPONSE>

## 7. Rehearsal record

| Date | Commit | Operator | Result | Evidence | Follow-up |
|---|---|---|---|---|---|
| `<DATE>` | `<SHA>` | `<NAME>` | `<RESULT>` | `<LINK>` | `<ISSUE_OR_NONE>` |
