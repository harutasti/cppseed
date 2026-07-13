#pragma once

#include <string>
#include <string_view>
#include <variant>

#include "cppseed/types.hpp"

namespace cppseed {

class ProjectName final {
public:
    using ParseResult = std::variant<ProjectName, Diagnostic>;

    [[nodiscard]] static ParseResult parse(std::string_view input);

    [[nodiscard]] const std::string& original() const noexcept;
    [[nodiscard]] const std::string& normalized() const noexcept;

private:
    ProjectName(std::string original, std::string normalized);

    std::string original_;
    std::string normalized_;
};

}  // namespace cppseed
