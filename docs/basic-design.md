# cppseed 外部設計書

- 文書バージョン: 0.1
- 対象リリース: cppseed v0.1.0
- ステータス: Approved
- 承認根拠: PR #2のmainへのマージ
- 作成日: 2026-07-13
- 上位文書: [要件定義書](requirements.md)

## 1. 目的

本書は、`cppseed` を利用者から見たときの振る舞いを定義する。CLI、入出力、終了コード、プロジェクト名の規則、生成物、対応環境および配布形式を設計対象とする。

クラス、関数、データ構造、テンプレートの保持方法、ロールバックの実装方法などは詳細設計で定義する。

## 2. システム境界

`cppseed` はローカルで完結する非対話型CLIである。

```text
利用者
  │ コマンド、引数、オプション
  ▼
cppseed
  │ ディレクトリとファイルを生成
  ▼
ローカルファイルシステム
  │
  ├── CMakeによる構成・ビルド
  ├── 生成された実行ファイルの実行
  └── CTestによるテスト
```

`cppseed` は以下を行わない。

- ネットワーク通信
- 外部コマンドの実行
- パッケージのダウンロード
- Gitリポジトリの初期化
- 既存プロジェクトの変更

## 3. 利用条件

### 3.1 cppseedの実行

配布済みバイナリを実行するために、C++コンパイラ、CMake、Python、Rust、Node.jsなどを必要としない。

### 3.2 生成物の利用

生成されたプロジェクトの構成、ビルド、テストには以下が必要である。

- CMake 3.20以降
- 選択したC++規格に対応するC++コンパイラ
- CMakeが利用できるビルドツール

## 4. CLIインターフェース

### 4.1 コマンド体系

```text
cppseed new <PROJECT_NAME> [--std <17|20|23>]
cppseed --help
cppseed --version
cppseed new --help
```

短縮オプションを含む同等の形式は以下とする。

```text
cppseed -h
cppseed -V
cppseed new -h
```

v0.1.0で提供するサブコマンドは `new` のみとする。

### 4.2 コマンドライン解析規則

- 引数とオプションはUTF-8の文字列として扱う。
- `--std` は `PROJECT_NAME` の前後どちらでも指定できる。
- `--std=20` 形式は受理せず、`--std 20` 形式のみ受理する。
- 同じオプションを複数回指定した場合はエラーとする。
- `--` によるオプション解析終了はv0.1.0では提供しない。
- オプション名は大文字・小文字を区別する。
- 未知のサブコマンド、オプション、余分な位置引数はエラーとする。
- `--help` と `--version` は情報を表示して終了し、プロジェクトを生成しない。

### 4.3 `new` コマンド

#### 4.3.1 構文

```text
cppseed new <PROJECT_NAME> [--std <17|20|23>]
```

#### 4.3.2 引数・オプション

| 項目 | 必須 | 初期値 | 説明 |
|---|---:|---:|---|
| `PROJECT_NAME` | はい | なし | プロジェクト名と生成先ディレクトリ名 |
| `--std` | いいえ | `20` | 生成物が要求するC++規格 |
| `-h`, `--help` | いいえ | なし | `new` コマンドのヘルプを表示 |

### 4.4 グローバルオプション

| オプション | 動作 |
|---|---|
| `-h`, `--help` | トップレベルのヘルプを標準出力へ表示して正常終了 |
| `-V`, `--version` | バージョンを標準出力へ表示して正常終了 |

### 4.5 トップレベルヘルプ

表示内容は以下を基準とする。空白の細部は実装上の差を許容するが、記載する項目と語句は維持する。

```text
Create small, buildable C++ projects.

Usage:
  cppseed <COMMAND>
  cppseed [OPTIONS]

Commands:
  new <PROJECT_NAME>  Create a new C++ project

Options:
  -h, --help     Show help
  -V, --version  Show version
```

### 4.6 `new` コマンドのヘルプ

```text
Create a new C++ project.

Usage:
  cppseed new <PROJECT_NAME> [--std <17|20|23>]

Arguments:
  <PROJECT_NAME>  Project name and output directory

Options:
      --std <17|20|23>  C++ standard [default: 20]
  -h, --help            Show help
```

### 4.7 バージョン表示

```text
cppseed 0.1.0
```

バージョン番号は実行ファイルのリリースバージョンと一致しなければならない。

## 5. プロジェクト名

### 5.1 入力規則

`PROJECT_NAME` は以下をすべて満たす必要がある。

- 長さは1文字以上64文字以下
- 先頭はASCII英字 `A-Z` または `a-z`
- 2文字目以降はASCII英数字、ハイフン、アンダースコアのみ
- 空白、ピリオド、パス区切り文字を含まない

検証規則は以下に相当する。

```regex
^[A-Za-z][A-Za-z0-9_-]{0,63}$
```

### 5.2 正規化規則

用途に応じて次の2種類の名前を使用する。

1. `original_name`: 利用者が入力した名前をそのまま保持する
2. `normalized_name`: ASCII英字を小文字化し、ハイフンまたはアンダースコアが連続する区間を1個のアンダースコアへ変換する

この正規化により、C++で予約される二重アンダースコアを含む識別子を生成しない。

例:

| 入力 | `original_name` | `normalized_name` |
|---|---|---|
| `fem-solver` | `fem-solver` | `fem_solver` |
| `My_App` | `My_App` | `my_app` |
| `CPPSeed2` | `CPPSeed2` | `cppseed2` |
| `mesh--tool` | `mesh--tool` | `mesh_tool` |
| `My__App` | `My__App` | `my_app` |

### 5.3 用途別の名称

`fem-solver` を入力した場合は以下とする。

| 用途 | 値 |
|---|---|
| 生成先ディレクトリ | `fem-solver` |
| CMakeプロジェクト名 | `fem-solver` |
| 実行ファイルのCMakeターゲット | `fem-solver` |
| 実行ファイル名 | `fem-solver`（Windowsでは `fem-solver.exe`） |
| ライブラリターゲット | `fem_solver_lib` |
| テストターゲット | `fem_solver_test` |
| includeディレクトリ | `include/fem_solver/` |
| C++名前空間 | `fem_solver` |

## 6. 生成先と生成処理

### 6.1 生成先

生成先は、コマンドを実行した時点のカレントディレクトリ直下にある `PROJECT_NAME` と同名のパスとする。

```console
$ pwd
/home/user/work
$ cppseed new fem-solver
```

生成先:

```text
/home/user/work/fem-solver
```

v0.1.0では出力先を変更するオプションを提供しない。

### 6.2 事前検証

ファイルを作成する前に以下を検証する。

1. CLIの構文が正しい
2. `PROJECT_NAME` が有効
3. `--std` の値が有効
4. 生成先と同名のファイルまたはディレクトリが存在しない
5. カレントディレクトリへ書き込み可能

検証に失敗した場合、ファイルまたはディレクトリを作成しない。

### 6.3 既存パスの保護

生成先と同名のファイル、ディレクトリ、シンボリックリンクなどが存在する場合は処理を中止する。対象の種類や内容にかかわらず、既存パスを変更または削除しない。

### 6.4 途中失敗時の状態

生成開始後に失敗した場合は、その実行によって新規作成した生成先ディレクトリ以下を可能な限り取り除く。

- 実行前から存在したパスを削除しない。
- クリーンアップにも失敗した場合は、元のエラーに加えて生成先を手動確認する必要がある旨を表示する。
- 成功として報告するのは、すべての生成ファイルの書き込みが完了した場合だけとする。

## 7. 生成物

### 7.1 ディレクトリ構成

```text
<PROJECT_NAME>/
├── CMakeLists.txt
├── README.md
├── .clang-format
├── .gitignore
├── include/
│   └── <normalized_name>/
│       └── hello.hpp
├── src/
│   ├── hello.cpp
│   └── main.cpp
└── tests/
    ├── CMakeLists.txt
    └── hello_test.cpp
```

### 7.2 ファイル別の外部仕様

| ファイル | 役割 |
|---|---|
| `CMakeLists.txt` | プロジェクト、ライブラリ、実行ファイル、CTestを定義 |
| `README.md` | 生成されたプロジェクト名、ビルド、実行、テスト手順を記載 |
| `.clang-format` | Google C++ Styleを基礎とする整形設定 |
| `.gitignore` | `build/`、CMake生成物、主要なコンパイル生成物を除外 |
| `include/<normalized_name>/hello.hpp` | サンプルライブラリの公開ヘッダー |
| `src/hello.cpp` | サンプルライブラリの実装 |
| `src/main.cpp` | サンプルライブラリを利用するエントリーポイント |
| `tests/CMakeLists.txt` | テスト実行ファイルとCTest登録を定義 |
| `tests/hello_test.cpp` | 標準ライブラリだけを使うサンプルテスト |

### 7.3 生成ファイルの共通仕様

- テキストファイルの文字コードはUTF-8（BOMなし）とする。
- 改行コードはLFとする。Windows上でも生成内容はLFを使用する。
- すべてのテキストファイルは末尾を改行で終える。
- テンプレート置換用の未解決プレースホルダーを残さない。
- 生成物に日時、生成した端末名、利用者名など再現性を損なう値を含めない。

## 8. 生成されるCMakeインターフェース

### 8.1 基本設定

生成されるトップレベルの `CMakeLists.txt` は以下を満たす。

- `cmake_minimum_required(VERSION 3.20)`
- `project(<original_name> LANGUAGES CXX)`
- `CMAKE_CXX_EXTENSIONS` を無効化
- `include(CTest)` によってCTestを利用可能にする
- トップレベルプロジェクトとしてビルドした場合にテストを有効化

### 8.2 C++規格

`--std` とCMakeのcompile featureを以下のように対応させる。

| `--std` | compile feature |
|---:|---|
| `17` | `cxx_std_17` |
| `20` | `cxx_std_20` |
| `23` | `cxx_std_23` |

C++規格はターゲット単位で要求する。コンパイラが要求された規格に対応しない場合、CMake構成時のエラーとして利用者へ通知される。

### 8.3 ターゲット

`fem-solver` を入力した場合、以下を提供する。

| ターゲット | 種類 | 入力ソース |
|---|---|---|
| `fem_solver_lib` | 静的ライブラリ | `src/hello.cpp` |
| `fem-solver` | 実行ファイル | `src/main.cpp` |
| `fem_solver_test` | テスト実行ファイル | `tests/hello_test.cpp` |

`fem-solver` と `fem_solver_test` は `fem_solver_lib` をリンクする。

### 8.4 生成された実行ファイル

生成された実行ファイルは、起動時に以下を標準出力へ表示し、終了コード0で終了する。

```text
Hello from <original_name>!
```

`fem-solver` の場合:

```text
Hello from fem-solver!
```

標準エラー出力には何も表示しない。

### 8.5 CTest

CTestへ登録するテスト名は `<normalized_name>.hello` とする。

例:

```text
fem_solver.hello
```

テストは外部テストフレームワークに依存せず、成功時に終了コード0、失敗時に非0を返す。

### 8.6 利用コマンド

構成とビルド:

```console
cmake -S . -B build
cmake --build build
```

テスト:

```console
ctest --test-dir build --output-on-failure
```

実行ファイルの位置はCMakeジェネレーターによって異なるため、READMEでは代表例を記載し、特定の生成先だけを保証しない。

## 9. 標準出力・標準エラー出力

### 9.1 基本方針

- 正常な結果とヘルプは標準出力へ表示する。
- エラーとヒントは標準エラー出力へ表示する。
- 通常出力へANSIカラーコードを含めない。
- メッセージは英語とする。
- エラー時にスタックトレース、ソースファイル名、内部例外などを表示しない。
- 秘密情報や環境変数の内容を表示しない。

### 9.2 生成成功時

`cppseed new fem-solver` の成功時は以下を表示する。

```text
Created C++ project `fem-solver` in ./fem-solver

Next steps:
  cd fem-solver
  cmake -S . -B build
  cmake --build build
  ctest --test-dir build --output-on-failure
```

### 9.3 エラー形式

基本形式:

```text
error: <cause>
```

利用者が入力を修正できる場合:

```text
error: <cause>
hint: <recovery action>
```

複数のエラーを検出できる場合でも、v0.1.0では最初に検出した1件を表示して終了する。

### 9.4 エラー一覧

| 状況 | 終了コード | 標準エラー出力 |
|---|---:|---|
| サブコマンドなし | `2` | `error: a command is required` |
| 未知のサブコマンド | `2` | ``error: unknown command `<value>` `` |
| `PROJECT_NAME` なし | `2` | `error: project name is required` |
| 余分な位置引数 | `2` | ``error: unexpected argument `<value>` `` |
| 未知のオプション | `2` | ``error: unknown option `<value>` `` |
| `--std` の値なし | `2` | ``error: option `--std` requires a value`` |
| `--std` の値が不正 | `2` | ``error: unsupported C++ standard `<value>` ``、続けて `hint: choose 17, 20, or 23` |
| オプションの重複 | `2` | ``error: option `<option>` was provided more than once`` |
| プロジェクト名が不正 | `1` | ``error: invalid project name `<value>` ``、続けて許可形式のhint |
| 生成先が存在 | `1` | ``error: path `<PROJECT_NAME>` already exists`` |
| 書き込み不可 | `1` | ``error: cannot create project in `<path>` `` |
| ファイル生成失敗 | `1` | ``error: failed to create `<relative-path>` `` |
| クリーンアップ失敗 | `1` | 元のエラー、続けて ``warning: cleanup was incomplete; inspect `<path>` `` |

OS由来の詳細は、秘密情報を含まない範囲でエラー原因の後ろへ付加できる。

## 10. 終了コード

| コード | 区分 | 例 |
|---:|---|---|
| `0` | 正常終了 | 生成成功、ヘルプ、バージョン表示 |
| `1` | 入力値または実行時エラー | 名前検証、既存パス、ファイル操作 |
| `2` | CLI使用方法のエラー | 未知のコマンド、引数不足、不正なオプション |

上記以外の終了コードはv0.1.0の公開仕様として使用しない。

## 11. 配布設計

### 11.1 GitHub Releases

GitHub Releasesに以下のアーカイブを掲載する。

| 環境 | アーカイブ名 | 実行ファイル |
|---|---|---|
| macOS Apple Silicon | `cppseed-v0.1.0-aarch64-apple-darwin.tar.gz` | `cppseed` |
| macOS Intel | `cppseed-v0.1.0-x86_64-apple-darwin.tar.gz` | `cppseed` |
| Linux x86-64 | `cppseed-v0.1.0-x86_64-unknown-linux-gnu.tar.gz` | `cppseed` |
| Windows x86-64 | `cppseed-v0.1.0-x86_64-pc-windows-msvc.zip` | `cppseed.exe` |

各アーカイブには以下を含める。

- 対象環境向け実行ファイル1個
- `LICENSE`
- `README.md`

各アーカイブと併せてSHA-256チェックサム一覧 `SHA256SUMS` を公開する。

### 11.2 インストール

v0.1.0では、利用者がアーカイブを展開し、実行ファイルをPATHの通った任意の場所へ配置する方式を正式なインストール方法とする。

Homebrew、Scoop、WinGetなどのパッケージマネージャー配布は対象外とする。

## 12. 対応環境

### 12.1 cppseed本体

| OS | CPU | リリースバイナリ |
|---|---|---:|
| macOS | Apple Silicon | 提供 |
| macOS | Intel x86-64 | 提供 |
| Linux | x86-64 | 提供 |
| Windows | x86-64 | 提供 |

各バイナリは対象環境で起動、ヘルプ表示、プロジェクト生成を検証する。

### 12.2 生成されるプロジェクト

以下のコンパイラ系列でCI検証する。

- Apple ClangまたはClang
- GCC
- MSVC

具体的な最低コンパイラバージョンはCIで採用するバージョンを基準に、実装・リリース時にREADMEへ明記する。

## 13. セキュリティ・安全性

- 入力されたプロジェクト名をカレントディレクトリ外へのパスとして解釈しない。
- シンボリックリンクを追跡して既存内容を変更しない。
- シェルを起動せず、入力をシェルコマンドとして解釈しない。
- 既存パスを上書き、削除、名前変更しない。
- 生成物へ環境変数、ユーザー名、ホームディレクトリなどを埋め込まない。
- 通常動作で管理者権限を要求しない。

## 14. 要件トレーサビリティ

| 要件 | 外部設計 |
|---|---|
| FR-001、FR-002 | 6章、7章 |
| FR-003、FR-004 | 5章 |
| FR-005 | 4章、8.2節 |
| FR-006、FR-007、FR-008 | 8章 |
| FR-009、FR-010 | 6章 |
| FR-011、FR-012 | 9章 |
| FR-013 | 利用者からは追加ファイル不要として3.1節に反映。実現方法は詳細設計で定義 |
| NFR-001、NFR-002、NFR-007、NFR-010 | 外部仕様へ影響する制約を継承し、具体的な実現方法は詳細設計で定義 |
| NFR-003、NFR-004、NFR-008 | 11章、12章 |
| NFR-005 | 要件定義書の性能目標を継承し、テスト計画で測定方法を定義 |
| NFR-006 | 6章、13章 |
| NFR-009 | 7.2節、11.1節。詳細なライセンス文面はリリース準備で確定 |

## 15. 外部設計の受け入れ条件

- [x] CLIの全コマンド、引数、オプションが定義されている
- [x] 正常時と異常時の出力先、文面、終了コードが定義されている
- [x] プロジェクト名の入力規則と変換結果が一意に決まる
- [x] 生成先、生成ファイル、各ファイルの役割が定義されている
- [x] 生成されるCMakeターゲットと利用コマンドが定義されている
- [x] 既存パスと途中失敗時の外部動作が定義されている
- [x] 対応環境と配布成果物が定義されている
- [x] v0.1.0の対象外機能が要件定義書と矛盾しない
- [x] 要件から外部設計への対応を追跡できる
