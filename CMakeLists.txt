cmake_minimum_required(VERSION 3.10)
project(RecipeManagementSystem VERSION 1.0)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find threads (Required by httplib)
find_package(Threads REQUIRED)

# Include the backend/include directory so we can use #include "httplib.h"
include_directories(backend/include)

# Define the executable
add_executable(recipe_server backend/main.cpp)

# Link threads to the executable
target_link_libraries(recipe_server PRIVATE Threads::Threads)

# Optional: Print success message
message(STATUS "Project configured successfully. Ready to build.")
vscode-remote://codespaces%2Bturbo-eureka-7w459q4qj7hqxp/workspaces/tmp/ARCHITECTURE.md