add_executable(
  cppseed_unit_tests
  fake_file_system.cpp
  test_application.cpp
  test_cli.cpp
  test_diagnostic.cpp
  test_file_system.cpp
  test_main.cpp
  test_project_generator.cpp
  test_project_name.cpp
  test_template_renderer.cpp
)
target_include_directories(cppseed_unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cppseed_unit_tests PRIVATE cppseed_core)
target_compile_features(cppseed_unit_tests PRIVATE cxx_std_20)
cppseed_set_warnings(cppseed_unit_tests)

add_test(NAME cppseed.unit COMMAND cppseed_unit_tests)

add_test(
  NAME cppseed.e2e
  COMMAND
    ${CMAKE_COMMAND}
    -DCPPSEED_EXECUTABLE=$<TARGET_FILE:cppseed>
    -DCPPSEED_E2E_ROOT=${CMAKE_CURRENT_BINARY_DIR}/e2e
    -P ${CMAKE_CURRENT_SOURCE_DIR}/e2e.cmake
)
