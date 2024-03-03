target_sources(basicraytracer
    PRIVATE
        main.cpp
        objects.cpp
        camera.cpp
        scene.cpp
        mesh.cpp
        GLDrawer.cpp
        logger.cpp
)

target_include_directories(basicraytracer
    PRIVATE
        .
)