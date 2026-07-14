#include <sstream>
#include <string>

#include "cppseed/diagnostic.hpp"
#include "test_framework.hpp"

TEST_CASE("Diagnostic prints error hint and warning in order") {
    std::ostringstream output;
    cppseed::print_diagnostic(
        {cppseed::ExitCode::runtime_error, "broken", "try again", "inspect"},
        output);
    CHECK_EQ(output.str(),
             "error: broken\nhint: try again\nwarning: inspect\n");
}

TEST_CASE("Diagnostic quoting escapes controls and truncates") {
    CHECK_EQ(cppseed::quote_for_diagnostic("a\n\tb"), "`a\\x0A\\x09b`");
    const std::string long_value(129, 'x');
    const auto quoted = cppseed::quote_for_diagnostic(long_value);
    CHECK(quoted.ends_with("...`"));
    CHECK_EQ(quoted.size(), 133U);
}
