#pragma once

#include <string>
#include <string_view>
#include <variant>

#include "cppseed/types.hpp"

namespace cppseed {

struct TemplateContext {
    std::string_view project_name;
    std::string_view normalized_name;
    std::string_view cpp_standard;
    std::string_view cmake_feature;
};

class TemplateRenderer final {
public:
    using RenderResult = std::variant<std::string, Diagnostic>;

    [[nodiscard]] RenderResult render_text(
        std::string_view source, const TemplateContext& context) const;
    [[nodiscard]] RenderResult render_path(
        std::string_view source, const TemplateContext& context) const;

private:
    [[nodiscard]] RenderResult render(
        std::string_view source, const TemplateContext& context) const;
};

}  // namespace cppseed
