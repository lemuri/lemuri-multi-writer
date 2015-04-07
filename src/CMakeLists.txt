set(CMAKE_AUTOMOC ON)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB QML_SRC qml/*.qml qml/*.js)

set(multiwriter_SRCS
    ${QML_SRC}
    devicesmodel.cpp
    writerthread.cpp
    multiwriter.cpp
    main.cpp
)

qt5_wrap_ui(multiwriter_SRCS
    multiwriter.ui
)

qt5_add_resources(multiwriter_SRCS
    application.qrc
    OPTIONS -compress 3 -threshold 3
)

add_executable(lemuri-multi-writer
    ${multiwriter_SRCS}
)

qt5_use_modules(lemuri-multi-writer Widgets Quick)
target_link_libraries(lemuri-multi-writer
    ${UDisks2Qt5_LIBRARIES}
)

install(TARGETS lemuri-multi-writer DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
