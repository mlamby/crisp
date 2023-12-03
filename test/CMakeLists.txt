add_library(simple_test
  simple_test.h
  simple_test.c)

target_link_libraries(simple_test
  PUBLIC
    crisp_lib
    project_options 
    project_warnings)

add_executable(scanner_test scanner_test.c)
add_executable(parse_test parse_test.c)
add_executable(eval_test eval_test.c)
add_executable(value_test value_test.c)
add_executable(hash_table_test hash_table_test.c)
add_executable(environment_test environment_test.c)
add_executable(evaluator_test evaluator_test.c)

target_link_libraries(scanner_test PRIVATE simple_test)
target_link_libraries(parse_test PRIVATE simple_test)
target_link_libraries(eval_test PRIVATE simple_test)
target_link_libraries(value_test PRIVATE simple_test)
target_link_libraries(hash_table_test PRIVATE simple_test)
target_link_libraries(environment_test PRIVATE simple_test)
target_link_libraries(evaluator_test PRIVATE simple_test)

add_test(scanner_test scanner_test)
add_test(parse_test parse_test)
add_test(eval_test eval_test)
add_test(value_test value_test)
add_test(hash_table_test hash_table_test)
add_test(environment_test environment_test)
add_test(evaluator_test evaluator_test)