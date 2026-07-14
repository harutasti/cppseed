#pragma once

#include <filesystem>
#include <iosfwd>
#include <span>
#include <string>
#include <string_view>

#include "cppseed/file_system.hpp"
#include "cppseed/project_name.hpp"

namespace cppseed {

struct ApplicationContext {
    FileSystem& file_system;
    std::filesystem::path current_directory;
    std::ostream& standard_output;
    std::ostream& standard_error;
};

[[nodiscard]] int run(
    std::span<const std::string_view> arguments, ApplicationContext& context);
[[nodiscard]] std::string make_success_message(
    const ProjectName& project_name);

}  // namespace cppseed
