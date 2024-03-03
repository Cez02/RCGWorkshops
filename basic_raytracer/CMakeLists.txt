add_executable(basicraytracer)

add_subdirectory(src)

set_property(TARGET basicraytracer PROPERTY CXX_STANDARD 17)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

find_package(embree 4 REQUIRED)

target_compile_definitions(basicraytracer
    PUBLIC
        GLM_ENABLE_EXPERIMENTAL
)

target_link_libraries(basicraytracer
    PRIVATE
        glfw
        OpenGL::GL
        embree
        glm::glm
        glad
)
