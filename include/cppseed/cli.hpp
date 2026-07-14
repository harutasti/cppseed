#pragma once

#include <span>
#include <string_view>
#include <variant>

#include "cppseed/project_name.hpp"
#include "cppseed/types.hpp"

namespace cppseed {

struct ShowTopLevelHelp {};
struct ShowNewHelp {};
struct ShowVersion {};

struct NewCommand {
    ProjectName project_name;
    CppStandard cpp_standard = CppStandard::cpp20;
};

using Command =
    std::variant<ShowTopLevelHelp, ShowNewHelp, ShowVersion, NewCommand>;
using ParseResult = std::variant<Command, Diagnostic>;

[[nodiscard]] ParseResult parse_cli(
    std::span<const std::string_view> arguments);
[[nodiscard]] std::string_view top_level_help() noexcept;
[[nodiscard]] std::string_view new_help() noexcept;
[[nodiscard]] std::string_view version_text() noexcept;

}  // namespace cppseed
