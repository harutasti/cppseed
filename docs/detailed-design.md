# cppseed 内部設計書

- 文書バージョン: 0.1
- 対象リリース: cppseed v0.1.0
- ステータス: Approved
- 承認根拠: PR #3のmainへのマージ
- 作成日: 2026-07-13
- 上位文書: [要件定義書](requirements.md)、[外部設計書](basic-design.md)

## 1. 目的

本書は、外部設計で定義した利用者向け仕様をC++20で実現するための内部構造を定義する。

設計対象は以下とする。

- ソースコードとCMakeターゲットの構成
- モジュールと型の責務
- CLI解析とアプリケーション制御
- プロジェクト名の検証と正規化
- テンプレートの保持と置換
- ファイル生成とロールバック
- 診断情報と終了コードの変換
- OS差の隔離
- 単体テスト可能な境界

テストケースの網羅表、CIマトリクスおよびリリース手順の詳細は、後続のテスト設計・リリース設計で定義する。

## 2. 設計原則

### 2.1 小さい実装単位

v0.1.0では汎用フレームワークを作らず、`new` コマンドに必要な機能だけを実装する。将来機能のための未使用抽象化は追加しない。

### 2.2 依存方向

依存は制御層からドメイン・基盤層への一方向とする。下位モジュールからCLIや標準入出力へ依存しない。

```text
main
  └── application
        ├── cli
        ├── project_name
        ├── project_generator
        │     ├── template_catalog
        │     ├── template_renderer
        │     └── file_system
        └── diagnostic
```

### 2.3 エラーを値として扱う

利用者の入力、既存パス、通常のファイル操作失敗は例外ではなく `Diagnostic` 値として返す。予期しない標準ライブラリ例外だけをアプリケーション境界で捕捉する。

### 2.4 副作用の局所化

標準出力、標準エラー出力、カレントディレクトリおよびファイルシステムへのアクセスは `ApplicationContext` から注入する。ドメインロジックからグローバルな `std::cout`、`std::cerr`、`std::filesystem::current_path()` を直接呼ばない。

### 2.5 実行時依存なし

ツール本体はC++標準ライブラリだけで実装する。テンプレートはコンパイル時に実行ファイルへ含め、実行時に外部テンプレートを探索しない。

## 3. リポジトリ構成

実装完了時の構成は以下を基本とする。

```text
cppseed/
├── CMakeLists.txt
├── LICENSE
├── README.md
├── cmake/
│   └── CompilerWarnings.cmake
├── docs/
│   ├── requirements.md
│   ├── basic-design.md
│   ├── detailed-design.md
│   └── test-plan.md
├── include/
│   └── cppseed/
│       ├── application.hpp
│       ├── cli.hpp
│       ├── diagnostic.hpp
│       ├── file_system.hpp
│       ├── project_generator.hpp
│       ├── project_name.hpp
│       ├── template_catalog.hpp
│       ├── template_renderer.hpp
│       └── types.hpp
├── src/
│   ├── application.cpp
│   ├── cli.cpp
│   ├── diagnostic.cpp
│   ├── file_system.cpp
│   ├── main.cpp
│   ├── project_generator.cpp
│   ├── project_name.cpp
│   ├── template_catalog.cpp
│   └── template_renderer.cpp
└── tests/
    ├── CMakeLists.txt
    ├── fake_file_system.cpp
    ├── fake_file_system.hpp
    ├── test_application.cpp
    ├── test_cli.cpp
    ├── test_framework.hpp
    ├── test_main.cpp
    ├── test_project_generator.cpp
    ├── test_project_name.cpp
    └── test_template_renderer.cpp
```

`include/cppseed/` のヘッダーはテストと内部ターゲット間の共有に使用する。v0.1.0ではライブラリAPIとしてインストールせず、後方互換性を保証する公開APIとはみなさない。

## 4. CMakeターゲット

### 4.1 ターゲット構成

| ターゲット | 種類 | 責務 |
|---|---|---|
| `cppseed_core` | 静的ライブラリ | `main` を除く全機能 |
| `cppseed` | 実行ファイル | OS引数・標準入出力をアプリケーションへ接続 |
| `cppseed_unit_tests` | テスト実行ファイル | `cppseed_core` の単体・コンポーネントテスト |

### 4.2 コンパイル要件

- `cppseed_core` は `cxx_std_20` を要求する。
- `CXX_EXTENSIONS` は無効にする。
- `cppseed` とテストは `cppseed_core` をリンクする。
- コンパイラ警告はプロジェクト内部のターゲットだけへ適用する。
- 警告のエラー化は開発・CI用オプションとし、利用者がサブプロジェクトとして組み込んだ場合に強制しない。

### 4.3 CMakeオプション

| オプション | 初期値 | 説明 |
|---|---:|---|
| `CPPSEED_BUILD_TESTS` | トップレベル時は `ON` | テストをビルドする |
| `CPPSEED_WARNINGS_AS_ERRORS` | CIでは `ON`、通常は `OFF` | cppseed自身の警告をエラー化する |

## 5. 共通データ型

共通型は `namespace cppseed` に置く。

### 5.1 終了コード

```cpp
enum class ExitCode : int {
    success = 0,
    runtime_error = 1,
    usage_error = 2,
};
```

### 5.2 診断情報

```cpp
struct Diagnostic {
    ExitCode exit_code;
    std::string message;
    std::optional<std::string> hint;
    std::optional<std::string> warning;
};
```

不変条件:

- `exit_code` は `runtime_error` または `usage_error`
- `message` は `error: ` を含まない本文だけを保持
- `hint` は `hint: ` を含まない
- `warning` は `warning: ` を含まない
- 表示時にだけ接頭辞と末尾改行を付ける

### 5.3 C++規格

```cpp
enum class CppStandard : int {
    cpp17 = 17,
    cpp20 = 20,
    cpp23 = 23,
};

std::string_view to_number(CppStandard value) noexcept;
std::string_view to_cmake_feature(CppStandard value) noexcept;
```

### 5.4 コマンド

```cpp
struct ShowTopLevelHelp {};
struct ShowNewHelp {};
struct ShowVersion {};

struct NewCommand {
    ProjectName project_name;
    CppStandard cpp_standard = CppStandard::cpp20;
};

using Command = std::variant<
    ShowTopLevelHelp,
    ShowNewHelp,
    ShowVersion,
    NewCommand>;

using ParseResult = std::variant<Command, Diagnostic>;
```

## 6. `ProjectName`

### 6.1 責務

- 利用者入力の妥当性を検証する
- 元の名前と正規化済みの名前を保持する
- C++識別子として安全な名前だけを生成する

### 6.2 インターフェース

```cpp
class ProjectName final {
public:
    using ParseResult = std::variant<ProjectName, Diagnostic>;

    static ParseResult parse(std::string_view input);

    [[nodiscard]] const std::string& original() const noexcept;
    [[nodiscard]] const std::string& normalized() const noexcept;

private:
    ProjectName(std::string original, std::string normalized);

    std::string original_;
    std::string normalized_;
};
```

コンストラクターを非公開にし、検証済みインスタンスだけが存在できるようにする。

### 6.3 検証アルゴリズム

1. バイト長が1以上64以下であることを確認する
2. 先頭がASCII英字か確認する
3. 残りがASCII英数字、`-`、`_` のいずれかであることを確認する
4. 失敗時は終了コード1の `Diagnostic` を返す
5. 成功時は正規化を実行する

許可文字がASCIIだけなので、文字数とバイト数は一致する。

### 6.4 正規化アルゴリズム

入力を左から1回走査する。

- ASCII大文字は小文字へ変換する
- ASCII英数字はそのまま追加する
- `-` または `_` を読んだ場合、直前の出力が `_` でなければ `_` を追加する
- 入力の先頭は英字なので、正規化結果が `_` から始まることはない
- 許可された入力では正規化結果が空になることはない

計算量は時間 `O(n)`、追加領域 `O(n)` とする。

## 7. CLI解析

### 7.1 責務

`parse_cli` は文字列列を副作用なしで `Command` または `Diagnostic` へ変換する。

```cpp
ParseResult parse_cli(std::span<const std::string_view> arguments);
```

`arguments` は実行ファイル名を除いた引数列とする。

### 7.2 トップレベル解析

以下の優先順位で解析する。

1. 引数なしなら `a command is required` のusage error
2. 引数が `-h` または `--help` の1個だけなら `ShowTopLevelHelp`
3. 引数が `-V` または `--version` の1個だけなら `ShowVersion`
4. 先頭が `new` なら `new` コマンド解析へ移る
5. 先頭が `-` ならunknown option
6. それ以外ならunknown command

グローバルオプションに追加の引数がある場合は、最初の余分な値をunexpected argumentとして返す。

### 7.3 `new` コマンド解析

`new` の後ろを1回走査し、次の状態を保持する。

```cpp
struct NewParseState {
    std::optional<std::string_view> project_name;
    std::optional<CppStandard> cpp_standard;
};
```

解析規則:

1. 引数が `-h` または `--help` の1個だけなら `ShowNewHelp`
2. `--std` を読んだら次の値を要求する
3. `17`、`20`、`23` を `CppStandard` へ変換する
4. `--std` が2回現れたらusage error
5. `-` で始まる未知の値はunknown option
6. 最初の位置引数をプロジェクト名候補とする
7. 2個目の位置引数はunexpected argument
8. 走査後にプロジェクト名がなければusage error
9. `ProjectName::parse` を呼び、成功時に `NewCommand` を構築する
10. `--std` 未指定時はC++20を使用する

`--std` は位置引数の前後どちらでも受理する。

### 7.4 ヘルプとバージョン文字列

ヘルプとバージョンは `cli.cpp` 内の `constexpr std::string_view` として保持する。外部設計の文面を変更する場合は、スナップショットテストも同時に変更する。

## 8. アプリケーション制御

### 8.1 コンテキスト

```cpp
struct ApplicationContext {
    FileSystem& file_system;
    std::filesystem::path current_directory;
    std::ostream& standard_output;
    std::ostream& standard_error;
};
```

### 8.2 エントリーポイント

```cpp
int run(
    std::span<const std::string_view> arguments,
    ApplicationContext& context);
```

`run` の処理:

1. `parse_cli` を呼ぶ
2. `Diagnostic` なら標準エラー出力へ表示し、対応する終了コードを返す
3. `ShowTopLevelHelp`、`ShowNewHelp`、`ShowVersion` なら標準出力へ表示して0を返す
4. `NewCommand` なら `ProjectGenerator::generate` を呼ぶ
5. 生成失敗なら診断を表示して終了コード1を返す
6. 生成成功なら成功メッセージを標準出力へ表示して0を返す

### 8.3 `main`

`main.cpp` は以下だけを担当する。

1. OSから渡された引数をUTF-8の `std::string` 列へ変換する
2. `RealFileSystem` を生成する
3. カレントディレクトリを取得する
4. `ApplicationContext` を構築する
5. `run` を呼ぶ
6. 予期しない `std::exception` およびその他の例外を捕捉し、内部情報を含まないエラーを表示して1を返す

WindowsではUnicode引数を扱うため `wmain` を使用し、UTF-16からUTF-8への変換だけをOS依存の薄い関数へ隔離する。変換後の解析処理は全OSで共通とする。

## 9. 診断情報の表示

### 9.1 表示関数

```cpp
void print_diagnostic(
    const Diagnostic& diagnostic,
    std::ostream& output);
```

表示順序:

1. `error: <message>`
2. `hint: <hint>`（存在する場合）
3. `warning: <warning>`（存在する場合）

各行はLFで終える。

### 9.2 利用者入力の引用

```cpp
std::string quote_for_diagnostic(std::string_view value);
```

- 外側をバッククォートで囲む
- ASCII制御文字、DEL、改行、タブ、エスケープを `\xNN` 形式へ変換する
- 表示対象は最大128バイトとし、超えた部分は `...` で省略する
- 端末制御シーケンスをそのまま出力しない

有効なプロジェクト名はASCIIかつ64文字以下なので、通常は変換されない。

## 10. テンプレートモデル

### 10.1 `TemplateContext`

```cpp
struct TemplateContext {
    std::string_view project_name;
    std::string_view normalized_name;
    std::string_view cpp_standard;
    std::string_view cmake_feature;
};
```

### 10.2 `TemplateSpec`

```cpp
struct TemplateSpec {
    std::string_view relative_path_template;
    std::string_view content_template;
};

struct RenderedFile {
    std::filesystem::path relative_path;
    std::string content;
};

std::span<const TemplateSpec> project_templates() noexcept;
```

テンプレート一覧は `template_catalog.cpp` の静的な `constexpr` 配列として保持する。内容は衝突しにくい区切り文字を持つraw string literalでコンパイルする。

### 10.3 プレースホルダー

v0.1.0で許可するプレースホルダーは以下だけとする。

| プレースホルダー | 値 |
|---|---|
| `{{PROJECT_NAME}}` | 元のプロジェクト名 |
| `{{NORMALIZED_NAME}}` | 正規化済みの名前 |
| `{{CPP_STANDARD}}` | `17`、`20`、`23` |
| `{{CMAKE_FEATURE}}` | `cxx_std_17`、`cxx_std_20`、`cxx_std_23` |

パスのテンプレートでは `{{NORMALIZED_NAME}}` だけを使用する。

### 10.4 `TemplateRenderer`

```cpp
class TemplateRenderer final {
public:
    using RenderResult = std::variant<std::string, Diagnostic>;

    [[nodiscard]] RenderResult render_text(
        std::string_view source,
        const TemplateContext& context) const;

    [[nodiscard]] RenderResult render_path(
        std::string_view source,
        const TemplateContext& context) const;
};
```

両メソッドは同じ非公開の置換処理を利用する。置換アルゴリズム:

1. 入力を左から走査する
2. `{{` がなければ残りを出力へ追加する
3. `{{` から次の `}}` までをトークンとして読む
4. 許可済みプレースホルダーなら値を追加する
5. 未知、閉じ忘れ、空のプレースホルダーなら内部テンプレートエラーを返す
6. 出力に未解決の `{{` または `}}` が残っていないことを確認する
7. `render_text` は出力末尾がLFであることを確認し、不足時はLFを追加する
8. `render_path` は改行を追加せず、NUL、CR、LFを含む結果をエラーにする

正規表現と再帰置換は使用しない。テンプレート値の中に現れる文字列を再度プレースホルダーとして解釈しない。

## 11. ファイルシステム境界

### 11.1 抽象インターフェース

```cpp
struct PathStatus {
    std::filesystem::file_status value;
    std::error_code error;
};

struct RemoveResult {
    std::uintmax_t removed_count;
    std::error_code error;
};

struct CreateDirectoryResult {
    bool created;
    std::error_code error;
};

class FileSystem {
public:
    virtual ~FileSystem() = default;

    [[nodiscard]] virtual PathStatus status(
        const std::filesystem::path& path) const = 0;

    [[nodiscard]] virtual CreateDirectoryResult create_directory(
        const std::filesystem::path& path) = 0;

    [[nodiscard]] virtual std::error_code write_text_file(
        const std::filesystem::path& path,
        std::string_view content) = 0;

    [[nodiscard]] virtual RemoveResult remove_all(
        const std::filesystem::path& path) noexcept = 0;
};
```

通常のOSエラーは戻り値の `std::error_code` で返す。メモリ確保失敗などの予期しない標準ライブラリ例外はアプリケーション境界まで伝播させ、固定の内部エラーへ変換する。`remove_all` はデストラクターからも利用するため例外を送出しない。

### 11.2 `RealFileSystem`

`RealFileSystem` は `std::filesystem` と `std::ofstream` を使用して上記を実装する。

- パス存在確認には `symlink_status` 相当を使用し、ダングリングシンボリックリンクも既存パスとして扱う
- `create_directory` は、対象ディレクトリを作成した場合だけ `created = true` を返す
- テキストファイルはbinary modeで開き、LFを変換せず書き込む
- 書き込み前に対象パスが存在しないことを確認し、ファイルまたはシンボリックリンクが存在する場合は拒否する
- 書き込み後にstream状態を確認する
- 書き込み対象の親ディレクトリが存在しない場合はエラーを返す
- `remove_all` は標準ライブラリからの例外も内部で捕捉し、削除失敗の `std::error_code` へ変換する
- シェルやOSコマンドを呼び出さない

### 11.3 `FakeFileSystem`

テスト用実装は以下を提供する。

- パスと内容をメモリ上で保持
- ファイル、ディレクトリ、シンボリックリンクの状態を表現
- 指定した操作回数またはパスで失敗を注入
- 削除されたパスと操作順序を記録

これにより、権限エラー、途中書き込み失敗、ロールバック失敗を実ファイルシステムへ依存せず再現する。

## 12. 生成トランザクション

### 12.1 責務

`GenerationTransaction` は、生成先ルートをcppseedが作成したか、および生成が完了したかを追跡する。

この型は `project_generator.cpp` の非公開実装とし、外部ヘッダーへ公開しない。テストは `ProjectGenerator` の観測可能な動作を通して検証する。

```cpp
class GenerationTransaction final {
public:
    GenerationTransaction(
        FileSystem& file_system,
        std::filesystem::path root);

    GenerationTransaction(const GenerationTransaction&) = delete;
    GenerationTransaction& operator=(const GenerationTransaction&) = delete;

    void mark_root_created() noexcept;
    void commit() noexcept;

    [[nodiscard]] std::error_code rollback() noexcept;
    ~GenerationTransaction() noexcept;

private:
    FileSystem& file_system_;
    std::filesystem::path root_;
    bool root_created_ = false;
    bool committed_ = false;
    bool rollback_attempted_ = false;
};
```

### 12.2 不変条件

- `root_created_ == false` の場合は削除しない
- `committed_ == true` の場合は削除しない
- 明示的ロールバックは最大1回だけ実行する
- デストラクターは例外を送出しない
- デストラクターの削除は、明示的ロールバックが呼ばれなかった場合の最終防衛とする

### 12.3 エラー時の流れ

1. 生成処理が失敗理由の `Diagnostic` を作る
2. `rollback()` を明示的に呼ぶ
3. ロールバックも失敗した場合は戻り値の `std::error_code` を、例外送出可能な通常処理の中で `Diagnostic.warning` へ変換する
4. `Diagnostic` を呼び出し元へ返す

## 13. `ProjectGenerator`

### 13.1 入力

```cpp
struct ProjectSpec {
    ProjectName project_name;
    CppStandard cpp_standard;
};

using GenerationResult = std::optional<Diagnostic>;
```

`std::nullopt` は成功を表す。

### 13.2 インターフェース

```cpp
class ProjectGenerator final {
public:
    ProjectGenerator(
        FileSystem& file_system,
        const TemplateRenderer& renderer);

    [[nodiscard]] GenerationResult generate(
        const ProjectSpec& spec,
        const std::filesystem::path& current_directory) const;

private:
    FileSystem& file_system_;
    const TemplateRenderer& renderer_;
};
```

### 13.3 生成手順

1. `root = current_directory / project_name.original()` を計算する
2. `FileSystem::status(root)` で既存パスを確認する
3. status取得失敗なら実行時エラーを返す
4. 既存なら何も変更せず実行時エラーを返す
5. `TemplateContext` を構築する
6. 全テンプレートの相対パスを `render_path`、内容を `render_text` でメモリ上へレンダリングする
7. レンダリング済みパスが相対パスであり、`..`、ルート名、空要素を含まないことを検証する
8. レンダリング後のパスが重複しないことを検証する
9. `root` を単一ディレクトリとして作成する
10. `created == true` の場合だけtransactionへ `mark_root_created()` を通知する。`created == false` なら競合による既存パスとして扱い、削除しない
11. 必要なサブディレクトリを `create_directory` で固定順序に作成する。`created == false` のサブディレクトリがあれば予期しない競合として失敗させる
12. レンダリング済みファイルを固定順序で書き込む
13. 全処理成功後に `commit()` する
14. 成功を返す

テンプレートの検証とレンダリングを副作用より前に完了させ、内部テンプレート不良で部分生成物を残さない。

### 13.4 固定生成順序

ディレクトリ:

```text
include/
include/{{NORMALIZED_NAME}}/
src/
tests/
```

ファイル:

```text
CMakeLists.txt
README.md
.clang-format
.gitignore
include/{{NORMALIZED_NAME}}/hello.hpp
src/hello.cpp
src/main.cpp
tests/CMakeLists.txt
tests/hello_test.cpp
```

固定順序により、テストの失敗注入と再現性を安定させる。

## 14. パス安全性

### 14.1 利用者入力

生成先ルートに使用する `original_name` は `ProjectName` でASCII英数字、`-`、`_` に限定される。パス区切り、ピリオド、絶対パス表現を含められない。

### 14.2 テンプレートパス

レンダリング後の各相対パスに対して以下を確認する。

- `path.is_absolute()` がfalse
- `path.has_root_name()` と `path.has_root_directory()` がfalse
- 各componentが空、`.`、`..` ではない
- `root / relative_path` を辞書的正規化してもroot配下から外れない

### 14.3 競合への対応

存在確認とルート作成の間に別プロセスが同名パスを作成した場合、`CreateDirectoryResult.created == false` として扱う。cppseedが作成したことを確認できないルートには `mark_root_created()` を呼ばず、削除しない。

### 14.4 並行変更の境界

cppseedがルートを作成した後も、各書き込み前に対象が未作成であることを確認する。標準C++だけでは存在確認とファイル作成を全OSで原子的かつシンボリックリンク非追跡にする共通APIがないため、生成中に別プロセスが生成先内部を敵対的に変更する状況はv0.1.0の保証対象外とする。

通常利用における既存パス保護と、ルート作成時の競合による他者パスの削除防止は保証する。将来、強い並行攻撃耐性が必要になった場合はOS別の排他的ファイル作成APIをファイルシステム実装内へ追加する。

## 15. エラー変換

### 15.1 分類

| 発生箇所 | `ExitCode` | 変換担当 |
|---|---|---|
| CLI文法 | `usage_error` | `cli.cpp` |
| プロジェクト名 | `runtime_error` | `project_name.cpp` |
| 既存パス・OSエラー | `runtime_error` | `project_generator.cpp` |
| テンプレート不整合 | `runtime_error` | `template_renderer.cpp` |
| 予期しない例外 | `runtime_error` | `main.cpp` |

### 15.2 OSエラー

OS由来の `std::error_code` は、公開メッセージの後ろへ安全な説明を付加するために使用できる。ただし、内部の絶対パスを必須表示にせず、利用者へは可能な限り外部設計で定めた相対パスを表示する。

### 15.3 予期しない例外

トップレベルでは以下の固定メッセージを表示する。

```text
error: unexpected internal failure
```

例外の `what()`、型名、スタックトレースは表示しない。デバッグ情報は将来のログ機能として扱い、v0.1.0には含めない。

## 16. 成功メッセージ

```cpp
std::string make_success_message(const ProjectName& project_name);
```

成功メッセージはすべてのファイル生成とtransactionのcommit後に構築・表示する。パスは外部設計どおり `./<original_name>` として表示し、OS固有の絶対パスへ変換しない。

## 17. テスト可能性

### 17.1 単体テスト対象

| モジュール | 主な検証 |
|---|---|
| `project_name` | 境界長、許可文字、不正文字、正規化、連続区切り |
| `cli` | 全コマンド形式、オプション順、重複、欠落、未知引数 |
| `template_renderer` | 全置換、未知トークン、閉じ忘れ、末尾LF |
| `diagnostic` | 接頭辞、hint、warning、制御文字のエスケープ |
| `project_generator` | 生成順序、全ファイル、既存パス、各段階の失敗、ロールバック |
| `application` | 出力先、終了コード、成功・失敗メッセージ |

### 17.2 テストフレームワーク

外部依存を増やさないため、v0.1.0ではテスト用の最小ハーネスを `tests/test_framework.hpp` に実装する。

最低限の機能:

- テスト関数の登録
- 真偽値、等値、variant種別のアサーション
- 失敗位置とメッセージの表示
- テスト件数と失敗件数の集計
- 失敗時に非0を返す

テストハーネスは製品バイナリへリンクしない。

### 17.3 実ファイルシステムテスト

`RealFileSystem` は一時ディレクトリを使うコンポーネントテストで検証する。テストは自分で作成した一時ルートだけを削除し、固定の共有パスを使用しない。

## 18. OS差の扱い

| 差異 | 方針 |
|---|---|
| パス表現 | 内部は `std::filesystem::path`、表示は仕様上の相対表現 |
| Windows実行ファイル | CMakeが `.exe` を付与。cppseed本体で拡張子を組み立てない |
| Windows引数 | `wmain` からUTF-8へ変換 |
| 改行 | ファイルはbinary modeでLFを書き込む |
| ファイル権限 | OS固有APIで事前判定せず、標準ファイル操作のerror codeを利用 |
| シンボリックリンク | `symlink_status` 相当でリンク自体の存在を検出 |

OS固有コードは引数変換に限定する。ファイル生成ロジックは共通実装とする。

## 19. 性能・資源使用

- テンプレート数は9個の固定配列とする
- 全テンプレートを副作用前にメモリへ展開する
- 想定される展開後サイズは合計100 KiB未満とする
- 各テンプレートは1回の走査でレンダリングする
- ネットワーク、外部プロセス、スレッドを使用しない
- 生成処理は同期的に実行する

この規模では安全な事前検証と単純性を、ストリーミングによる省メモリ化より優先する。

## 20. 内部設計トレーサビリティ

| 上位仕様 | 内部設計 |
|---|---|
| 外部設計4章 CLI | 5章、7章、8章 |
| 外部設計5章 名前 | 6章 |
| 外部設計6章 生成処理 | 11章、12章、13章 |
| 外部設計7章 生成物 | 10章、13章 |
| 外部設計8章 CMake・実行 | 10章、13章およびテンプレート実装 |
| 外部設計9・10章 診断・終了コード | 5章、8章、9章、15章、16章 |
| 外部設計11・12章 配布・環境 | 4章、18章。パッケージ処理はリリース設計で定義 |
| 外部設計13章 安全性 | 11章、12章、14章、15章 |
| NFR-001、NFR-002 | 2章、4章 |
| NFR-005 | 19章。測定条件はテスト設計で定義 |
| NFR-007 | 11章、18章 |
| NFR-010 | 2章、3章、17章 |

## 21. 実装順序

依存の下位から以下の順で実装する。

1. 共通型と `Diagnostic`
2. `ProjectName`
3. CLI解析
4. `TemplateRenderer`
5. `TemplateCatalog`
6. `FileSystem` と `FakeFileSystem`
7. `GenerationTransaction`
8. `ProjectGenerator`
9. `Application`
10. `main`
11. CMake・CI・リリース設定

各段階で対応する単体テストを同時に追加する。

## 22. 内部設計の受け入れ条件

- [x] 全モジュールの責務と依存方向が定義されている
- [x] 主要な型と関数シグネチャが定義されている
- [x] CLI解析アルゴリズムが外部仕様を満たす
- [x] 名前検証と正規化アルゴリズムが定義されている
- [x] テンプレート一覧、プレースホルダー、置換規則が定義されている
- [x] 既存パスを保護する生成手順が定義されている
- [x] 途中失敗とロールバックの流れが定義されている
- [x] OSエラーと終了コードの変換が定義されている
- [x] Windowsを含むOS差の分離方針が定義されている
- [x] 単体テスト用の差し替え境界と失敗注入方法が定義されている
- [x] 上位仕様から内部設計への対応を追跡できる
