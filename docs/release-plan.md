# cppseed v0.1.0 リリース設計書

- 文書バージョン: 0.1
- 対象リリース: cppseed v0.1.0
- ステータス: Completed
- 完了根拠: PR #6およびPR #7のmainへのマージ、[v0.1.0 Release](https://github.com/harutasti/cppseed/releases/tag/v0.1.0)の公開
- 作成日: 2026-07-14
- 上位文書: [要件定義書](requirements.md)、[外部設計書](basic-design.md)、[内部設計書](detailed-design.md)、[テスト計画書](test-plan.md)

## 1. 目的

本書は、mainへ統合済みのcppseed v0.1.0を、利用者が取得して実行できるGitHub Releaseとして安全かつ再現可能な手順で公開するための設計を定義する。

設計対象は次のとおりとする。

- バージョンとタグ
- 対象OS・CPUと互換性境界
- リリース用ビルド
- アーカイブとチェックサム
- GitHub Actionsによる自動化
- リリース成果物のsmoke test
- privateリポジトリからOSSとして公開する前の確認
- 公開、失敗、訂正および撤回の手順
- リリース証跡と完了条件

## 2. リリースの位置付け

v0.1.0はcppseedの最初の公開リリースとする。

- 安定版1.0.0ではなく、初期利用とフィードバック収集を目的とする0.xリリースである
- 要件定義書で定めた `new` コマンドの範囲を提供する
- 0.xであっても、公開済みの同一バージョン成果物は差し替えない
- 破壊的変更が必要な場合は、以後の0.xリリースでCHANGELOGまたはRelease notesへ明記する

## 3. 対象と対象外

### 3.1 対象

- 4対象環境向け実行ファイルのビルド
- OS別アーカイブの作成
- SHA-256チェックサムの作成
- 対象環境上での展開・起動・生成smoke test
- GitHub上のdraft Release作成
- 利用者向けインストール・検証手順のREADME追記
- リポジトリのpublic化とRelease公開

### 3.2 対象外

- Homebrew、Scoop、WinGetなどのパッケージマネージャー配布
- Linux ARM64、Windows ARM64および32 bit環境
- macOS notarizationおよびApple Developer ID署名
- Windows Authenticode署名
- SBOM、SLSA provenanceおよび再現可能ビルドの保証
- 自動更新機能
- Docker image
- GitHub以外のミラー配布

コード署名を行わないため、macOS GatekeeperまたはWindows SmartScreenが未署名バイナリの警告を表示する可能性がある。この制約とSHA-256による確認方法をREADMEおよびRelease notesへ記載する。

## 4. リリース識別子

### 4.1 バージョン

バージョン番号はSemantic Versioning形式の `0.1.0` とする。

| 場所 | 値 |
|---|---|
| Git tag | `v0.1.0` |
| GitHub Release名 | `cppseed v0.1.0` |
| `cppseed --version` | `cppseed 0.1.0` |
| CMake project version | `0.1.0` |
| アーカイブ名のversion部分 | `v0.1.0` |

### 4.2 整合性検証

リリースworkflowはタグ名から先頭の `v` を除いた値を取得し、以下が同じ `0.1.0` であることを検証する。

1. `CMakeLists.txt` のproject version
2. Release buildした `cppseed --version`
3. アーカイブ名
4. GitHub Release名

不一致が1件でもあればRelease作成前に失敗させる。

### 4.3 タグ方針

- mainの検証済みcommitだけへannotated tagを作成する
- 公開済みタグを移動、削除または同名で再作成しない
- 公開後の修正は `v0.1.1` 以降の新しいversionで行う
- draft Release公開前に限り、誤って作成したタグを削除してやり直せる

## 5. 名称と公開前確認

プロジェクト名と実行ファイル名は `cppseed` を維持する。

2026-07-14時点の一般的なWeb検索および主要公開パッケージレジストリを対象とした予備確認では、同名の著名なC++プロジェクトは確認できなかった。ただし、これは網羅的な商標調査または法的判断ではない。

public化の直前に、リリース担当者が以下を再確認する。

- GitHubの同名・類似目的リポジトリ
- Homebrew、Scoop、WinGet、npm、PyPI、crates.ioの同名package
- 一般Web検索
- 必要に応じてJ-PlatPatなどの商標データベース

重大な衝突が見つかった場合は、タグ作成前に名称変更を判断する。軽微な同名利用だけの場合は、説明文と配布元 `harutasti/cppseed` を明確にして継続できる。

## 6. 配布対象

v0.1.0では外部設計どおり4環境すべてへビルド済み成果物を提供する。

| ID | OS | CPU | GitHub Actions runner | アーカイブ |
|---|---|---|---|---|
| REL-MAC-ARM | macOS | Apple Silicon arm64 | `macos-15` | `cppseed-v0.1.0-aarch64-apple-darwin.tar.gz` |
| REL-MAC-X64 | macOS | Intel x86-64 | `macos-15-intel` | `cppseed-v0.1.0-x86_64-apple-darwin.tar.gz` |
| REL-LINUX-X64 | Linux | x86-64 | `ubuntu-22.04` | `cppseed-v0.1.0-x86_64-unknown-linux-gnu.tar.gz` |
| REL-WIN-X64 | Windows | x86-64 | `windows-2025` | `cppseed-v0.1.0-x86_64-pc-windows-msvc.zip` |

runner labelは実装時に利用可能性を再確認し、固定値としてworkflowへ記載する。runner廃止時は同一CPU向けの後継runnerへ変更し、変更理由をPRに記録する。

## 7. 互換性方針

### 7.1 ソースからのビルド

cppseed本体の最低対応コンパイラを次のとおり定める。

| コンパイラ | 最低version |
|---|---:|
| GCC | 11 |
| Clang | 14 |
| Apple Clang | 14 |
| MSVC | 19.34（Visual Studio 2022 17.4相当） |

最低versionはcppseed本体が利用するC++20標準ライブラリ機能を基準とする。リリース実装では、可能な範囲で最低versionのcompatibility jobを追加する。最低versionを直接実行できない環境は、READMEへ「設計上の最低version」と「リリースCIで実際に検証したversion」を分けて記載する。

生成プロジェクトにはCMake 3.20以降と、選択したC++17、C++20またはC++23に対応するコンパイラが必要である。

### 7.2 Linux ABI

- Release buildは `ubuntu-22.04` のx86-64 runnerで行う
- glibcおよびlibstdc++へ動的リンクする
- 最低実行環境はUbuntu 22.04、またはglibc 2.35以上と必要なlibstdc++を備える互換ディストリビューションとする
- musl環境はv0.1.0の対象外とする
- `ldd` の結果をリリースログへ残し、リポジトリ内のファイルや非標準の共有ライブラリへ依存していないことを確認する

単一実行ファイルとは、cppseed固有の追加ファイルや言語ランタイムを要求しないことを意味する。OSが通常提供するC/C++ランタイムおよびシステムライブラリは許容する。

### 7.3 macOS

- Release buildでは `CMAKE_OSX_DEPLOYMENT_TARGET=13.0` を指定する
- arm64とx86-64を別々にbuildし、universal binaryにはしない
- `otool -L` で依存先を記録し、非システム共有ライブラリへ依存していないことを確認する
- macOS 13以降をv0.1.0の対象とする

### 7.4 Windows

- MSVC Release buildを使用する
- `CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded` によりMSVC runtimeを静的リンクする
- cppseed固有のDLLおよびVisual C++ Redistributableの別途導入を要求しない
- `dumpbin /dependents` の結果を記録し、Windows system DLL以外への依存がないことを確認する
- Windows 10およびWindows 11のx86-64を利用対象とする

## 8. ビルド設定

全環境で次の論理設定を使用する。

```console
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCPPSEED_BUILD_TESTS=ON \
  -DCPPSEED_WARNINGS_AS_ERRORS=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

multi-config generatorでは `CMAKE_BUILD_TYPE` を使用せず、`--config Release` を使用する。Windowsでは前章のMSVC runtime設定を追加し、macOSではdeployment targetを追加する。

Release buildにデバッグ用の秘密情報、ローカル絶対パス、アクセストークンまたはテスト専用コードを含めない。

## 9. アーカイブ設計

### 9.1 内容

各アーカイブの直下には以下の3項目だけを含める。

```text
cppseed            # Windowsではcppseed.exe
LICENSE
README.md
```

- 親ディレクトリはアーカイブへ含めない
- macOS/Linuxの実行ファイルには実行権限を付与する
- Windows以外はtar.gz、Windowsはzipを使用する
- READMEとLICENSEはタグcommitの内容を使用する
- シンボリックリンクを含めない

### 9.2 決定性

v0.1.0ではbit-for-bitの再現可能アーカイブを保証しない。ただし、入力commit SHA、runner image、コンパイラversion、CMake versionおよび生成されたSHA-256を証跡として残す。

## 10. SHA-256

全4アーカイブに対して `SHA256SUMS` を1個作成する。

形式:

```text
<64文字の小文字16進数>  <archive-file-name>
```

規則:

- ファイル名の昇順で並べる
- 区切りは空白2個とする
- 改行はLFとする
- `SHA256SUMS` 自体は一覧へ含めない
- Releaseへ添付する前に全行を再検証する

READMEにはmacOS/Linuxの `shasum -a 256` または `sha256sum`、Windows PowerShellの `Get-FileHash -Algorithm SHA256` による確認例を記載する。

## 11. Release workflow

### 11.1 ファイル

実装工程で `.github/workflows/release.yml` を追加する。

### 11.2 起動条件

workflowは次の2契機を持つ。

1. `pull_request`: リリース実装に関係する変更のpackaging dry run。GitHub Releaseは作成しない
2. `workflow_dispatch`: main上の指定refに対するpackaging dry run。GitHub Releaseは作成しない
3. `push` tags `v*.*.*`: draft GitHub Releaseを作成する

GitHub Actionsのtag filterはglobであるため、`validate` jobで `^v[0-9]+\.[0-9]+\.[0-9]+$` に完全一致することを別途検証する。dry runではmainの指定commitをbuildし、Actions artifactとして7日間保存する。正式実行ではタグがmainの履歴上にあることを確認する。

### 11.3 権限

- workflow全体の既定権限は `contents: read`
- draft Releaseを作成するjobだけ `contents: write`
- pull request、issue、packages、id-tokenへのwrite権限を与えない
- repository secretを使用しない

### 11.4 Job構成

```text
validate
   │
   ├── package-macos-arm64 ── smoke-macos-arm64
   ├── package-macos-x64   ── smoke-macos-x64
   ├── package-linux-x64   ── smoke-linux-x64
   └── package-windows-x64 ── smoke-windows-x64
                    │
                    ▼
             checksum-and-release
```

| Job | 責務 |
|---|---|
| `validate` | version整合、タグ位置、main CI成功を確認 |
| `package-*` | Release build、CTest、staging、圧縮、依存関係記録 |
| `smoke-*` | 展開後バイナリを同一対象OSで検証 |
| `checksum-and-release` | 4成果物を集約、SHA256SUMS作成、draft Release作成 |

packageとsmokeは同じmatrix job内で連続実行してよい。1環境でも失敗した場合は集約jobを実行しない。

### 11.5 Actions依存

- GitHub公式Actionだけを使用する
- Actionは実装時点の安定majorと完全なcommit SHAへ固定し、コメントにmajor versionを記載する
- Dependabotによる更新を許可する場合も、Release前に差分とCIを確認する
- artifact名とアーカイブ名を一致させ、意図しないファイル混入を防ぐ

### 11.6 同時実行

同じtagまたは同じdry-run refのworkflowはconcurrency groupで直列化する。進行中の正式リリースを新しい実行で自動cancelしない。

## 12. Release smoke test

各対象OSで、アーカイブを新しい一時ディレクトリへ展開して以下を行う。

1. アーカイブ内の項目が実行ファイル、LICENSE、READMEだけであることを確認
2. 実行ファイル名と実行権限を確認
3. `cppseed --version` がタグversionと一致
4. `cppseed --help` が終了コード0
5. `cppseed new release-smoke --std 20` が終了コード0
6. 9個の生成ファイルが存在
7. 生成プロジェクトをCMakeで構成・Release build
8. 生成実行ファイルの出力を確認
9. CTestが全件成功
10. cppseed実行時にアーカイブ外のテンプレートや言語ランタイムを要求しないことを確認

smoke testはアーカイブ作成前のbuild tree内バイナリではなく、必ず展開後の実行ファイルへ対して実行する。

## 13. draft Release

tag workflow成功後、GitHub上にdraft Releaseを作成する。

### 13.1 添付物

- 4個のOS別アーカイブ
- `SHA256SUMS`

source code zipおよびtar.gzはGitHubが自動提供するものを利用する。

### 13.2 Release notes

Release notesには最低限、以下を記載する。

- v0.1.0が最初の公開リリースであること
- 主な機能
- 対象OS・CPU
- インストール手順
- SHA-256確認手順
- 生成物のビルドに必要なCMakeとコンパイラ
- 未署名バイナリに関するmacOS/Windowsの注意
- Linux ABI境界
- 既知の制限
- Issue報告先

自動生成したPR一覧だけに依存せず、利用者が導入判断できる文章を手動で確認する。

## 14. README更新

正式リリース実装PRではREADMEへ以下を追加する。

- GitHub Releasesへの導線
- 対象環境とアーカイブの選び方
- 展開とPATH配置
- SHA-256の確認方法
- `cppseed --version` による導入確認
- 生成プロジェクトにはCMakeとコンパイラが必要であること
- ソースからbuildする場合の最低versionとCI検証version
- 未署名バイナリの制約

READMEの手順はREL-011として新しい一時環境で再実行する。

## 15. privateからpublicへの移行

現在のリポジトリはprivateであるため、GitHub Release公開前にpublicへ変更する必要がある。public化は自動workflowで行わず、repository ownerがGitHub設定画面で明示的に行う。

### 15.1 public化前チェック

- Git履歴へtoken、password、秘密鍵、個人用設定を含まない
- Git commitのauthor名とemailを公開してよい
- Issue、PR、コメント、添付画像に非公開情報がない
- README、LICENSE、CONTRIBUTINGが存在する
- `SECURITY.md` または脆弱性報告方法が明記されている
- リポジトリ説明、topics、既定branchが適切
- branch protectionまたはrulesetでmainへのCIを要求する
- fork可能になることを理解している
- 名称衝突の最終確認が完了している

履歴に秘密情報がある場合、単に最新commitから削除するだけでは不十分である。public化を停止し、資格情報の失効と履歴除去を先に行う。

### 15.2 順序

1. release workflowのdry run成功
2. public化前チェック承認
3. リポジトリをpublicへ変更
4. main CIが引き続き成功することを確認
5. `v0.1.0` tagを作成・push
6. draft Releaseの全成果物を確認
7. draftを公開

## 16. 手動承認ゲート

自動検証がすべて成功しても、Releaseは自動公開しない。repository ownerが以下を確認してdraftを公開する。

- tagとcommit SHA
- 4アーカイブとSHA256SUMS
- 全release smoke test
- Release notes
- Critical、Highの未解決不具合が0件
- Mediumの未解決不具合に判断とIssueがある
- READMEのインストール手順
- public repositoryの表示内容

## 17. 失敗時の扱い

### 17.1 draft公開前

- workflow失敗時はReleaseを作成しない
- 一部環境だけ成功しても部分Releaseを公開しない
- draft Releaseに不足・不整合があればdraftを削除して再実行できる
- 公開前の誤ったtagは削除できるが、原因と操作を作業記録へ残す

### 17.2 公開後

- tagと添付済み成果物を同じversionのまま差し替えない
- 軽微な文書誤りはRelease notesを訂正し、訂正日時を追記できる
- バイナリ不良または安全性問題ではRelease notes先頭へ警告を追加し、必要なら成果物を非公開化する
- 修正版は新しいpatch versionで公開する
- 漏えいした秘密情報は直ちに失効し、通常のRelease修正とは別に対応する

## 18. リリース証跡

次をGitHub上で追跡可能にする。

- tagとcommit SHA
- main CIおよびrelease workflowのURL
- runner OS、CPU、image version
- コンパイラ、CMake、Gitのversion
- CTest結果
- dependency inspection結果
- 各smoke test結果
- 4アーカイブ
- SHA256SUMS
- 公開日時と公開担当者
- 未解決Issueとリリース判断

Actions artifactの一時保持期間を過ぎても、GitHub Releaseの成果物、tag、workflow logおよびRelease notesから公開内容を特定できることを求める。

## 19. 性能・カバレッジゲート

テスト計画に従い、v0.1.0公開前に以下を確認する。

- `cppseed new` 30回の測定で参照環境のp95が1秒以内
- `cppseed_core` の行カバレッジ90%以上
- `cppseed_core` の分岐カバレッジ80%以上

カバレッジ未達の場合は未検証箇所と理由をPRへ記載し、明示的な承認がなければ公開しない。hosted runnerの一時的な性能変動だけで判定せず、安定した参照環境の測定値を使用する。

## 20. ライセンス

- cppseed本体とリポジトリはMIT Licenseで公開する
- 各配布アーカイブへルートのLICENSEを含める
- 生成コードの利用をcppseedのライセンスで制限しない
- 生成READMEの説明とルートREADMEの説明が一致することを確認する
- 外部コードまたは新しいActionを追加する場合は、そのライセンスと配布条件を確認する

## 21. 実装順序

1. `SECURITY.md` とREADMEの公開・インストール情報を追加
2. version整合性checkを追加
3. `.github/workflows/release.yml` のdry runを実装
4. 4環境のRelease buildとpackageを実装
5. 展開後smoke testを実装
6. SHA256SUMS集約を実装
7. draft Release作成を実装
8. compatibility、coverage、performance gateを実装または判定
9. dry runをmainのcommitで成功させる
10. public化前チェックを実施
11. public化、tag、draft確認、公開を順に実施

各段階は専用branchとPRでレビューし、直接mainへ変更しない。

## 22. 完了条件

v0.1.0リリース工程は以下をすべて満たしたとき完了とする。

- リポジトリがpublicである
- `v0.1.0` annotated tagが検証済みmain commitを指す
- 4対象環境のアーカイブがGitHub Releaseへ添付されている
- SHA256SUMSが全成果物と一致する
- 全対象環境のrelease smoke testが成功している
- main CI、coverage、performanceの判定が完了している
- READMEの手順で取得、検証、生成、build、CTestまで再現できる
- Critical、Highの未解決不具合が0件である
- Release notesに対象環境と既知の制限が記載されている
- GitHub Releaseがdraftではなく公開状態である
- 要件定義書とテスト計画書の受け入れ項目が更新されている

## 23. リリース設計の受け入れ条件

- [x] version、tagおよびRelease名の対応が一意に定義されている
- [x] 4対象環境とアーカイブ名が定義されている
- [x] Linux、macOS、Windowsのruntime互換性方針が定義されている
- [x] 最低コンパイラversionが定義されている
- [x] アーカイブ内容とSHA256SUMS形式が定義されている
- [x] release workflowの起動条件、権限、job責務が定義されている
- [x] 展開後の対象OS別smoke testが定義されている
- [x] privateからpublicへの移行ゲートが定義されている
- [x] draft Releaseの手動承認項目が定義されている
- [x] 失敗、訂正、撤回の方針が定義されている
- [x] 性能・カバレッジの公開ゲートが定義されている
- [x] ライセンスと未署名バイナリの扱いが定義されている
- [x] リリース証跡と完了条件が定義されている
