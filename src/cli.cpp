#include "cppseed/cli.hpp"

#include <optional>
#include <utility>

#include "cppseed/diagnostic.hpp"

namespace cppseed {
namespace {

constexpr std::string_view top_help_text = R"(Create small, buildable C++ projects.

Usage:
  cppseed <COMMAND>
  cppseed [OPTIONS]

Commands:
  new <PROJECT_NAME>  Create a new C++ project

Options:
  -h, --help     Show help
  -V, --version  Show version
)";

constexpr std::string_view new_help_text = R"(Create a new C++ project.

Usage:
  cppseed new <PROJECT_NAME> [--std <17|20|23>]

Arguments:
  <PROJECT_NAME>  Project name and output directory

Options:
      --std <17|20|23>  C++ standard [default: 20]
  -h, --help            Show help
)";

constexpr std::string_view version = "cppseed 0.1.0\n";

Diagnostic usage_error(std::string message,
                       std::optional<std::string> hint = std::nullopt) {
    return {ExitCode::usage_error, std::move(message), std::move(hint),
            std::nullopt};
}

std::optional<CppStandard> parse_standard(const std::string_view value) {
    if (value == "17") {
        return CppStandard::cpp17;
    }
    if (value == "20") {
        return CppStandard::cpp20;
    }
    if (value == "23") {
        return CppStandard::cpp23;
    }
    return std::nullopt;
}

ParseResult parse_new(const std::span<const std::string_view> arguments) {
    if (arguments.size() == 1 &&
        (arguments.front() == "-h" || arguments.front() == "--help")) {
        return Command{ShowNewHelp{}};
    }

    std::optional<std::string_view> project_name;
    std::optional<CppStandard> standard;
    for (std::size_t index = 0; index < arguments.size(); ++index) {
        const auto argument = arguments[index];
        if (argument == "--std") {
            if (standard) {
                return usage_error(
                    "option `--std` was provided more than once");
            }
            if (index + 1 >= arguments.size()) {
                return usage_error("option `--std` requires a value");
            }
            const auto value = arguments[++index];
            const auto parsed = parse_standard(value);
            if (!parsed) {
                return usage_error(
                    "unsupported C++ standard " + quote_for_diagnostic(value),
                    "choose 17, 20, or 23");
            }
            standard = *parsed;
            continue;
        }
        if (!argument.empty() && argument.front() == '-') {
            return usage_error("unknown option " + quote_for_diagnostic(argument));
        }
        if (project_name) {
            return usage_error("unexpected argument " +
                               quote_for_diagnostic(argument));
        }
        project_name = argument;
    }

    if (!project_name) {
        return usage_error("project name is required");
    }
    auto parsed_name = ProjectName::parse(*project_name);
    if (const auto* diagnostic = std::get_if<Diagnostic>(&parsed_name)) {
        return *diagnostic;
    }
    return Command{NewCommand{std::get<ProjectName>(std::move(parsed_name)),
                              standard.value_or(CppStandard::cpp20)}};
}

}  // namespace

ParseResult parse_cli(const std::span<const std::string_view> arguments) {
    if (arguments.empty()) {
        return usage_error("a command is required");
    }

    const auto first = arguments.front();
    if (first == "-h" || first == "--help") {
        if (arguments.size() == 1) {
            return Command{ShowTopLevelHelp{}};
        }
        return usage_error("unexpected argument " +
                           quote_for_diagnostic(arguments[1]));
    }
    if (first == "-V" || first == "--version") {
        if (arguments.size() == 1) {
            return Command{ShowVersion{}};
        }
        return usage_error("unexpected argument " +
                           quote_for_diagnostic(arguments[1]));
    }
    if (first == "new") {
        return parse_new(arguments.subspan(1));
    }
    if (!first.empty() && first.front() == '-') {
        return usage_error("unknown option " + quote_for_diagnostic(first));
    }
    return usage_error("unknown command " + quote_for_diagnostic(first));
}

std::string_view top_level_help() noexcept { return top_help_text; }
std::string_view new_help() noexcept { return new_help_text; }
std::string_view version_text() noexcept { return version; }

}  // namespace cppseed
