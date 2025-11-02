cmake_minimum_required(VERSION 3.10)
project(reminderScheduler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -O1")

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(reminderScheduler 
    src/ReminderScheduler.cpp 
    src/Users.cpp 
    src/users_c_api.cpp
)
target_link_libraries(reminderScheduler CURL::libcurl nlohmann_json::nlohmann_json)
target_include_directories(reminderScheduler PRIVATE include)

add_library(users SHARED
    src/Users.cpp
    src/users_c_api.cpp
)
target_link_libraries(users
    CURL::libcurl
    nlohmann_json::nlohmann_json
)
target_include_directories(users PUBLIC include)