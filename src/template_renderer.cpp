#include "cppseed/template_renderer.hpp"

#include <optional>

namespace cppseed {
namespace {

Diagnostic template_error() {
    return {ExitCode::runtime_error, "invalid embedded project template",
            std::nullopt, std::nullopt};
}

std::optional<std::string_view> token_value(
    const std::string_view token, const TemplateContext& context) {
    if (token == "{{PROJECT_NAME}}") {
        return context.project_name;
    }
    if (token == "{{NORMALIZED_NAME}}") {
        return context.normalized_name;
    }
    if (token == "{{CPP_STANDARD}}") {
        return context.cpp_standard;
    }
    if (token == "{{CMAKE_FEATURE}}") {
        return context.cmake_feature;
    }
    return std::nullopt;
}

}  // namespace

TemplateRenderer::RenderResult TemplateRenderer::render(
    const std::string_view source, const TemplateContext& context) const {
    std::string output;
    output.reserve(source.size());
    std::size_t position = 0;
    while (position < source.size()) {
        const auto opening = source.find("{{", position);
        const auto stray_closing = source.find("}}", position);
        if (opening == std::string_view::npos) {
            if (stray_closing != std::string_view::npos) {
                return template_error();
            }
            output.append(source.substr(position));
            break;
        }
        if (stray_closing != std::string_view::npos && stray_closing < opening) {
            return template_error();
        }
        output.append(source.substr(position, opening - position));
        const auto closing = source.find("}}", opening + 2);
        if (closing == std::string_view::npos) {
            return template_error();
        }
        const auto token = source.substr(opening, closing + 2 - opening);
        const auto value = token_value(token, context);
        if (!value) {
            return template_error();
        }
        output.append(*value);
        position = closing + 2;
    }
    if (output.find("{{") != std::string::npos ||
        output.find("}}") != std::string::npos) {
        return template_error();
    }
    return output;
}

TemplateRenderer::RenderResult TemplateRenderer::render_text(
    const std::string_view source, const TemplateContext& context) const {
    auto result = render(source, context);
    auto* text = std::get_if<std::string>(&result);
    if (text != nullptr && (text->empty() || text->back() != '\n')) {
        *text += '\n';
    }
    return result;
}

TemplateRenderer::RenderResult TemplateRenderer::render_path(
    const std::string_view source, const TemplateContext& context) const {
    auto result = render(source, context);
    const auto* path = std::get_if<std::string>(&result);
    if (path != nullptr &&
        (path->find('\0') != std::string::npos ||
         path->find('\r') != std::string::npos ||
         path->find('\n') != std::string::npos)) {
        return template_error();
    }
    return result;
}

}  // namespace cppseed
