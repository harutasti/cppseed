#include <string>
#include <variant>

#include "cppseed/template_renderer.hpp"
#include "test_framework.hpp"

namespace {

const cppseed::TemplateContext context{"fem-solver", "fem_solver", "23",
                                       "cxx_std_23"};

}  // namespace

TEST_CASE("TemplateRenderer replaces all supported tokens") {
    cppseed::TemplateRenderer renderer;
    auto result = renderer.render_text(
        "{{PROJECT_NAME}} {{NORMALIZED_NAME}} {{CPP_STANDARD}} "
        "{{CMAKE_FEATURE}}",
        context);
    CHECK(std::holds_alternative<std::string>(result));
    CHECK_EQ(std::get<std::string>(result),
             "fem-solver fem_solver 23 cxx_std_23\n");
}

TEST_CASE("TemplateRenderer does not recursively replace values") {
    cppseed::TemplateRenderer renderer;
    const cppseed::TemplateContext special{"{{CPP_STANDARD}}", "safe", "20",
                                           "cxx_std_20"};
    auto result = renderer.render_text("{{PROJECT_NAME}}", special);
    CHECK(std::holds_alternative<cppseed::Diagnostic>(result));
}

TEST_CASE("TemplateRenderer rejects malformed tokens") {
    cppseed::TemplateRenderer renderer;
    for (const std::string source : {"{{UNKNOWN}}", "{{PROJECT_NAME}", "{{}}",
                                     "stray }}"}) {
        auto result = renderer.render_text(source, context);
        CHECK(std::holds_alternative<cppseed::Diagnostic>(result));
    }
}

TEST_CASE("TemplateRenderer keeps path free of added newlines") {
    cppseed::TemplateRenderer renderer;
    auto result = renderer.render_path("include/{{NORMALIZED_NAME}}/hello.hpp",
                                       context);
    CHECK(std::holds_alternative<std::string>(result));
    CHECK_EQ(std::get<std::string>(result), "include/fem_solver/hello.hpp");
}
