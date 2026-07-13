#include <sstream>
#include <string_view>
#include <vector>

#include "cppseed/application.hpp"
#include "fake_file_system.hpp"
#include "test_framework.hpp"

namespace {

struct RunResult {
    int exit_code;
    std::string standard_output;
    std::string standard_error;
};

RunResult run_with(cppseed::test::FakeFileSystem& file_system,
                   const std::initializer_list<std::string_view> arguments) {
    std::ostringstream output;
    std::ostringstream error;
    cppseed::ApplicationContext context{file_system, "/workspace", output,
                                        error};
    const auto exit_code = cppseed::run(
        std::span<const std::string_view>{arguments.begin(), arguments.size()},
        context);
    return {exit_code, output.str(), error.str()};
}

}  // namespace

TEST_CASE("Application routes informational output to stdout") {
    cppseed::test::FakeFileSystem file_system;
    for (const auto argument : {"--help", "--version"}) {
        const auto result = run_with(file_system, {argument});
        CHECK_EQ(result.exit_code, 0);
        CHECK(!result.standard_output.empty());
        CHECK(result.standard_error.empty());
    }
}

TEST_CASE("Application routes usage errors to stderr") {
    cppseed::test::FakeFileSystem file_system;
    const auto result = run_with(file_system, {});
    CHECK_EQ(result.exit_code, 2);
    CHECK(result.standard_output.empty());
    CHECK_EQ(result.standard_error, "error: a command is required\n");
}

TEST_CASE("Application reports success only after generation") {
    cppseed::test::FakeFileSystem file_system;
    const auto result = run_with(file_system, {"new", "fem-solver"});
    CHECK_EQ(result.exit_code, 0);
    CHECK(result.standard_error.empty());
    CHECK_EQ(result.standard_output,
             "Created C++ project `fem-solver` in ./fem-solver\n\n"
             "Next steps:\n"
             "  cd fem-solver\n"
             "  cmake -S . -B build\n"
             "  cmake --build build\n"
             "  ctest --test-dir build --output-on-failure\n");
}

TEST_CASE("Application suppresses success output when generation fails") {
    cppseed::test::FakeFileSystem file_system;
    file_system.fail_on_operation(7);
    const auto result = run_with(file_system, {"new", "demo"});
    CHECK_EQ(result.exit_code, 1);
    CHECK(result.standard_output.empty());
    CHECK(result.standard_error.starts_with("error: failed to create"));
}
