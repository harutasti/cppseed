#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "cppseed/file_system.hpp"
#include "test_framework.hpp"

namespace {

class TemporaryDirectory final {
public:
    TemporaryDirectory() {
        const auto nonce = std::chrono::steady_clock::now()
                               .time_since_epoch()
                               .count();
        path_ = std::filesystem::temp_directory_path() /
                ("cppseed-test-" + std::to_string(nonce));
        if (!std::filesystem::create_directory(path_)) {
            throw std::runtime_error("failed to create test directory");
        }
    }

    ~TemporaryDirectory() { std::filesystem::remove_all(path_); }

    [[nodiscard]] const std::filesystem::path& path() const noexcept {
        return path_;
    }

private:
    std::filesystem::path path_;
};

}  // namespace

TEST_CASE("RealFileSystem creates and writes without overwriting") {
    TemporaryDirectory temporary;
    cppseed::RealFileSystem file_system;
    const auto missing = file_system.status(temporary.path() / "missing");
    CHECK(!missing.error);
    CHECK_EQ(missing.value.type(), std::filesystem::file_type::not_found);

    const auto directory = file_system.create_directory(temporary.path() / "dir");
    CHECK(directory.created);
    CHECK(!directory.error);
    const auto repeated = file_system.create_directory(temporary.path() / "dir");
    CHECK(!repeated.created);
    CHECK(!repeated.error);

    const auto file = temporary.path() / "dir" / "content.txt";
    CHECK(!file_system.write_text_file(file, "hello\n"));
    CHECK(file_system.write_text_file(file, "changed\n"));
    std::ifstream input(file, std::ios::binary);
    const std::string content{std::istreambuf_iterator<char>{input},
                              std::istreambuf_iterator<char>{}};
    CHECK_EQ(content, "hello\n");
}
