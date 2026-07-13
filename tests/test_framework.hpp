#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cppseed::test {

struct TestCase {
    std::string name;
    std::function<void()> function;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

class Registrar final {
public:
    Registrar(std::string name, std::function<void()> function) {
        registry().push_back({std::move(name), std::move(function)});
    }
};

[[noreturn]] inline void fail(const std::string_view expression,
                              const std::string_view file, const int line) {
    std::ostringstream message;
    message << file << ':' << line << ": check failed: " << expression;
    throw std::runtime_error(message.str());
}

}  // namespace cppseed::test

#define CPPSEED_TEST_CONCAT_INNER(a, b) a##b
#define CPPSEED_TEST_CONCAT(a, b) CPPSEED_TEST_CONCAT_INNER(a, b)
#define TEST_CASE(name)                                                       \
    static void CPPSEED_TEST_CONCAT(test_function_, __LINE__)();              \
    static ::cppseed::test::Registrar CPPSEED_TEST_CONCAT(test_registrar_,    \
                                                           __LINE__){          \
        name, CPPSEED_TEST_CONCAT(test_function_, __LINE__)};                 \
    static void CPPSEED_TEST_CONCAT(test_function_, __LINE__)()
#define CHECK(expression)                                                     \
    do {                                                                      \
        if (!(expression)) {                                                  \
            ::cppseed::test::fail(#expression, __FILE__, __LINE__);           \
        }                                                                     \
    } while (false)
#define CHECK_EQ(actual, expected) CHECK((actual) == (expected))
#define CHECK_NE(actual, expected) CHECK((actual) != (expected))
