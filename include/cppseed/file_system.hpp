#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>
#include <system_error>

namespace cppseed {

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
        const std::filesystem::path& path, std::string_view content) = 0;
    [[nodiscard]] virtual RemoveResult remove_all(
        const std::filesystem::path& path) noexcept = 0;
};

class RealFileSystem final : public FileSystem {
public:
    [[nodiscard]] PathStatus status(
        const std::filesystem::path& path) const override;
    [[nodiscard]] CreateDirectoryResult create_directory(
        const std::filesystem::path& path) override;
    [[nodiscard]] std::error_code write_text_file(
        const std::filesystem::path& path, std::string_view content) override;
    [[nodiscard]] RemoveResult remove_all(
        const std::filesystem::path& path) noexcept override;
};

}  // namespace cppseed
