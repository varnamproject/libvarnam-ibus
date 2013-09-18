cmake_minimum_required (VERSION 2.8.8)

project(varnam-sync)
message ("Generating project ${PROJECT_NAME}")

include_directories(${VARNAM_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})

# Append the source files here
list (APPEND varnam_sync_sources
  sync.c
)

set(sync_executable_name varnam-sync)
add_executable(${sync_executable_name} ${varnam_sync_sources})
target_link_libraries(${sync_executable_name} ${VARNAM_LIBRARIES} ${GLIB_LIBRARIES})

