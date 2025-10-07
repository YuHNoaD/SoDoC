cmake_minimum_required(VERSION 3.10)
project(SoDoCay)

set(CMAKE_CXX_STANDARD 11)

# Tim thu vien wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)

# Include thu muc cua wxWidgets
include(${wxWidgets_USE_FILE})

# Them thu muc chua header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Tao danh sach cac source files
set(SOURCES
    main.cpp
    Tree.cpp
    UI.cpp
)

# Tao executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link voi thu vien wxWidgets
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})