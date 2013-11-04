
include_directories(${IBUS_INCLUDE_DIRS} ${VARNAM_INCLUDE_DIRS})

# Append the source files here
list (APPEND common_sources
  engine-util.c
)

add_library (common OBJECT ${common_sources})
