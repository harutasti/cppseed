if(NOT DEFINED CPPSEED_EXECUTABLE OR NOT DEFINED CPPSEED_E2E_ROOT)
  message(FATAL_ERROR "CPPSEED_EXECUTABLE and CPPSEED_E2E_ROOT are required")
endif()

file(REMOVE_RECURSE "${CPPSEED_E2E_ROOT}")
file(MAKE_DIRECTORY "${CPPSEED_E2E_ROOT}")

foreach(standard IN ITEMS 17 20 23)
  set(project_name "e2e-${standard}")
  set(source_dir "${CPPSEED_E2E_ROOT}/${project_name}")
  set(build_dir "${source_dir}/build")

  execute_process(
    COMMAND "${CPPSEED_EXECUTABLE}" new "${project_name}" --std "${standard}"
    WORKING_DIRECTORY "${CPPSEED_E2E_ROOT}"
    RESULT_VARIABLE generate_result
    OUTPUT_VARIABLE generate_output
    ERROR_VARIABLE generate_error
  )
  if(NOT generate_result EQUAL 0)
    message(FATAL_ERROR "generation failed: ${generate_error}")
  endif()

  execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${source_dir}" -B "${build_dir}"
    RESULT_VARIABLE configure_result
  )
  if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR "generated C++${standard} project did not configure")
  endif()

  execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${build_dir}" --config Debug
    RESULT_VARIABLE build_result
  )
  if(NOT build_result EQUAL 0)
    message(FATAL_ERROR "generated C++${standard} project did not build")
  endif()

  execute_process(
    COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${build_dir}" -C Debug
            --output-on-failure
    RESULT_VARIABLE test_result
  )
  if(NOT test_result EQUAL 0)
    message(FATAL_ERROR "generated C++${standard} project tests failed")
  endif()

  set(executable_suffix "")
  if(WIN32)
    set(executable_suffix ".exe")
  endif()
  set(executable "${build_dir}/${project_name}${executable_suffix}")
  if(NOT EXISTS "${executable}")
    set(executable "${build_dir}/Debug/${project_name}${executable_suffix}")
  endif()
  if(NOT EXISTS "${executable}")
    message(FATAL_ERROR "generated executable was not found")
  endif()

  execute_process(
    COMMAND "${executable}"
    RESULT_VARIABLE run_result
    OUTPUT_VARIABLE run_output
    ERROR_VARIABLE run_error
  )
  if(NOT run_result EQUAL 0 OR NOT run_output STREQUAL "Hello from ${project_name}!\n")
    message(FATAL_ERROR
      "generated executable failed: stdout=${run_output} stderr=${run_error}")
  endif()
endforeach()

file(REMOVE_RECURSE "${CPPSEED_E2E_ROOT}")
