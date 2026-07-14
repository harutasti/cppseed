#pragma once

#include <span>
#include <string_view>

namespace cppseed {

struct TemplateSpec {
    std::string_view relative_path_template;
    std::string_view content_template;
};

[[nodiscard]] std::span<const TemplateSpec> project_templates() noexcept;

}  // namespace cppseed
