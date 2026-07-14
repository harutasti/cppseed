#include "fake_file_system.hpp"

#include <algorithm>
#include <stdexcept>

namespace cppseed::test {

std::string FakeFileSystem::key(const std::filesystem::path& path) {
    return path.lexically_normal().generic_string();
}

bool FakeFileSystem::should_fail() const {
    ++operation_count_;
    return failing_operation_ && operation_count_ == *failing_operation_;
}

PathStatus FakeFileSystem::status(const std::filesystem::path& path) const {
    operations_.push_back("status:" + key(path));
    if (should_fail()) {
        return {{}, std::make_error_code(std::errc::io_error)};
    }
    const auto found = entries_.find(key(path));
    if (found == entries_.end()) {
        return {std::filesystem::file_status{
                    std::filesystem::file_type::not_found},
                {}};
    }
    return {std::filesystem::file_status{found->second.type}, {}};
}

CreateDirectoryResult FakeFileSystem::create_directory(
    const std::filesystem::path& path) {
    operations_.push_back("mkdir:" + key(path));
    if (should_fail()) {
        return {false, std::make_error_code(std::errc::io_error)};
    }
    const auto [iterator, inserted] = entries_.emplace(
        key(path), Entry{std::filesystem::file_type::directory, {}});
    static_cast<void>(iterator);
    return {inserted, {}};
}

std::error_code FakeFileSystem::write_text_file(
    const std::filesystem::path& path, const std::string_view content) {
    operations_.push_back("write:" + key(path));
    if (should_fail()) {
        return std::make_error_code(std::errc::io_error);
    }
    const auto path_key = key(path);
    if (entries_.contains(path_key)) {
        return std::make_error_code(std::errc::file_exists);
    }
    entries_.emplace(path_key,
                     Entry{std::filesystem::file_type::regular,
                           std::string(content)});
    return {};
}

RemoveResult FakeFileSystem::remove_all(
    const std::filesystem::path& path) noexcept {
    operations_.push_back("remove:" + key(path));
    if (fail_rollback_) {
        return {0, std::make_error_code(std::errc::io_error)};
    }
    const auto root = key(path);
    std::uintmax_t removed = 0;
    for (auto iterator = entries_.begin(); iterator != entries_.end();) {
        const bool exact = iterator->first == root;
        const bool child = iterator->first.size() > root.size() &&
                           iterator->first.starts_with(root + "/");
        if (exact || child) {
            iterator = entries_.erase(iterator);
            ++removed;
        } else {
            ++iterator;
        }
    }
    return {removed, {}};
}

void FakeFileSystem::add_directory(const std::filesystem::path& path) {
    entries_[key(path)] = {std::filesystem::file_type::directory, {}};
}

void FakeFileSystem::add_file(const std::filesystem::path& path,
                              std::string content) {
    entries_[key(path)] =
        {std::filesystem::file_type::regular, std::move(content)};
}

void FakeFileSystem::add_symlink(const std::filesystem::path& path) {
    entries_[key(path)] = {std::filesystem::file_type::symlink, {}};
}

void FakeFileSystem::fail_on_operation(const std::size_t operation) {
    failing_operation_ = operation;
}

void FakeFileSystem::fail_rollback(const bool enabled) {
    fail_rollback_ = enabled;
}

bool FakeFileSystem::contains(const std::filesystem::path& path) const {
    return entries_.contains(key(path));
}

const FakeFileSystem::Entry& FakeFileSystem::at(
    const std::filesystem::path& path) const {
    const auto found = entries_.find(key(path));
    if (found == entries_.end()) {
        throw std::out_of_range("fake path not found");
    }
    return found->second;
}

const std::vector<std::string>& FakeFileSystem::operations() const noexcept {
    return operations_;
}

std::size_t FakeFileSystem::size() const noexcept { return entries_.size(); }

}  // namespace cppseed::test
