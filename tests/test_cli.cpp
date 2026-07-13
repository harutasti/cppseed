#include <array>
#include <string_view>
#include <variant>

#include "cppseed/cli.hpp"
#include "test_framework.hpp"

namespace {

cppseed::ParseResult parse(
    const std::initializer_list<std::string_view> arguments) {
    return cppseed::parse_cli(
        std::span<const std::string_view>{arguments.begin(), arguments.size()});
}

const cppseed::Diagnostic& diagnostic(const cppseed::ParseResult& result) {
    CHECK(std::holds_alternative<cppseed::Diagnostic>(result));
    return std::get<cppseed::Diagnostic>(result);
}

}  // namespace

TEST_CASE("CLI parses global help and version") {
    CHECK(std::holds_alternative<cppseed::Command>(parse({"--help"})));
    CHECK(std::holds_alternative<cppseed::Command>(parse({"-V"})));
    CHECK_EQ(cppseed::version_text(), "cppseed 0.1.0\n");
}

TEST_CASE("CLI parses new with defaults and option ordering") {
    for (auto result : {parse({"new", "demo"}),
                        parse({"new", "--std", "23", "demo"}),
                        parse({"new", "demo", "--std", "17"})}) {
        CHECK(std::holds_alternative<cppseed::Command>(result));
        const auto& command = std::get<cppseed::Command>(result);
        CHECK(std::holds_alternative<cppseed::NewCommand>(command));
    }
}

TEST_CASE("CLI rejects missing and unknown inputs with usage code") {
    for (const auto& result : {parse({}), parse({"unknown"}), parse({"--bad"}),
                               parse({"new"}), parse({"new", "demo", "extra"}),
                               parse({"new", "demo", "--std"}),
                               parse({"new", "demo", "--std", "14"}),
                               parse({"new", "demo", "--std=20"}),
                               parse({"new", "demo", "--std", "20", "--std",
                                      "23"})}) {
        CHECK_EQ(diagnostic(result).exit_code, cppseed::ExitCode::usage_error);
    }
}

TEST_CASE("CLI reports invalid project names as runtime errors") {
    const auto result = parse({"new", "bad/name"});
    CHECK_EQ(diagnostic(result).exit_code, cppseed::ExitCode::runtime_error);
}
