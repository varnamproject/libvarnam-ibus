
cmake_minimum_required (VERSION 2.8.8)

include_directories(${COMMON_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS} ${VARNAM_INCLUDE_DIRS})

# Append the source files here
list (APPEND varnam_engine_sources
  engine.c
  main.c
)

set(engine_executable_name ibus-engine-varnam)
add_executable(${engine_executable_name} ${varnam_engine_sources} $<TARGET_OBJECTS:common>)
target_link_libraries(${engine_executable_name} ${VARNAM_LIBRARIES} ${IBUS_LIBRARIES})

