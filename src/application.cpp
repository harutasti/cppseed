#include "cppseed/application.hpp"

#include <sstream>
#include <variant>

#include "cppseed/cli.hpp"
#include "cppseed/diagnostic.hpp"
#include "cppseed/project_generator.hpp"
#include "cppseed/template_renderer.hpp"

namespace cppseed {

std::string make_success_message(const ProjectName& project_name) {
    std::ostringstream output;
    output << "Created C++ project `" << project_name.original() << "` in ./"
           << project_name.original() << "\n\n"
           << "Next steps:\n"
           << "  cd " << project_name.original() << '\n'
           << "  cmake -S . -B build\n"
           << "  cmake --build build\n"
           << "  ctest --test-dir build --output-on-failure\n";
    return output.str();
}

int run(const std::span<const std::string_view> arguments,
        ApplicationContext& context) {
    auto parse_result = parse_cli(arguments);
    if (const auto* diagnostic = std::get_if<Diagnostic>(&parse_result)) {
        print_diagnostic(*diagnostic, context.standard_error);
        return static_cast<int>(diagnostic->exit_code);
    }

    auto command = std::get<Command>(std::move(parse_result));
    if (std::holds_alternative<ShowTopLevelHelp>(command)) {
        context.standard_output << top_level_help();
        return static_cast<int>(ExitCode::success);
    }
    if (std::holds_alternative<ShowNewHelp>(command)) {
        context.standard_output << new_help();
        return static_cast<int>(ExitCode::success);
    }
    if (std::holds_alternative<ShowVersion>(command)) {
        context.standard_output << version_text();
        return static_cast<int>(ExitCode::success);
    }

    auto new_command = std::get<NewCommand>(std::move(command));
    TemplateRenderer renderer;
    ProjectGenerator generator(context.file_system, renderer);
    const ProjectSpec spec{std::move(new_command.project_name),
                           new_command.cpp_standard};
    const auto result = generator.generate(spec, context.current_directory);
    if (result) {
        print_diagnostic(*result, context.standard_error);
        return static_cast<int>(result->exit_code);
    }
    context.standard_output << make_success_message(spec.project_name);
    return static_cast<int>(ExitCode::success);
}

}  // namespace cppseed
