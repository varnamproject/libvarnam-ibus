
cmake_minimum_required (VERSION 2.8.8)

include_directories(${COMMON_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS} ${VARNAM_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})

# Append the source files here
list (APPEND varnam_engine_sources
  engine.c
	varnamd_proxy.c
  main.c
)

# Format of the list of component languages should be
# iBusLangCode,varnamLangCode,engineName,description

foreach(varnam_languages ${VARNAM_LANGUAGES})
    if(varnam_languages STREQUAL "as")
        list (APPEND component_xml_files "as,as,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "as")
    if(varnam_languages STREQUAL "bn")
        list (APPEND component_xml_files "bn,bn,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "bn")
    if(varnam_languages STREQUAL "gu")
        list (APPEND component_xml_files "gu,gu,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "gu")
    if(varnam_languages STREQUAL "hi")
        list (APPEND component_xml_files "hi,hi,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "hi")
    if(varnam_languages STREQUAL "kn")
        list (APPEND component_xml_files "kn,kn,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "kn")
    if(varnam_languages STREQUAL "ml")
        list (APPEND component_xml_files "ml,ml,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "ml")
    if(varnam_languages STREQUAL "ml-inscript")
        list (APPEND component_xml_files "ml,ml-inscript,Varnam,Varnam input method for inscript")
    endif(varnam_languages STREQUAL "ml-inscript")
    if(varnam_languages STREQUAL "mr")
        list (APPEND component_xml_files "mr,mr,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "mr")
    if(varnam_languages STREQUAL "ne")
        list (APPEND component_xml_files "ne,ne,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "ne")
    if(varnam_languages STREQUAL "or")
        list (APPEND component_xml_files "or,or,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "or")
    if(varnam_languages STREQUAL "pa")
        list (APPEND component_xml_files "pa,pa,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "pa")
    if(varnam_languages STREQUAL "sa")
        list (APPEND component_xml_files "sa,sa,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "sa")
    if(varnam_languages STREQUAL "ta")
        list (APPEND component_xml_files "ta,ta,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "ta")
    if(varnam_languages STREQUAL "te")
        list (APPEND component_xml_files "te,te,Varnam,Varnam input method")
    endif(varnam_languages STREQUAL "te")
endforeach(varnam_languages)

foreach(componentXmlFile ${component_xml_files})
    # replaces , with ;. CMake uses ; for list
    STRING(REGEX REPLACE "," ";" componentXmlFile ${componentXmlFile})
    list(GET componentXmlFile 0 ibusLangCode)
    list(GET componentXmlFile 1 varnamLangCode)
    list(GET componentXmlFile 2 varnamEngineName)
    list(GET componentXmlFile 3 varnamEngineDescription)
    configure_file(varnam.xml.in varnam.${varnamLangCode}.xml)
    install(FILES varnam.${varnamLangCode}.xml DESTINATION /usr/share/ibus/component)
endforeach(componentXmlFile)

set(engine_executable_name ibus-engine-varnam)
add_executable(${engine_executable_name} ${varnam_engine_sources} $<TARGET_OBJECTS:common>)
target_link_libraries(${engine_executable_name} ${VARNAM_LIBRARIES} ${IBUS_LIBRARIES} ${CURL_LIBRARIES} pthread dl)

install(TARGETS ${engine_executable_name} RUNTIME DESTINATION bin)

