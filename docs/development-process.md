# cppseed 開発・品質保証プロセス

- 文書バージョン: 1.0
- ステータス: Active
- プロセスオーナー: repository maintainer
- 発効日: 2026-07-16
- 承認根拠: [PR #18](https://github.com/harutasti/cppseed/pull/18)

## 1. 目的

本書は、cppseedの変更について、要求、設計、実装、検証、承認およびリリースの
証跡を第三者が追跡・再現できる開発プロセスを定義する。

文書量そのものを品質とみなさない。次の4点を成立させることを目的とする。

1. すべての変更理由と承認者を追跡できる
2. すべての承認済み要件が設計とテストへ対応する
3. リリース可否を実行結果と未解決riskから説明できる
4. 実装者以外が文書だけで検証・リリース・障害対応できる

## 2. 適用範囲

### 2.1 Full lifecycle

次のいずれかに該当する変更は、本書の全工程を適用する。

- 公開CLI、終了コード、生成物または互換性の変更
- release workflow、配布形式、署名、SBOMまたはprovenanceの変更
- security boundary、権限、secretまたは外部通信の変更
- 対応OS、CPU、compiler、CMakeまたはruntime境界の変更
- 外部dependencyまたはpackage managerの追加
- データ消失、任意コード実行または供給網riskを持つ変更
- 1.0 roadmapのrelease gateに関係する変更

### 2.2 Tailored lifecycle

誤字修正、リンク修正、内部refactorなど、公開contractとriskを変更しない小規模変更は
工程を短縮できる。短縮する場合もPRへ次を記録する。

- full lifecycleを適用しない理由
- 影響分析
- 実施した検証
- reviewerの承認

security、releaseまたは互換性へ影響する可能性が不明な場合はfull lifecycleを選ぶ。

## 3. 成果物の配置

full lifecycle対象の変更は、次の構成で管理する。

```text
docs/initiatives/<initiative>/
├── README.md                 # 工程台帳、責任者、承認記録
├── requirements.md           # 要件定義
├── external-design.md        # 外部設計
├── detailed-design.md        # 内部設計
├── threat-model.md           # 脅威分析
├── test-plan.md              # テスト計画
├── traceability.md           # 要件・設計・テスト・証跡対応
├── test-report.md            # 実施結果
├── release-runbook.md        # リリース・rollback・障害対応
├── acceptance-report.md      # 受け入れ判定
└── adr/
    └── NNNN-<decision>.md    # initiative固有の設計判断
```

未着手の成果物を空ファイルで作成しない。工程開始時にtemplateから作成し、
initiativeのREADMEで `Not started` として管理する。

## 4. 開発ライフサイクル

| Gate | 工程 | 必須成果物・証跡 | Exit criteria |
|---:|---|---|---|
| G0 | 起案 | Issue、initiative README | owner、目的、scope、risk分類が明確 |
| G1 | 要件定義 | requirements.md | requirement ID、対象外、受け入れ条件が承認済み |
| G2 | 技術検証 | spike結果または実現性記録 | 未知事項、制約、採否が記録済み |
| G3 | 外部設計 | external-design.md | public contract、workflow、data/asset形式が承認済み |
| G4 | ADR | 重要判断のADR | 選択肢、理由、欠点、見直し条件が承認済み |
| G5 | 内部設計 | detailed-design.md | component、interface、error handlingが実装可能 |
| G6 | 脅威分析 | threat-model.md | asset、trust boundary、threat、mitigationがreview済み |
| G7 | テスト設計 | test-plan.md、traceability.md | 全要件にtestまたは承認済み例外がある |
| G8 | 実装 | code、test、user docs | review可能な差分、local check成功 |
| G9 | 検証 | CI、test-report.md | 計画済みtest実施、逸脱とdefectを記録 |
| G10 | 運用準備 | release-runbook.md | release、rollback、revocation、incident手順が再現可能 |
| G11 | 受け入れ | acceptance-report.md | gate、未解決risk、承認者、release判断が明確 |
| G12 | 統合・公開 | protected merge、tag、Release | required checks成功、承認済みcommitを公開 |
| G13 | 公開後確認 | Release evidence、Issue更新 | install/verify/smoke test成功、残課題を起票 |

前工程の成果が後工程で誤りと判明した場合は前工程へ戻り、文書revisionとPR履歴を
残す。工程を戻ることは失敗ではなく、baselineを無断で変更することが失敗である。

## 5. Gate運用

### 5.1 Entry criteria

各工程を開始する前に、initiative READMEで次を確認する。

- 直前Gateが `Approved` または明示的なconditional approvalである
- 未解決事項とassumptionが記録されている
- ownerとreview roleが割り当てられている
- 変更対象branchとIssueが一意である

### 5.2 Exit criteria

Gateを完了するには次を満たす。

- 必須成果物がrepositoryへcommitされている
- review指摘が解決またはrisk acceptanceされている
- initiative READMEにPR、commit、承認日を記録している
- downstream成果物への影響を確認している
- 文書内のstatusを `Approved` または `Completed` へ更新している

### 5.3 Conditional approval

条件付き承認は、条件、owner、期限および未達時の扱いを記録した場合だけ許可する。
CriticalまたはHigh security risk、データ損失risk、未検証のrelease artifactを条件付きで
公開してはならない。

## 6. 文書管理

### 6.1 Status

成果物では次のstatusだけを使用する。

| Status | 意味 |
|---|---|
| Draft | 作成中でbaselineではない |
| In review | review中で変更可能 |
| Approved | 承認済みbaseline |
| Implemented | 設計内容が実装済み |
| Verified | 対応する検証が完了 |
| Superseded | 後続文書に置き換え済み |
| Withdrawn | 採用せず終了 |

### 6.2 必須metadata

各成果物は、文書version、status、owner、review role、作成日、対象Issueおよび上位文書を
冒頭に記載する。承認済み成果物は承認PR、commit SHAおよび承認日を追記する。

### 6.3 Baselineと変更管理

- `Approved` 文書の意味を変える変更は新しいPRで行う
- PR bodyへ変更理由、影響するrequirement/test/releaseを記載する
- 公開済みreleaseのhistorical recordは後続状態へ書き換えない
- 要件変更後はtraceability、test plan、user docsへの影響を再評価する
- 削除した要件IDを別の意味で再利用しない

## 7. 識別子とトレーサビリティ

initiativeごとに短い大文字prefixを定める。SBOM/provenance initiativeでは `SC` を使う。

| 種類 | 形式 | 例 |
|---|---|---|
| 機能要件 | `FR-<PREFIX>-NNN` | `FR-SC-001` |
| セキュリティ要件 | `SR-<PREFIX>-NNN` | `SR-SC-001` |
| 非機能要件 | `NFR-<PREFIX>-NNN` | `NFR-SC-001` |
| 外部設計 | `ED-<PREFIX>-NNN` | `ED-SC-001` |
| 内部設計 | `DD-<PREFIX>-NNN` | `DD-SC-001` |
| Threat | `TH-<PREFIX>-NNN` | `TH-SC-001` |
| Test case | `TC-<PREFIX>-NNN` | `TC-SC-001` |
| Risk | `RISK-<PREFIX>-NNN` | `RISK-SC-001` |

traceability matrixは、すべての承認済み要件について、設計、test case、CI jobまたは
手動手順、結果証跡および最終判定を結び付ける。testを持たない要件には、理由と
risk acceptanceを記録する。

## 8. Review role

| Role | 責務 |
|---|---|
| Author | 成果物作成、自己review、指摘対応 |
| Technical reviewer | 実現性、設計整合、保守性のreview |
| Test reviewer | test coverage、traceability、証跡のreview |
| Security reviewer | trust boundary、権限、threat、mitigationのreview |
| Release approver | 未解決risk、runbook、release gateの承認 |

個人運営では同一人物が複数roleを兼務できる。ただし、どの観点でいつreviewしたかを
別々に記録し、存在しない第三者承認を装ってはならない。

## 9. Testと証跡

### 9.1 Test分類

test planは最低限、該当する次の分類を検討する。

- unit、component、integration、end-to-end
- normal、boundary、negative、fault injection
- compatibility、upgrade、rollback
- security、permission、tamper resistance
- performance、coverage、static analysis、sanitizer
- release packaging、installation、user acceptance

### 9.2 証跡

test reportは対象commitと環境を固定し、次を残す。

- workflow run URLとjob名
- OS、CPU、runner image、compiler、CMake、tool version
- test caseごとのPass/Fail/Blocked/Not run
- coverageとperformance結果
- defect、deviation、waiverおよび未解決risk
- artifact名とdigest

Actions logだけを長期証跡にしない。releaseごとに要約したtest reportをrepositoryへ残す。

## 10. Pull requestとmerge gate

- mainへ直接pushしない
- 1つのPRは1つの工程またはreview可能な変更単位を原則とする
- PR templateで対象Gate、変更成果物、traceability、検証、riskを申告する
- required CIが成功するまでReadyまたはmergeにしない
- unresolved review、未説明のtest failure、Critical/High riskがあるPRをmergeしない
- squash merge後のcommit SHAをinitiative READMEへ記録する

文書PRでは実装完了を主張せず、実装PRでは未承認要件を既成事実として扱わない。

## 11. Definition of Done

full lifecycle変更は、次をすべて満たしてDoneとする。

- [ ] 承認済み要件がすべてtraceability matrixに存在する
- [ ] 設計とADRが実装内容に一致する
- [ ] threat mitigationが実装・検証済みである
- [ ] planned testが完了し、test reportが承認済みである
- [ ] CI、compatibility、coverage、performance gateが成功している
- [ ] user docs、CHANGELOG、support boundaryが更新されている
- [ ] release、rollback、incident手順が再現できる
- [ ] 未解決riskが明示的にacceptまたはrelease blockerとして処理されている
- [ ] protected mainへ承認済みPRから統合されている
- [ ] 公開後確認が完了し、残課題がIssue化されている

## 12. 例外と継続改善

process上の例外はPRへ理由、影響、代替control、承認者および解消予定を記録する。
「時間がない」だけを恒久的な例外理由にしない。

release後またはincident後にprocess不足が判明した場合は、再発防止をIssue化し、
本書、templateまたはCI checkへ反映する。

## 13. 承認

個人運営のため同一maintainerがroleを兼務した。独立reviewではなく、観点別の
self-reviewとして記録する。

| Role | Reviewer | Result | Date | Evidence |
|---|---|---|---|---|
| Process owner | `harutasti` | Approved | 2026-07-16 | [PR #18](https://github.com/harutasti/cppseed/pull/18) |
| Quality/process review | `harutasti` | Approved | 2026-07-16 | [PR #18](https://github.com/harutasti/cppseed/pull/18) |
