cmake_minimum_required(VERSION 3.16)

project(MyChat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_add_executable(appMyChat
    main.cpp
)

set_source_files_properties(
    Global/AppState.qml
    PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appMyChat
    URI MyChat
    VERSION 1.0
    QML_FILES main.qml
    QML_FILES Login/LoginPage.qml
    RESOURCES Icon/LogConfig.png
    RESOURCES Icon/LogClose.png
    RESOURCES Icon/userImg_test.jpg
    RESOURCES Resrc.qrc
    QML_FILES Login/VerifyPage.qml
    SOURCES CNetWork/netbridge.h CNetWork/netbridge.cpp
    QML_FILES Global/AppState.qml
    QML_FILES Main/MainPage.qml
    QML_FILES Main/MarginPage.qml
    QML_FILES Main/LeftPage.qml
    QML_FILES Main/RightPage.qml
    RESOURCES Icon/Chat.png
    RESOURCES Icon/Contact.png
    RESOURCES Icon/Collect.png
    RESOURCES Icon/Top.png
    RESOURCES Icon/Minimize.png
    RESOURCES Icon/Maximize.png
    RESOURCES Icon/Config.png
    RESOURCES Icon/Search.png
    RESOURCES Icon/Add.png
    RESOURCES Icon/MyChat.png
    RESOURCES Icon/ChatInfo.png Icon/ChatVideo.png
    RESOURCES Icon/ChatEmoji.png Icon/ChatFile.png
    RESOURCES Icon/ChatMore.png
    RESOURCES Icon/ChatRecord.png Icon/ChatScreenShot.png
    RESOURCES Icon/ChatVoice.png
    RESOURCES Icon/ChatCollect.png
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appMyChat PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appMyChat
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appMyChat
    PRIVATE Qt6::Quick)
target_link_libraries(appMyChat PRIVATE Qt6::Core)
target_link_libraries(appMyChat PRIVATE Qt6::Core)

include(GNUInstallDirs)
install(TARGETS appMyChat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
