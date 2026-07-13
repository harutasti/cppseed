#include "test_framework.hpp"

int main() {
    std::size_t failed = 0;
    for (const auto& test : cppseed::test::registry()) {
        try {
            test.function();
            std::cout << "[PASS] " << test.name << '\n';
        } catch (const std::exception& error) {
            ++failed;
            std::cerr << "[FAIL] " << test.name << "\n  " << error.what()
                      << '\n';
        } catch (...) {
            ++failed;
            std::cerr << "[FAIL] " << test.name << "\n  unknown exception\n";
        }
    }
    std::cout << "Ran " << cppseed::test::registry().size() << " tests; "
              << failed << " failed.\n";
    return failed == 0 ? 0 : 1;
}
