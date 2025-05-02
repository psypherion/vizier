# engine/src/CMakeLists.txt
file(GLOB SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(vizier_core ${SRC_FILES})
target_include_directories(vizier_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
