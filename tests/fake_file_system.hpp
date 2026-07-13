#pragma once

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "cppseed/file_system.hpp"

namespace cppseed::test {

class FakeFileSystem final : public FileSystem {
public:
    struct Entry {
        std::filesystem::file_type type;
        std::string content;
    };

    [[nodiscard]] PathStatus status(
        const std::filesystem::path& path) const override;
    [[nodiscard]] CreateDirectoryResult create_directory(
        const std::filesystem::path& path) override;
    [[nodiscard]] std::error_code write_text_file(
        const std::filesystem::path& path, std::string_view content) override;
    [[nodiscard]] RemoveResult remove_all(
        const std::filesystem::path& path) noexcept override;

    void add_directory(const std::filesystem::path& path);
    void add_file(const std::filesystem::path& path, std::string content = {});
    void add_symlink(const std::filesystem::path& path);
    void fail_on_operation(std::size_t operation);
    void fail_rollback(bool enabled = true);

    [[nodiscard]] bool contains(const std::filesystem::path& path) const;
    [[nodiscard]] const Entry& at(const std::filesystem::path& path) const;
    [[nodiscard]] const std::vector<std::string>& operations() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;

private:
    [[nodiscard]] static std::string key(const std::filesystem::path& path);
    [[nodiscard]] bool should_fail() const;

    mutable std::size_t operation_count_ = 0;
    std::optional<std::size_t> failing_operation_;
    bool fail_rollback_ = false;
    std::map<std::string, Entry> entries_;
    mutable std::vector<std::string> operations_;
};

}  // namespace cppseed::test
