#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace cppseed {

enum class ExitCode : int {
    success = 0,
    runtime_error = 1,
    usage_error = 2,
};

struct Diagnostic {
    ExitCode exit_code;
    std::string message;
    std::optional<std::string> hint;
    std::optional<std::string> warning;
};

enum class CppStandard : int {
    cpp17 = 17,
    cpp20 = 20,
    cpp23 = 23,
};

[[nodiscard]] std::string_view to_number(CppStandard value) noexcept;
[[nodiscard]] std::string_view to_cmake_feature(CppStandard value) noexcept;

}  // namespace cppseed
