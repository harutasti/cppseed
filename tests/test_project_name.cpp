#include <string>
#include <variant>
#include <vector>

#include "cppseed/project_name.hpp"
#include "test_framework.hpp"

using cppseed::Diagnostic;
using cppseed::ProjectName;

TEST_CASE("ProjectName accepts boundaries and preserves original") {
    auto minimum = ProjectName::parse("a");
    CHECK(std::holds_alternative<ProjectName>(minimum));
    CHECK_EQ(std::get<ProjectName>(minimum).original(), "a");

    const std::string maximum = "a" + std::string(63, '9');
    auto maximum_result = ProjectName::parse(maximum);
    CHECK(std::holds_alternative<ProjectName>(maximum_result));
    CHECK_EQ(std::get<ProjectName>(maximum_result).original(), maximum);
}

TEST_CASE("ProjectName normalizes case and separator runs") {
    auto result = ProjectName::parse("My-_--__App2");
    CHECK(std::holds_alternative<ProjectName>(result));
    CHECK_EQ(std::get<ProjectName>(result).normalized(), "my_app2");
}

TEST_CASE("ProjectName rejects invalid values") {
    const std::string too_long = "a" + std::string(64, 'x');
    const std::vector<std::string> invalid_values{
        "",          "1name",        "_name",       "-name",
        "bad name",  "bad/name",     "bad.name",    "project\nname",
        "プロジェクト", too_long};
    for (const auto& value : invalid_values) {
        auto result = ProjectName::parse(value);
        CHECK(std::holds_alternative<Diagnostic>(result));
        CHECK_EQ(std::get<Diagnostic>(result).exit_code,
                 cppseed::ExitCode::runtime_error);
    }
}
