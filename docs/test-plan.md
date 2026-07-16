# cppseed テスト計画書

- 文書バージョン: 0.1
- 対象リリース: cppseed v0.1.0
- ステータス: Approved
- 承認・完了根拠: PR #4のmainへのマージ、PR #5およびPR #7のCI成功
- 作成日: 2026-07-13
- 上位文書: [要件定義書](requirements.md)、[外部設計書](basic-design.md)、[内部設計書](detailed-design.md)

## 1. 目的

本書は、cppseed v0.1.0が要件定義、外部設計および内部設計を満たすことを確認するためのテスト方針、テストレベル、環境、ケース、合格基準および工程ゲートを定義する。

テストの中心は次の2点とする。

1. CLIと生成処理が、入力・失敗条件を含めて仕様どおり動作すること
2. 生成されたC++17、C++20、C++23プロジェクトが実際に構成、ビルド、実行、テストできること

## 2. テスト対象

### 2.1 対象

- cppseed実行ファイル
- `cppseed_core` の各モジュール
- CLI解析
- プロジェクト名の検証と正規化
- 診断情報の生成と表示
- テンプレートカタログと描画
- ファイルシステム境界
- 生成トランザクションとロールバック
- 生成されたCMakeプロジェクト
- OS別リリースアーカイブ
- GitHub Actionsのビルド・テスト結果

### 2.2 対象外

- v0.1.0で提供しないオプションやプリセット
- Eigen、GoogleTest、Conan、vcpkgとの連携
- Homebrew、Scoop、WinGetなどからのインストール
- 悪意ある別プロセスが生成中のディレクトリ内部を同時変更する競合攻撃
- サポート対象外OS・CPUでの動作
- CMake以外のビルドシステム

## 3. テスト方針

### 3.1 基本方針

- 下位モジュールの単体テストを先に実施する
- ファイル操作失敗は `FakeFileSystem` で決定的に再現する
- `RealFileSystem` は専用一時ディレクトリ内で検証する
- CLIの出力先、文面、改行、終了コードを同時に検証する
- 生成物は文字列比較だけでなく、実際のCMakeビルドで検証する
- Linux、macOS、WindowsのCIを必須とする
- 正常系、境界値、異常系、ロールバック、安全性を含める
- テストは順序に依存せず、繰り返し実行可能にする

### 3.2 テストピラミッド

```text
                 リリース検証
              クロスプラットフォームE2E
                 CLI統合テスト
              コンポーネントテスト
                   単体テスト
```

単体テストを最多とし、E2Eでは利用者の代表経路と環境差に集中する。

### 3.3 自動化方針

| テストレベル | 自動化 | 実行契機 |
|---|---:|---|
| 静的検証 | 必須 | 全PR、main |
| 単体テスト | 必須 | 全PR、main |
| コンポーネントテスト | 必須 | 全PR、main |
| CLI統合テスト | 必須 | 全PR、main |
| 生成物E2E | 必須 | 全PR、main |
| クロスプラットフォーム | 必須 | 全PR、main |
| 性能測定 | 自動記録＋リリース前判定 | main、リリース候補 |
| リリースアーカイブ検証 | 必須 | タグ、リリース候補 |

## 4. テストレベル

### 4.1 静的検証

| ID | 検証内容 | 合格基準 |
|---|---|---|
| ST-BUILD-001 | cppseed本体をC++20として構成・ビルド | GCC、Clang、MSVCで警告をエラーとして成功 |
| ST-DEPS-001 | include、link、配布バイナリの依存関係 | 製品コードは標準ライブラリ以外のサードパーティ依存を持たない |
| ST-DOC-001 | `git diff --check` とMarkdown相対リンク | 空白エラーとリンク切れがない |
| ST-TPL-001 | 埋め込みテンプレートのトークン | 全トークンが許可一覧内にある |
| ST-TPL-002 | 埋め込みテンプレートの内容 | 日時、端末名、利用者名、ホームディレクトリを含まない |
| ST-SAFE-001 | 製品コードのプロセス起動API | shellや外部コマンドを起動する実装がない |
| ST-TEST-001 | 製品・テストのターゲット依存 | 製品コードへテスト専用コードがリンクされていない |
| ST-LIC-001 | ルートLICENSEと生成README | MIT Licenseを含み、生成コードの利用をcppseedのライセンスで制限しない旨が明確 |

### 4.2 単体テスト

外部I/Oなしで、純粋ロジックまたは注入済み依存を検証する。

対象:

- `ProjectName`
- `parse_cli`
- `TemplateRenderer`
- 診断文字列
- C++規格変換
- 成功メッセージ

### 4.3 コンポーネントテスト

複数モジュールを結合して、プロセスを起動せず検証する。

対象:

- `ProjectGenerator`＋`TemplateRenderer`＋`FakeFileSystem`
- `Application`＋各モジュール＋メモリ上の入出力stream
- `RealFileSystem`＋一時ディレクトリ

### 4.4 CLI統合テスト

ビルド済みcppseed実行ファイルを別プロセスとして起動し、引数、標準出力、標準エラー出力、終了コード、生成物を検証する。

### 4.5 生成物E2Eテスト

cppseedでプロジェクトを生成した後、利用者と同じコマンドを実行する。

```console
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

さらに生成された実行ファイルを起動し、`Hello from <PROJECT_NAME>!` と終了コード0を確認する。

### 4.6 リリース検証

- アーカイブ名
- アーカイブ内容
- SHA-256チェックサム
- 展開後の起動
- `--help`、`--version`
- 一時ディレクトリでのプロジェクト生成

を対象OSごとに確認する。

## 5. テスト環境

### 5.1 CI環境マトリクス

| ID | OS | CPU | コンパイラ系列 | 用途 |
|---|---|---|---|---|
| ENV-LINUX-GCC | Linux | x86-64 | GCC | 主テスト、カバレッジ、E2E |
| ENV-LINUX-CLANG | Linux | x86-64 | Clang | コンパイラ差検証 |
| ENV-MAC-ARM | macOS | Apple Silicon | Apple Clang | macOS ARM検証 |
| ENV-MAC-X64 | macOS | Intel x86-64 | Apple Clang | macOS Intel検証 |
| ENV-WIN-MSVC | Windows | x86-64 | MSVC | Windows検証 |

GitHub Actionsの具体的なrunner labelとコンパイラバージョンは実装時に固定し、READMEへ記載する。

### 5.2 C++規格マトリクス

生成物E2Eでは以下を検証する。

| cppseedオプション | 期待するcompile feature |
|---|---|
| `--std 17` | `cxx_std_17` |
| オプションなし | `cxx_std_20` |
| `--std 20` | `cxx_std_20` |
| `--std 23` | `cxx_std_23` |

各CI環境で利用するコンパイラは、対応する規格を実際に構成・ビルドできるバージョンを選ぶ。

### 5.3 ロケール・パス

最低限、次の条件を含める。

- 一般的なASCIIパス
- 親ディレクトリ名に空白を含むパス
- 親ディレクトリ名に日本語を含むUTF-8パス
- Windowsの長い一時ディレクトリパス

`PROJECT_NAME` 自体は仕様どおりASCIIに限定する。

## 6. テストデータ

### 6.1 有効なプロジェクト名

| データ | 目的 | 正規化結果 |
|---|---|---|
| `a` | 最小長 | `a` |
| `A` | 大文字 | `a` |
| `fem-solver` | ハイフン | `fem_solver` |
| `My_App` | 大文字とアンダースコア | `my_app` |
| `mesh--tool` | 連続ハイフン | `mesh_tool` |
| `My__App` | 連続アンダースコア | `my_app` |
| `mix-_--__name` | 混在した連続区切り | `mix_name` |
| `a`＋英数字63文字 | 最大長 | 入力に応じた値 |

### 6.2 無効なプロジェクト名

| データ | 理由 |
|---|---|
| 空文字 | 最小長未満 |
| 65文字 | 最大長超過 |
| `1project` | 先頭が数字 |
| `_project` | 先頭がアンダースコア |
| `-project` | 先頭がハイフン |
| `my project` | 空白 |
| `foo/bar` | パス区切り |
| `foo\\bar` | Windowsパス区切り |
| `.`、`..` | ピリオド |
| `プロジェクト` | 非ASCII |
| 改行、タブ、ESCを含む値 | 制御文字 |

### 6.3 C++規格値

- 有効: `17`、`20`、`23`
- 無効: 空、`14`、`26`、`020`、`20.0`、`cpp20`、`C++20`

## 7. テストケース表記

テストケースIDは以下の形式とする。

```text
<LEVEL>-<MODULE>-<NUMBER>
```

例:

```text
UT-PN-001
CT-PG-004
IT-CLI-003
E2E-GEN-002
```

| Prefix | 意味 |
|---|---|
| `ST` | 静的検証 |
| `UT` | 単体テスト |
| `CT` | コンポーネントテスト |
| `IT` | CLI統合テスト |
| `E2E` | 生成物E2E |
| `PERF` | 性能テスト |
| `REL` | リリース検証 |

## 8. 単体テストケース

### 8.1 `ProjectName`

| ID | 条件 | 期待結果 |
|---|---|---|
| UT-PN-001 | 最小長 `a` | 受理、original=`a`、normalized=`a` |
| UT-PN-002 | 64文字 | 受理 |
| UT-PN-003 | 空文字 | runtime error |
| UT-PN-004 | 65文字 | runtime error |
| UT-PN-005 | 先頭が英字以外 | runtime error |
| UT-PN-006 | 2文字目以降に許可記号 | 受理 |
| UT-PN-007 | 空白・ピリオド・区切り文字 | runtime error |
| UT-PN-008 | 非ASCII | runtime error |
| UT-PN-009 | ASCII大文字 | 小文字化 |
| UT-PN-010 | ハイフンとアンダースコアの連続 | 1個の `_` へ集約 |
| UT-PN-011 | 検証失敗 | `ProjectName` が生成されない |
| UT-PN-012 | 有効な全ASCII文字クラス | originalを保持 |

### 8.2 CLI解析

| ID | 入力 | 期待結果 |
|---|---|---|
| UT-CLI-001 | 引数なし | usage error 2 |
| UT-CLI-002 | `--help`、`-h` | top-level help |
| UT-CLI-003 | `--version`、`-V` | version |
| UT-CLI-004 | help/version＋余分な引数 | unexpected argument |
| UT-CLI-005 | 未知コマンド | unknown command |
| UT-CLI-006 | top-level未知オプション | unknown option |
| UT-CLI-007 | `new --help`、`new -h` | new help |
| UT-CLI-008 | `new` のみ | project name required |
| UT-CLI-009 | `new demo` | `NewCommand`、C++20 |
| UT-CLI-010 | `new demo --std 17/20/23` | 対応するenum |
| UT-CLI-011 | `new --std 23 demo` | オプション前置を受理 |
| UT-CLI-012 | `--std` の値なし | usage error 2 |
| UT-CLI-013 | 無効な規格 | usage error 2＋hint |
| UT-CLI-014 | `--std` 重複 | usage error 2 |
| UT-CLI-015 | 位置引数重複 | unexpected argument |
| UT-CLI-016 | `--std=20` | unknown option |
| UT-CLI-017 | 未知のnewオプション | unknown option |
| UT-CLI-018 | 不正なプロジェクト名 | runtime error 1 |
| UT-CLI-019 | `new --help`＋余分な引数 | usage error 2 |
| UT-CLI-020 | オプションの大文字違い | unknown option |

### 8.3 C++規格変換

| ID | 入力 | 期待結果 |
|---|---|---|
| UT-STD-001 | cpp17 | `17`、`cxx_std_17` |
| UT-STD-002 | cpp20 | `20`、`cxx_std_20` |
| UT-STD-003 | cpp23 | `23`、`cxx_std_23` |

### 8.4 テンプレート描画

| ID | 条件 | 期待結果 |
|---|---|---|
| UT-TR-001 | 4種の既知トークン | 全て正しく置換 |
| UT-TR-002 | 同じトークンが複数回 | 全て置換 |
| UT-TR-003 | トークンなし | 元内容を維持 |
| UT-TR-004 | 未知トークン | 内部テンプレートエラー |
| UT-TR-005 | 閉じ `}}` がない | 内部テンプレートエラー |
| UT-TR-006 | 空トークン `{{}}` | 内部テンプレートエラー |
| UT-TR-007 | 値の中に `{{` | 再帰置換しない |
| UT-TR-008 | 本文末尾にLFなし | LFを1個追加 |
| UT-TR-009 | 本文末尾にLFあり | 追加しない |
| UT-TR-010 | パス描画 | LFを追加しない |
| UT-TR-011 | パス結果にNUL/CR/LF | エラー |
| UT-TR-012 | 未解決区切りが残る | エラー |

### 8.5 診断表示

| ID | 条件 | 期待結果 |
|---|---|---|
| UT-DIAG-001 | messageのみ | `error:` 1行 |
| UT-DIAG-002 | hintあり | error、hintの順 |
| UT-DIAG-003 | warningあり | error、warningの順 |
| UT-DIAG-004 | hintとwarning | error、hint、warningの順 |
| UT-DIAG-005 | 制御文字を含む入力 | `\\xNN` へ変換 |
| UT-DIAG-006 | 128バイト超 | 省略記号を付与 |
| UT-DIAG-007 | 通常の有効名 | バッククォート内で維持 |
| UT-DIAG-008 | 全行 | LFで終端 |

### 8.6 成功メッセージ

| ID | 条件 | 期待結果 |
|---|---|---|
| UT-MSG-001 | `fem-solver` | 外部設計の完全一致文字列 |
| UT-MSG-002 | 大文字を含むoriginal | originalを表示 |
| UT-MSG-003 | 任意OS | `/` を用いた仕様上の相対表現 |

## 9. コンポーネントテストケース

### 9.1 `ProjectGenerator`＋`FakeFileSystem`

| ID | 条件 | 期待結果 |
|---|---|---|
| CT-PG-001 | 標準生成 | 4ディレクトリ、9ファイル |
| CT-PG-002 | 各ファイル内容 | UTF-8（BOMなし）、LF改行、末尾LF、プレースホルダーなし |
| CT-PG-003 | 生成順序 | 内部設計の固定順序と一致 |
| CT-PG-004 | 生成先ファイルが存在 | 変更・削除なし |
| CT-PG-005 | 生成先ディレクトリが存在 | 変更・削除なし |
| CT-PG-006 | 生成先がダングリングsymlink | 変更・削除なし |
| CT-PG-007 | status失敗 | runtime error、生成なし |
| CT-PG-008 | ルート作成失敗 | runtime error、削除なし |
| CT-PG-009 | 競合でcreated=false | 他者ルートを削除しない |
| CT-PG-010 | 各サブディレクトリ作成で失敗 | ルート全体をロールバック |
| CT-PG-011 | 各ファイル書き込みで失敗 | ルート全体をロールバック |
| CT-PG-012 | ロールバックも失敗 | warningと残存パスを通知 |
| CT-PG-013 | テンプレート不正 | 副作用開始前に失敗 |
| CT-PG-014 | 相対パスに `..` | 副作用開始前に失敗 |
| CT-PG-015 | 絶対テンプレートパス | 副作用開始前に失敗 |
| CT-PG-016 | レンダリング後パス重複 | 副作用開始前に失敗 |
| CT-PG-017 | C++17/20/23 | 対応値を全ファイルへ反映 |
| CT-PG-018 | transaction commit後 | デストラクターで削除しない |

「各段階の失敗」はFakeFileSystemの操作番号を順番に変え、全作成操作を網羅するパラメーター化テストとする。

### 9.2 `Application`

| ID | 条件 | 期待結果 |
|---|---|---|
| CT-APP-001 | top-level help | stdoutのみ、終了0 |
| CT-APP-002 | new help | stdoutのみ、終了0 |
| CT-APP-003 | version | stdoutのみ、終了0 |
| CT-APP-004 | usage error | stderrのみ、終了2 |
| CT-APP-005 | runtime error | stderrのみ、終了1 |
| CT-APP-006 | 生成成功 | stdoutに成功文、終了0 |
| CT-APP-007 | 生成失敗 | 成功文なし |
| CT-APP-008 | hintあり | stderrの2行目にhint |
| CT-APP-009 | cleanup warning | stderrの最終行にwarning |

### 9.3 `RealFileSystem`

| ID | 条件 | 期待結果 |
|---|---|---|
| CT-FS-001 | 存在しないパスstatus | not_found、エラーなし |
| CT-FS-002 | ファイル・ディレクトリstatus | 種別を識別 |
| CT-FS-003 | ダングリングsymlink | リンクの存在を識別 |
| CT-FS-004 | ディレクトリ新規作成 | created=true |
| CT-FS-005 | 同名ディレクトリ再作成 | created=false |
| CT-FS-006 | UTF-8本文書き込み | バイト完全一致 |
| CT-FS-007 | 既存ファイルへの書き込み | 拒否、内容不変 |
| CT-FS-008 | 書き込み不可 | error code |
| CT-FS-009 | remove_all | 一時ルートだけ削除 |

権限テストは管理者権限やroot権限では実行せず、OS差で安定しない場合はFakeFileSystem試験を必須判定とする。

## 10. CLI統合テストケース

| ID | コマンド | 期待結果 |
|---|---|---|
| IT-CLI-001 | `cppseed --help` | 外部設計のhelp、stdout、終了0 |
| IT-CLI-002 | `cppseed -h` | IT-CLI-001と同等 |
| IT-CLI-003 | `cppseed --version` | `cppseed 0.1.0`、終了0 |
| IT-CLI-004 | `cppseed` | command required、stderr、終了2 |
| IT-CLI-005 | `cppseed unknown` | unknown command、終了2 |
| IT-CLI-006 | `cppseed new` | project name required、終了2 |
| IT-CLI-007 | `cppseed new bad/name` | invalid project name、終了1 |
| IT-CLI-008 | `cppseed new demo` | 生成成功、終了0 |
| IT-CLI-009 | 同じコマンドを再実行 | path exists、終了1、内容不変 |
| IT-CLI-010 | `--std 17/20/23` | 各規格を反映 |
| IT-CLI-011 | `--std 14` | unsupported＋hint、終了2 |
| IT-CLI-012 | 空白を含む親パス | 正常生成 |
| IT-CLI-013 | UTF-8を含む親パス | 正常生成 |
| IT-CLI-014 | エラー出力 | ANSIカラーなし |
| IT-CLI-015 | 生成成功出力 | 外部設計と完全一致 |

stdoutとstderrは別々に捕捉し、混在していないことを確認する。

## 11. 生成物E2Eテストケース

### 11.1 共通フロー

各C++規格について以下を実行する。

1. 空の一時ディレクトリを作成
2. `cppseed new e2e-project --std <value>` を実行
3. 期待する9ファイルが存在することを確認
4. `cmake -S . -B build` を実行
5. `cmake --build build --config Release` を実行
6. 生成された実行ファイルを起動
7. stdoutと終了コードを確認
8. `ctest --test-dir build -C Release --output-on-failure` を実行
9. 一時ディレクトリを削除

single-config generatorでは `-C Release` が無視または不要となるため、CI実装でgeneratorに応じて調整する。

### 11.2 ケース

| ID | 規格・条件 | 期待結果 |
|---|---|---|
| E2E-GEN-001 | C++17 | 構成、ビルド、実行、CTest成功 |
| E2E-GEN-002 | 既定C++20 | 構成、ビルド、実行、CTest成功 |
| E2E-GEN-003 | 明示C++20 | 構成、ビルド、実行、CTest成功 |
| E2E-GEN-004 | C++23 | 構成、ビルド、実行、CTest成功 |
| E2E-GEN-005 | `fem-solver` | ターゲット名と名前空間が仕様一致 |
| E2E-GEN-006 | `My__App` | 正規化結果 `my_app` で成功 |
| E2E-GEN-007 | 生成READMEの手順 | 記載手順だけで成功 |
| E2E-GEN-008 | 生成 `.gitignore` | build成果物を無視 |
| E2E-GEN-009 | 生成 `.clang-format` | clang-formatが設定を読める |
| E2E-GEN-010 | テスト一覧 | `<normalized_name>.hello` が1件以上 |

E2E-GEN-001から004は全OSの必須ケースとする。その他はLinux主環境で必須とし、少なくとも1件の代表生成を全OSで実施する。

E2E-GEN-008は生成物内で `git check-ignore build/example.o` を実行して確認する。E2E-GEN-009はclang-formatを導入したLinux主環境で `clang-format --dry-run` を実行し、設定ファイルの構文と適用可否を確認する。

## 12. 安全性・ロールバックテスト

| ID | 条件 | 合格基準 |
|---|---|---|
| SAFE-001 | 既存ファイルと同名 | バイト単位で不変 |
| SAFE-002 | 既存の空ディレクトリ | ディレクトリエントリ不変 |
| SAFE-003 | 既存の非空ディレクトリ | 子要素を含めて不変 |
| SAFE-004 | 既存symlink | リンク先・リンク自体とも不変 |
| SAFE-005 | ルート作成競合 | cppseedが作成していないパスを削除しない |
| SAFE-006 | 各生成操作で失敗 | cppseed作成ルートを除去 |
| SAFE-007 | rollback失敗 | 終了1、warning、成功文なし |
| SAFE-008 | `../` を含む入力 | カレントディレクトリ外に生成なし |
| SAFE-009 | 制御文字を含む入力 | 端末制御文字をそのまま出力しない |
| SAFE-010 | 代表入力を生成 | shellや外部コマンドを起動せず完了 |
| SAFE-011 | 生成物を走査 | 日時、端末名、利用者名、ホームパスを含まない |

破壊系試験は、テストが作成した一時ルートの絶対パスを確認してから実行する。ホームディレクトリ、リポジトリルート、システム一時ディレクトリそのものを削除対象にしてはならない。

## 13. 性能テスト

### 13.1 対象

外部プロセスとしての `cppseed new perf-project-N` の生成時間を測定する。CMakeビルド時間は含めない。

### 13.2 手順

1. ローカルファイルシステム上の一時ディレクトリを使用
2. 3回ウォームアップ
3. 異なる名前で30回生成
4. 各回のwall-clock時間を記録
5. median、p95、最大値を算出
6. 生成物を測定後に削除

### 13.3 判定

- 参照環境でp95が1秒以内: 合格
- GitHub hosted runnerでは5秒timeoutを設定し、ハング検出を必須にする
- hosted runnerの一時的負荷だけによる1秒超過は記録し、安定した参照環境で再判定する

メモリ使用量はv0.1.0のリリースゲートとしないが、テンプレート展開後の合計が内部設計の100 KiB想定を大きく超えないことをレビューする。

## 14. カバレッジ

### 14.1 対象

Linux GCC環境で `cppseed_core` を測定する。

### 14.2 目標

| 指標 | 目標 |
|---|---:|
| 行カバレッジ | 90%以上 |
| 分岐カバレッジ | 80%以上 |

`main.cpp`、テストハーネス、埋め込みテンプレートのraw文字列は集計対象外とする。

数値だけを目的に無意味なテストを追加せず、要件・境界値・失敗経路の網羅を優先する。未達の場合は未検証箇所を明示してリリース判断を行う。

## 15. リリース検証ケース

| ID | 条件 | 期待結果 |
|---|---|---|
| REL-001 | アーカイブ名 | 外部設計の命名規則と一致 |
| REL-002 | アーカイブ内容 | 実行ファイル、LICENSE、READMEのみ |
| REL-003 | SHA256SUMS | 全アーカイブと一致 |
| REL-004 | 対象OSで展開 | エラーなし |
| REL-005 | `--version` | タグのバージョンと一致 |
| REL-006 | `--help` | 終了0 |
| REL-007 | 代表プロジェクト生成 | 終了0、全ファイル存在 |
| REL-008 | 追加ランタイムなしの環境 | cppseedが起動 |
| REL-009 | Windows | `cppseed.exe` が起動 |
| REL-010 | macOS/Linux | 実行権限が付与されている |
| REL-011 | ルートREADMEの手順 | 新規環境で取得、起動、生成、ビルド、テストまで再現可能 |

## 16. CIジョブ設計

| Job | 内容 | 必須 |
|---|---|---:|
| `linux-gcc` | build、unit、component、CLI、全規格E2E | はい |
| `linux-clang` | build、unit、代表E2E | はい |
| `macos-arm64` | build、unit、全規格E2E | はい |
| `macos-x64` | build、unit、代表E2E | はい |
| `windows-msvc` | build、unit、全規格E2E | はい |
| `coverage` | GCCカバレッジ集計 | はい |
| `docs` | Markdownリンク・形式 | はい |
| `package` | OS別アーカイブとチェックサム | タグ時 |
| `release-smoke` | アーカイブ展開後の起動・生成 | タグ時 |

利用可能なrunner事情でmacOSアーキテクチャを直接実行できない場合、該当アーカイブの作成と実機smoke testをリリース条件として補完する。

## 17. テスト実行コマンド

cppseed自身:

```console
cmake -S . -B build -DCPPSEED_BUILD_TESTS=ON
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

警告をエラー化:

```console
cmake -S . -B build-werror \
  -DCPPSEED_BUILD_TESTS=ON \
  -DCPPSEED_WARNINGS_AS_ERRORS=ON
cmake --build build-werror
ctest --test-dir build-werror --output-on-failure
```

具体的なカバレッジコマンドとパッケージコマンドは実装時のworkflowおよびREADMEへ記載する。

## 18. テスト結果と証跡

各CI実行で以下を保存またはGitHub上から参照可能にする。

- commit SHA
- OS、CPU、コンパイラとバージョン
- CMakeバージョン
- CTestの結果
- 失敗テストの標準出力・標準エラー出力
- カバレッジ概要
- 性能測定値
- タグ時のアーカイブとSHA256SUMS

秘密情報、アクセストークン、ホームディレクトリ以下の無関係な絶対パスはログへ出さない。

## 19. 不具合重要度

| 重要度 | 定義 | 例 | リリース判断 |
|---|---|---|---|
| Critical | データ破壊、任意パス変更、重大な安全性問題 | 既存ディレクトリ削除 | 必ず修正 |
| High | 中核機能が利用不能 | 生成不可、生成物がビルド不能 | 必ず修正 |
| Medium | 回避可能な仕様不一致 | 一部メッセージ、特定入力の誤動作 | 原則修正 |
| Low | 軽微な表示・文書問題 | 空白、表記揺れ | Issue化を許容 |

## 20. 開始条件

テストコード実装を開始する条件:

- 要件定義書がApproved
- 外部設計書がApproved
- 内部設計書がApproved
- 本テスト計画がレビュー済み
- テスト対象モジュールのインターフェースが内部設計と一致

テスト実行を開始する条件:

- cppseedが構成・ビルドできる
- テストハーネスが動作する
- テスト用一時ディレクトリを安全に作成できる

## 21. 完了条件

v0.1.0のテスト工程は以下をすべて満たしたとき完了とする。

- 全必須CIジョブが成功
- 全単体、コンポーネント、CLI統合テストが成功
- C++17、C++20、C++23の生成物E2Eが成功
- macOS、Linux、Windowsの代表E2Eが成功
- Critical、Highの未解決不具合が0件
- Mediumの未解決不具合にリリース判断とIssueがある
- 既存パス保護と全失敗段階のロールバック試験が成功
- カバレッジ目標を達成、または未達理由が承認済み
- 性能目標を参照環境で達成
- リリースアーカイブsmoke testが全対象環境で成功
- 要件トレーサビリティに未検証項目がない

## 22. 要件トレーサビリティ

| 要件 | 主なテスト |
|---|---|
| FR-001、FR-002 | CT-PG-001〜003、IT-CLI-008、E2E-GEN-001〜010 |
| FR-003、FR-004 | UT-PN-001〜012、E2E-GEN-005〜006 |
| FR-005 | UT-CLI-010〜016、UT-STD-001〜003、E2E-GEN-001〜004 |
| FR-006、FR-007、FR-008 | E2E-GEN-001〜010 |
| FR-009 | CT-PG-004〜006、IT-CLI-009、SAFE-001〜004 |
| FR-010 | CT-PG-008〜012、SAFE-005〜007 |
| FR-011 | UT-MSG-001〜003、CT-APP-006、IT-CLI-015 |
| FR-012 | UT-DIAG-001〜008、CT-APP-004〜009、IT-CLI-004〜007、011、014 |
| FR-013 | UT-TR-001〜012、ST-TPL-001、REL-008 |
| NFR-001、NFR-002 | ST-BUILD-001、ST-DEPS-001、REL-008 |
| NFR-003、NFR-004 | 5章・16章の環境マトリクス |
| NFR-005 | PERFテスト |
| NFR-006 | ST-SAFE-001、SAFE-001〜011、CT-PG-004〜016 |
| NFR-007 | ST-DEPS-001、全クロスプラットフォームジョブ |
| NFR-008 | REL-001〜010 |
| NFR-009 | ST-LIC-001、REL-002 |
| NFR-010 | ST-TEST-001、単体・コンポーネントテスト、カバレッジ |

## 23. テスト計画の受け入れ条件

- [x] 全機能要件と非機能要件にテストが割り当てられている
- [x] 単体、コンポーネント、CLI統合、E2Eの責務が分離されている
- [x] 正常系、境界値、異常系、安全性ケースが定義されている
- [x] 全ファイル生成段階の失敗注入が定義されている
- [x] C++17、C++20、C++23の実ビルドが定義されている
- [x] macOS、Linux、Windowsの環境マトリクスが定義されている
- [x] 性能とカバレッジの目標が定義されている
- [x] リリース成果物のsmoke testが定義されている
- [x] テスト開始条件と完了条件が定義されている
- [x] 破壊系テストの安全な実行範囲が定義されている
