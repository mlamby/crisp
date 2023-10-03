add_library(crisp_lib
  common.h
  value.h value.c
  memory.h memory.c
  hash_table.h hash_table.c
  scanner.c scanner.h
  parser.c parser.h
  symbols.c symbols.h
  interpreter.c interpreter.h)

target_include_directories(crisp_lib
  PUBLIC .)

target_link_libraries(crisp_lib 
  PUBLIC
    project_options 
    project_warnings)

add_executable(crisp
    main.c)

target_link_libraries(crisp 
    PUBLIC
      crisp_lib
      project_options
      project_warnings
  )