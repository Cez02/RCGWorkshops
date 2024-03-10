target_sources(basicraytracer
    PRIVATE
        main.cpp
        objects.cpp
        camera.cpp
        scene.cpp
        mesh.cpp
        GLDrawer.cpp
        logger.cpp
        sceneDrawer.cpp
        model.cpp
)

# target_compile_definitions(basicraytracer
#     PUBLIC
#         RTC_MAX_INSTANCE_LEVEL_COUNT=4
#         EMBREE_MAX_INSTANCE_LEVEL_COUNT=4
# )
# SET(EMBREE_MAX_INSTANCE_LEVEL_COUNT 4)

target_include_directories(basicraytracer
    PRIVATE
        .
)