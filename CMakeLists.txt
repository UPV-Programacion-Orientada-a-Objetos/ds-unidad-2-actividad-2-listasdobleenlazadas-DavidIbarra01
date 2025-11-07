cmake_minimum_required(VERSION 3.10)
project(DecodificadorPRT7
    VERSION 1.0.0
    DESCRIPTION "Sistema Decodificador de Protocolo PRT-7"
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de compilación" FORCE)
endif()

message(STATUS "Tipo de compilación: ${CMAKE_BUILD_TYPE}")
message(STATUS "Sistema operativo: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compilador: ${CMAKE_CXX_COMPILER_ID}")
set(SOURCES
    decodificador_prt7.cpp
)
add_executable(decodificador_prt7 ${SOURCES})
if(WIN32)
    message(STATUS "Configurando para Windows")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Configurando para Linux")
elseif(APPLE)
    message(STATUS "Configurando para macOS")
endif()
if(MSVC)
    target_compile_options(decodificador_prt7 PRIVATE
        /W4      
        /WX   
        /permissive-
    )
else()
    target_compile_options(decodificador_prt7 PRIVATE
        -Wall   
        -Wextra     
        -Wpedantic  
        -Werror    
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(decodificador_prt7 PRIVATE -O3)
    else()
        target_compile_options(decodificador_prt7 PRIVATE -g)
    endif()
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
install(TARGETS decodificador_prt7
    RUNTIME DESTINATION bin
    COMPONENT Applications
)
if(EXISTS "${CMAKE_SOURCE_DIR}/doc/Doxyfile")
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc)
        add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generando documentación Doxygen..."
        )
        install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html/
            DESTINATION doc
            COMPONENT Documentation
            OPTIONAL
        )
    endif()
endif()
install(FILES README.md
    DESTINATION .
    COMPONENT Documentation
)
if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
    install(FILES LICENSE
        DESTINATION .
        COMPONENT Runtime
    )
endif()
set(CPACK_PACKAGE_NAME "DecodificadorPRT7")
set(CPACK_PACKAGE_VENDOR "Sistema de Ciberseguridad Industrial")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Decodificador para protocolo PRT-7")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
endif()
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

include(CPack)

# Mensaje de finalización de configuración
message(STATUS "")
message(STATUS "========================================")
message(STATUS "  CONFIGURACIÓN DEL PROYECTO")
message(STATUS "========================================")
message(STATUS "Proyecto: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Ejecutable: prt7_decoder")
message(STATUS "Directorio de salida: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "")
message(STATUS "Para compilar:")
message(STATUS "  cmake --build .")
message(STATUS "")
message(STATUS "Para generar documentación (si Doxygen está instalado):")
message(STATUS "  cmake --build . --target doc")
message(STATUS "")
message(STATUS "Para empaquetar:")
message(STATUS "  cpack -C ${CMAKE_BUILD_TYPE}")
message(STATUS "========================================")
message(STATUS "")