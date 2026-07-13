#pragma once

#include <filesystem>
#include <optional>

#include "cppseed/file_system.hpp"
#include "cppseed/project_name.hpp"
#include "cppseed/template_renderer.hpp"
#include "cppseed/types.hpp"

namespace cppseed {

struct ProjectSpec {
    ProjectName project_name;
    CppStandard cpp_standard;
};

using GenerationResult = std::optional<Diagnostic>;

class ProjectGenerator final {
public:
    ProjectGenerator(FileSystem& file_system, const TemplateRenderer& renderer);

    [[nodiscard]] GenerationResult generate(
        const ProjectSpec& spec,
        const std::filesystem::path& current_directory) const;

private:
    FileSystem& file_system_;
    const TemplateRenderer& renderer_;
};

}  // namespace cppseed
