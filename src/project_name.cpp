#include "cppseed/project_name.hpp"

#include <utility>

#include "cppseed/diagnostic.hpp"

namespace cppseed {
namespace {

bool is_ascii_letter(const char value) noexcept {
    return (value >= 'A' && value <= 'Z') ||
           (value >= 'a' && value <= 'z');
}

bool is_ascii_digit(const char value) noexcept {
    return value >= '0' && value <= '9';
}

Diagnostic invalid_name(const std::string_view input) {
    return {
        ExitCode::runtime_error,
        "invalid project name " + quote_for_diagnostic(input),
        "use 1-64 characters: ASCII letters, numbers, hyphens, or underscores",
        std::nullopt};
}

}  // namespace

ProjectName::ProjectName(std::string original, std::string normalized)
    : original_(std::move(original)), normalized_(std::move(normalized)) {}

ProjectName::ParseResult ProjectName::parse(const std::string_view input) {
    if (input.empty() || input.size() > 64 || !is_ascii_letter(input.front())) {
        return invalid_name(input);
    }

    for (const char value : input.substr(1)) {
        if (!is_ascii_letter(value) && !is_ascii_digit(value) && value != '-' &&
            value != '_') {
            return invalid_name(input);
        }
    }

    std::string normalized;
    normalized.reserve(input.size());
    for (char value : input) {
        if (value >= 'A' && value <= 'Z') {
            value = static_cast<char>(value - 'A' + 'a');
        }
        if (value == '-' || value == '_') {
            if (normalized.empty() || normalized.back() != '_') {
                normalized += '_';
            }
        } else {
            normalized += value;
        }
    }

    return ProjectName{std::string(input), std::move(normalized)};
}

const std::string& ProjectName::original() const noexcept { return original_; }

const std::string& ProjectName::normalized() const noexcept {
    return normalized_;
}

}  // namespace cppseed
