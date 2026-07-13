#include "cppseed/file_system.hpp"

#include <fstream>

namespace cppseed {

PathStatus RealFileSystem::status(
    const std::filesystem::path& path) const {
    std::error_code error;
    auto value = std::filesystem::symlink_status(path, error);
    if (error == std::errc::no_such_file_or_directory) {
        error.clear();
        value = std::filesystem::file_status{
            std::filesystem::file_type::not_found};
    }
    return {value, error};
}

CreateDirectoryResult RealFileSystem::create_directory(
    const std::filesystem::path& path) {
    std::error_code error;
    const bool created = std::filesystem::create_directory(path, error);
    return {created, error};
}

std::error_code RealFileSystem::write_text_file(
    const std::filesystem::path& path, const std::string_view content) {
    const auto existing = status(path);
    if (existing.error) {
        return existing.error;
    }
    if (existing.value.type() != std::filesystem::file_type::not_found) {
        return std::make_error_code(std::errc::file_exists);
    }

    std::ofstream stream(path, std::ios::binary | std::ios::out);
    if (!stream.is_open()) {
        return std::make_error_code(std::errc::io_error);
    }
    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
    stream.close();
    if (!stream) {
        return std::make_error_code(std::errc::io_error);
    }
    return {};
}

RemoveResult RealFileSystem::remove_all(
    const std::filesystem::path& path) noexcept {
    try {
        std::error_code error;
        const auto removed = std::filesystem::remove_all(path, error);
        return {removed, error};
    } catch (...) {
        return {0, std::make_error_code(std::errc::io_error)};
    }
}

}  // namespace cppseed
