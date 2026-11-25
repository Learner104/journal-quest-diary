cmake_minimum_required(VERSION 3.16)
project(JournalQuest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/WritingWorkspace.cpp
    src/DetailsSidebar.cpp
    src/HomeScreen.cpp
    src/GardenWidget.cpp
    src/EntryListView.cpp
    src/SettingsView.cpp
    src/UtilitiesView.cpp
    src/DatabaseManager.cpp
    src/DocxHandler.cpp
    src/ThemeManager.cpp
)

set(HEADERS
    src/Models.h
    src/MainWindow.h
    src/WritingWorkspace.h
    src/DetailsSidebar.h
    src/HomeScreen.h
    src/GardenWidget.h
    src/EntryListView.h
    src/SettingsView.h
    src/UtilitiesView.h
    src/DatabaseManager.h
    src/DocxHandler.h
    src/ThemeManager.h
)

add_executable(journal-quest ${SOURCES} ${HEADERS})

target_link_libraries(journal-quest PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)

# Test runner executable
add_executable(test-core
    tests/test_core.cpp
    src/DatabaseManager.cpp
    src/DocxHandler.cpp
)

target_include_directories(test-core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(test-core PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
)


