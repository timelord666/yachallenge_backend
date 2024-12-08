cmake_minimum_required(VERSION 3.12)
project(ya_challenge CXX)


# Adding userver dependency
find_package(userver COMPONENTS core postgresql QUIET)
if(NOT userver_FOUND)  # Fallback to subdirectory usage
    # Enable userver libraries that are needed in this project
    set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)

    # Compatibility mode: some systems don't support these features
    set(USERVER_FEATURE_CRYPTOPP_BLAKE2 OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_GRPC_CHANNELZ OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_REDIS_HI_MALLOC ON CACHE BOOL "" FORCE)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver)
        message(STATUS "Using userver framework from third_party/userver")
        add_subdirectory(third_party/userver)
    else()
        message(FATAL_ERROR "Either install the userver or provide a path to it")
    endif()
endif()

userver_setup_environment()


# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    src/handlers/v1/users/create/view.hpp
    src/handlers/v1/users/create/view.cpp
    
    src/models/category.cpp
    src/models/category.hpp
    
    src/models/user.hpp
    src/models/user.cpp
    
    src/models/userStats.hpp
    src/models/userStats.cpp
    
    src/models/completedChallenges.hpp
    src/models/completedChallenges.cpp
    
    src/models/userCategories.hpp
    src/models/userCategories.cpp
    
    src/handlers/v1/admin/add-challenge/view.cpp
    src/handlers/v1/admin/add-challenge/view.hpp
    
    src/handlers/v1/admin/add-category/view.cpp
    src/handlers/v1/admin/add-category/view.hpp
    
    src/handlers/v1/users/view.cpp
    src/handlers/v1/users/view.hpp
    
    src/handlers/v1/users/rankings/view.cpp
    src/handlers/v1/users/rankings/view.hpp
    
    src/handlers/v1/users/categories/view.cpp
    src/handlers/v1/users/categories/view.hpp
    
    src/handlers/v1/users/challenges/view.cpp
    src/handlers/v1/users/challenges/view.hpp

    src/handlers/v1/get-categories/view.hpp
    src/handlers/v1/get-categories/view.cpp
    src/handlers/v1/complate-challenge/view.cpp
    src/handlers/v1/complate-challenge/view.hpp

    src/handlers/v1/users/update/view.cpp
    src/handlers/v1/users/update/view.hpp

    src/models/challenge.hpp
    src/models/challenge.cpp

    src/handlers/v1/get-challenges/view.hpp
    src/handlers/v1/get-challenges/view.cpp
    
    src/handlers/v1/login/login.cpp
    src/handlers/v1/login/login.cpp


)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Unit Tests


# Benchmarks



# Functional Tests

userver_testsuite_add_simple()


# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
