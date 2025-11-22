cmake_minimum_required(VERSION 3.20)
project(Castle_Defender LANGUAGES CXX)

# Use modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find FLTK installed via vcpkg
# Make sure CLion is configured with:
# -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
# -DVCPKG_TARGET_TRIPLET=x64-windows   (or x64-mingw-dynamic if using MinGW)
find_package(FLTK CONFIG REQUIRED)

# Add your main source file (and others if you have them)
add_executable(Castle_Defender
        main.cpp
        "Grid.cpp"
        "Grid.h"
        "Block.cpp"
        "Block.h"
        "GameWindow.cpp"
        "GameWindow.h"
        Enemy.cpp
        Enemy.h
        Tower.cpp
        Tower.h
)

# Link FLTK libraries
target_link_libraries(Castle_Defender
        PRIVATE
        fltk
        fltk_images
        fltk_forms
)

# Include FLTK headers
target_include_directories(Castle_Defender PRIVATE ${FLTK_INCLUDE_DIRS})

# Optional: hide console window when running as GUI app on Windows
if (WIN32)
    set_property(TARGET Castle_Defender PROPERTY WIN32_EXECUTABLE TRUE)
endif()

