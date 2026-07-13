#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "cppseed/application.hpp"
#include "cppseed/file_system.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

int run_application(const std::vector<std::string>& owned_arguments) {
    std::vector<std::string_view> arguments;
    arguments.reserve(owned_arguments.size());
    for (const auto& argument : owned_arguments) {
        arguments.push_back(argument);
    }

    cppseed::RealFileSystem file_system;
    cppseed::ApplicationContext context{file_system,
                                        std::filesystem::current_path(),
                                        std::cout,
                                        std::cerr};
    return cppseed::run(arguments, context);
}

int guarded_run(const std::vector<std::string>& arguments) noexcept {
    try {
        return run_application(arguments);
    } catch (const std::exception&) {
        std::cerr << "error: unexpected internal failure\n";
        return static_cast<int>(cppseed::ExitCode::runtime_error);
    } catch (...) {
        std::cerr << "error: unexpected internal failure\n";
        return static_cast<int>(cppseed::ExitCode::runtime_error);
    }
}

#ifdef _WIN32
std::string to_utf8(const wchar_t* value) {
    const int required = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value,
                                             -1, nullptr, 0, nullptr, nullptr);
    if (required <= 0) {
        throw std::runtime_error("UTF-16 conversion failed");
    }
    std::string output(static_cast<std::size_t>(required), '\0');
    const int converted = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS, value, -1, output.data(), required,
        nullptr, nullptr);
    if (converted != required) {
        throw std::runtime_error("UTF-16 conversion failed");
    }
    output.pop_back();
    return output;
}
#endif

}  // namespace

#ifdef _WIN32
int wmain(const int argc, wchar_t* argv[]) {
    std::vector<std::string> arguments;
    arguments.reserve(static_cast<std::size_t>(argc > 0 ? argc - 1 : 0));
    try {
        for (int index = 1; index < argc; ++index) {
            arguments.push_back(to_utf8(argv[index]));
        }
    } catch (...) {
        std::cerr << "error: unexpected internal failure\n";
        return static_cast<int>(cppseed::ExitCode::runtime_error);
    }
    return guarded_run(arguments);
}
#else
int main(const int argc, char* argv[]) {
    std::vector<std::string> arguments;
    arguments.reserve(static_cast<std::size_t>(argc > 0 ? argc - 1 : 0));
    for (int index = 1; index < argc; ++index) {
        arguments.emplace_back(argv[index]);
    }
    return guarded_run(arguments);
}
#endif
