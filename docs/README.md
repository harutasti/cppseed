# Engineering documentation

このディレクトリは、cppseedの製品仕様、設計、検証証跡およびリリース運用を
管理する。

## Governing process

- [開発・品質保証プロセス](development-process.md)
- [文書テンプレート](templates/README.md)
- [Architecture Decision Records](adr/README.md)

## Active initiatives

- [SBOM and build provenance](initiatives/supply-chain/README.md)

各initiativeは、要件からリリース後確認までの工程台帳と成果物を
`docs/initiatives/<initiative>/` にまとめる。

## Historical v0.1.0 baseline

- [要件定義書](requirements.md)
- [外部設計書](basic-design.md)
- [内部設計書](detailed-design.md)
- [テスト計画書](test-plan.md)
- [リリース設計書](release-plan.md)
- [v0.1.0 Release notes](releases/v0.1.0.md)

これらはv0.1.0時点の承認済みbaselineである。後続変更の内容へ書き換えず、
新しいinitiativeまたはversioned release文書から参照する。
