cmake_minimum_required (VERSION 2.8.8)

project(ibus-engine-varnam-prefs)
message ("Generating project ${PROJECT_NAME}")

include_directories(${COMMON_INCLUDE_DIRS} ${VARNAM_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GTK+_INCLUDE_DIRS})

# Append the source files here
list (APPEND varnam_preferences_sources
  prefs.c
)

add_executable(${preferences_executable_name} ${varnam_preferences_sources} $<TARGET_OBJECTS:common>)
target_link_libraries(${preferences_executable_name} ${VARNAM_LIBRARIES} ${GLIB_LIBRARIES} ${GTK+_LIBRARIES} dl pthread)
install(TARGETS ${preferences_executable_name} RUNTIME DESTINATION bin)

install(FILES prefs.glade DESTINATION ${CMAKE_INSTALL_PREFIX}/share/varnam/ibus)
