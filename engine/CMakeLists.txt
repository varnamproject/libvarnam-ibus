
cmake_minimum_required (VERSION 2.8.8)

include_directories(${COMMON_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS} ${VARNAM_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})

# Append the source files here
list (APPEND varnam_engine_sources
  engine.c
	varnamd_proxy.c
  main.c
)

# Format of the below list should be
# iBusLangCode,varnamLangCode,engineName,description
list (APPEND component_xml_files
  "ml,ml,Varnam,Varnam input method"
  "ml,ml-inscript,Varnam Inscript,Varnam input method for inscript"
  "hi,hi,Varnam,Varnam input method"
)

foreach(componentXmlFile ${component_xml_files})
    # replaces , with ;. CMake uses ; for list
    STRING(REGEX REPLACE "," ";" componentXmlFile ${componentXmlFile})
    list(GET componentXmlFile 0 ibusLangCode)
    list(GET componentXmlFile 1 varnamLangCode)
    list(GET componentXmlFile 2 varnamEngineName)
    list(GET componentXmlFile 3 varnamEngineDescription)
    configure_file(varnam.xml.in varnam.${varnamLangCode}.xml)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/varnam.${varnamLangCode}.xml DESTINATION /usr/share/ibus/component) #In case the user is building in a different directory from the source directory.
endforeach(componentXmlFile)

set(engine_executable_name ibus-engine-varnam)
add_executable(${engine_executable_name} ${varnam_engine_sources} $<TARGET_OBJECTS:common>)
target_link_libraries(${engine_executable_name} ${VARNAM_LIBRARIES} ${IBUS_LIBRARIES} ${CURL_LIBRARIES} pthread dl)

install(TARGETS ${engine_executable_name} RUNTIME DESTINATION bin)

