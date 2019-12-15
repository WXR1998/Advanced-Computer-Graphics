CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(Proj CXX)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

ADD_SUBDIRECTORY(deps/vecmath)

SET(Proj_SOURCES
        src/image.cpp
        src/main.cpp
        src/mesh.cpp
        src/scene_parser.cpp)

SET(Proj_INCLUDES
        include/light.hpp
        include/kdtree.hpp
        include/camera.hpp
        include/group.hpp
        include/hit.hpp
        include/image.hpp
        include/material.hpp
        include/mesh.hpp
        include/object3d.hpp
        include/plane.hpp
        include/ray.hpp
        include/scene_parser.hpp
        include/sphere.hpp
        include/transform.hpp
        include/triangle.hpp
        include/render_pt.hpp )

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "-g -O3 -Wno-deprecated-declarations -w -fopenmp")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${PROJECT_NAME} ${Proj_SOURCES} ${Proj_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
