#include "cppseed/project_generator.hpp"

#include <algorithm>
#include <array>
#include <set>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "cppseed/diagnostic.hpp"
#include "cppseed/template_catalog.hpp"

namespace cppseed {
namespace {

struct RenderedFile {
    std::filesystem::path relative_path;
    std::string content;
};

class GenerationTransaction final {
public:
    GenerationTransaction(FileSystem& file_system, std::filesystem::path root)
        : file_system_(file_system), root_(std::move(root)) {}

    GenerationTransaction(const GenerationTransaction&) = delete;
    GenerationTransaction& operator=(const GenerationTransaction&) = delete;

    void mark_root_created() noexcept { root_created_ = true; }
    void commit() noexcept { committed_ = true; }

    [[nodiscard]] std::error_code rollback() noexcept {
        if (!root_created_ || committed_ || rollback_attempted_) {
            return {};
        }
        rollback_attempted_ = true;
        return file_system_.remove_all(root_).error;
    }

    ~GenerationTransaction() noexcept {
        if (!rollback_attempted_) {
            static_cast<void>(rollback());
        }
    }

private:
    FileSystem& file_system_;
    std::filesystem::path root_;
    bool root_created_ = false;
    bool committed_ = false;
    bool rollback_attempted_ = false;
};

Diagnostic runtime_error(std::string message) {
    return {ExitCode::runtime_error, std::move(message), std::nullopt,
            std::nullopt};
}

bool is_safe_relative_path(const std::filesystem::path& path) {
    if (path.empty() || path.is_absolute() || path.has_root_name() ||
        path.has_root_directory()) {
        return false;
    }
    for (const auto& component : path) {
        if (component.empty() || component == "." || component == "..") {
            return false;
        }
    }
    return true;
}

Diagnostic with_rollback_warning(Diagnostic diagnostic,
                                 GenerationTransaction& transaction,
                                 const std::string& display_root) {
    if (transaction.rollback()) {
        diagnostic.warning =
            "cleanup was incomplete; inspect " + quote_for_diagnostic(display_root);
    }
    return diagnostic;
}

}  // namespace

ProjectGenerator::ProjectGenerator(FileSystem& file_system,
                                   const TemplateRenderer& renderer)
    : file_system_(file_system), renderer_(renderer) {}

GenerationResult ProjectGenerator::generate(
    const ProjectSpec& spec,
    const std::filesystem::path& current_directory) const {
    const auto root = current_directory / spec.project_name.original();
    const auto display_root = std::string{"./"} + spec.project_name.original();
    const auto root_status = file_system_.status(root);
    if (root_status.error) {
        return runtime_error("cannot create project in " +
                             quote_for_diagnostic(display_root));
    }
    if (root_status.value.type() != std::filesystem::file_type::not_found) {
        return runtime_error("path " +
                             quote_for_diagnostic(spec.project_name.original()) +
                             " already exists");
    }

    const TemplateContext context{spec.project_name.original(),
                                  spec.project_name.normalized(),
                                  to_number(spec.cpp_standard),
                                  to_cmake_feature(spec.cpp_standard)};
    std::vector<RenderedFile> rendered_files;
    rendered_files.reserve(project_templates().size());
    std::set<std::filesystem::path> rendered_paths;
    for (const auto& template_spec : project_templates()) {
        auto path_result = renderer_.render_path(
            template_spec.relative_path_template, context);
        if (const auto* diagnostic = std::get_if<Diagnostic>(&path_result)) {
            return *diagnostic;
        }
        auto content_result =
            renderer_.render_text(template_spec.content_template, context);
        if (const auto* diagnostic = std::get_if<Diagnostic>(&content_result)) {
            return *diagnostic;
        }

        std::filesystem::path relative_path{
            std::get<std::string>(std::move(path_result))};
        if (!is_safe_relative_path(relative_path)) {
            return runtime_error("invalid embedded project template");
        }
        const auto normalized_destination =
            (root / relative_path).lexically_normal();
        const auto normalized_root = root.lexically_normal();
        auto mismatch = std::mismatch(normalized_root.begin(), normalized_root.end(),
                                      normalized_destination.begin(),
                                      normalized_destination.end());
        if (mismatch.first != normalized_root.end()) {
            return runtime_error("invalid embedded project template");
        }
        if (!rendered_paths.insert(relative_path).second) {
            return runtime_error("invalid embedded project template");
        }
        rendered_files.push_back(
            {std::move(relative_path),
             std::get<std::string>(std::move(content_result))});
    }

    GenerationTransaction transaction(file_system_, root);
    const auto root_creation = file_system_.create_directory(root);
    if (root_creation.error) {
        return runtime_error("cannot create project in " +
                             quote_for_diagnostic(display_root));
    }
    if (!root_creation.created) {
        return runtime_error("path " +
                             quote_for_diagnostic(spec.project_name.original()) +
                             " already exists");
    }
    transaction.mark_root_created();

    const std::array<std::filesystem::path, 4> directories{
        "include", std::filesystem::path{"include"} /
                       spec.project_name.normalized(),
        "src", "tests"};
    for (const auto& directory : directories) {
        const auto result = file_system_.create_directory(root / directory);
        if (result.error || !result.created) {
            return with_rollback_warning(
                runtime_error("failed to create " +
                              quote_for_diagnostic(directory.generic_string())),
                transaction, display_root);
        }
    }

    for (const auto& rendered : rendered_files) {
        const auto error = file_system_.write_text_file(
            root / rendered.relative_path, rendered.content);
        if (error) {
            return with_rollback_warning(
                runtime_error("failed to create " + quote_for_diagnostic(
                    rendered.relative_path.generic_string())),
                transaction, display_root);
        }
    }

    transaction.commit();
    return std::nullopt;
}

}  // namespace cppseed
