#include <filesystem>
#include <string>
#include <variant>

#include "cppseed/project_generator.hpp"
#include "cppseed/template_renderer.hpp"
#include "fake_file_system.hpp"
#include "test_framework.hpp"

namespace {

cppseed::ProjectName name(const std::string& value) {
    auto result = cppseed::ProjectName::parse(value);
    CHECK(std::holds_alternative<cppseed::ProjectName>(result));
    return std::get<cppseed::ProjectName>(std::move(result));
}

cppseed::GenerationResult generate(cppseed::test::FakeFileSystem& file_system,
                                   const std::string& project_name = "demo",
                                   const cppseed::CppStandard standard =
                                       cppseed::CppStandard::cpp20) {
    cppseed::TemplateRenderer renderer;
    cppseed::ProjectGenerator generator(file_system, renderer);
    return generator.generate({name(project_name), standard}, "/workspace");
}

}  // namespace

TEST_CASE("ProjectGenerator creates four directories and nine files") {
    cppseed::test::FakeFileSystem file_system;
    const auto result = generate(file_system, "fem-solver",
                                 cppseed::CppStandard::cpp23);
    CHECK(!result);
    CHECK_EQ(file_system.size(), 14U);
    CHECK(file_system.contains("/workspace/fem-solver/include/fem_solver/hello.hpp"));
    CHECK(file_system.contains("/workspace/fem-solver/tests/hello_test.cpp"));
    const auto& cmake = file_system.at("/workspace/fem-solver/CMakeLists.txt").content;
    CHECK_NE(cmake.find("cxx_std_23"), std::string::npos);
    CHECK_EQ(cmake.find("{{"), std::string::npos);
    CHECK(cmake.ends_with('\n'));
}

TEST_CASE("ProjectGenerator preserves existing paths of every kind") {
    for (const auto type : {std::filesystem::file_type::regular,
                            std::filesystem::file_type::directory,
                            std::filesystem::file_type::symlink}) {
        cppseed::test::FakeFileSystem file_system;
        if (type == std::filesystem::file_type::regular) {
            file_system.add_file("/workspace/demo", "original");
        } else if (type == std::filesystem::file_type::directory) {
            file_system.add_directory("/workspace/demo");
        } else {
            file_system.add_symlink("/workspace/demo");
        }
        const auto result = generate(file_system);
        CHECK(result);
        CHECK(file_system.contains("/workspace/demo"));
        CHECK_EQ(file_system.size(), 1U);
    }
}

TEST_CASE("ProjectGenerator rolls back every creation-stage failure") {
    for (std::size_t operation = 3; operation <= 15; ++operation) {
        cppseed::test::FakeFileSystem file_system;
        file_system.fail_on_operation(operation);
        const auto result = generate(file_system);
        CHECK(result);
        CHECK(!file_system.contains("/workspace/demo"));
        CHECK_EQ(file_system.size(), 0U);
    }
}

TEST_CASE("ProjectGenerator reports incomplete rollback") {
    cppseed::test::FakeFileSystem file_system;
    file_system.fail_on_operation(7);
    file_system.fail_rollback();
    const auto result = generate(file_system);
    CHECK(result);
    CHECK(result->warning);
    CHECK(file_system.contains("/workspace/demo"));
}

TEST_CASE("ProjectGenerator does not remove a root it did not create") {
    cppseed::test::FakeFileSystem file_system;
    file_system.fail_on_operation(2);
    const auto result = generate(file_system);
    CHECK(result);
    CHECK_EQ(file_system.size(), 0U);
    for (const auto& operation : file_system.operations()) {
        CHECK(!operation.starts_with("remove:"));
    }
}
