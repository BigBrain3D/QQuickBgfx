cmake_minimum_required(VERSION 3.14)
project(bgfx_qquickitem LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "bgfx_qquickitem")

find_package(bgfx REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS    Core
                                        Gui
                                        Qml
                                        Quick)

set(SOURCES src/qquickbgfxitem/qquickbgfxitem.h
            src/qquickbgfxitem/qquickbgfxitem.cpp
            src/qbgfx.h
            src/qbgfx.cpp
            src/qquick_bgfx.h
            src/qsgbgfxnode/qsgbgfxnode.h
            src/qsgbgfxnode/qsgbgfxnode.cpp)

if (APPLE)
    set(RENDERER_NAME "Metal")
    add_compile_definitions(QQ_ENABLE_METAL)
elseif(WIN32)
    set(RENDERER_NAME "OpenGL")
    add_compile_definitions(QQ_ENABLE_OPENGL)
elseif(UNIX)
    set(RENDERER_NAME "OpenGL")
    add_compile_definitions(QQ_ENABLE_OPENGL)
endif()

if(RENDERER_NAME STREQUAL "Metal")
    add_definitions(ENABLE_METAL)
endif()

if(RENDERER_NAME STREQUAL "Metal")
    set(SOURCES
        ${SOURCES}
        src/qquick_bgfx.mm)
else()
    set(SOURCES
        ${SOURCES}
        src/qquick_bgfx.cpp)
endif()

if (QQBGFX_BUILD_LIBRARY)
    qt_add_library(bgfx_qquickitem ${SOURCES})
else()
    set(SOURCES ${SOURCES}
                main.cpp)

    target_link_libraries(bgfx_qquickitem PRIVATE example-common)

    qt_add_executable(bgfx_qquickitem ${SOURCES})
endif()

if (RENDERER_NAME STREQUAL "Metal")
    target_link_libraries(bgfx_qquickitem PUBLIC "-framework AppKit")

    target_link_libraries(bgfx_qquickitem PUBLIC "-framework Metal")
endif()

target_link_libraries(bgfx_qquickitem PUBLIC        Qt6::Core
                                                    Qt6::Gui
                                                    Qt6::Qml
                                                    Qt6::Quick
                                                    Qt6::QuickPrivate
                                                    bgfx::bgfx
                                                    bgfx::bimg
                                                    bgfx::bx)

target_compile_definitions(bgfx_qquickitem PRIVATE BX_CONFIG_DEBUG)

get_target_property(BGFX_INCLUDE_PATH bgfx::bgfx INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(bgfx_qquickitem
                            PUBLIC
                            ${CMAKE_CURRENT_LIST_DIR}
                            ${CMAKE_CURRENT_LIST_DIR}/src
                            ${CMAKE_CURRENT_LIST_DIR}/src/qquickbgfxitem
                            ${BGFX_INCLUDE_PATH})

if (QQBGFX_BUILD_LIBRARY)
    # ...
    install(TARGETS bgfx_qquickitem RUNTIME DESTINATION bin
                                    LIBRARY DESTINATION lib
                                    ARCHIVE DESTINATION lib/myproject)
else()
    # Resources:
    set(bgfx_qquickitem_resource_files "main.qml")

    qt6_add_resources(bgfx_qquickitem "bgfx_qquickitem"
        PREFIX
            "/bgfx_qquickitem"
        FILES
            ${bgfx_qquickitem_resource_files})

    set_target_properties(bgfx_qquickitem PROPERTIES
        QT_QML_MODULE_VERSION 1.0
        QT_QML_MODULE_URI QQuickBgfx
    )

    install(TARGETS bgfx_qquickitem
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION  "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}")

    qt_add_qml_module(bgfx_qquickitem
                  URI QQuickBgfx
                  VERSION 1.0
                  QML_FILES main.qml)
endif()


