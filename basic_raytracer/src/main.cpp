#include <embree4/rtcore.h>
#include <glad/glad.h>

#include "common.hpp"
#include "GLDrawer.hpp"
#include "objects.hpp"
#include "logger.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 480

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Basic raytracer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        CandlelightRTC::LogError("Failed to initialize GLFW window");
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GL */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CandlelightRTC::LogError("Failed to initialize GLAD");
        return -1;
    }

    
    CandlelightRTC::LogInfo("Initializing drawer");
    CandlelightRTC::GLDrawer drawer;

    drawer.Setup(WINDOW_WIDTH, WINDOW_HEIGHT);


    // Setup embree
    RTCDevice device = rtcNewDevice(NULL);
    RTCScene scene   = rtcNewScene(device);
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

    float* vb = (float*) rtcSetNewGeometryBuffer(geom,
        RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), 3);
    vb[0] = 0.f; vb[1] = 0.f; vb[2] = 0.f; // 1st vertex
    vb[3] = 1.f; vb[4] = 0.f; vb[5] = 0.f; // 2nd vertex
    vb[6] = 0.f; vb[7] = 1.f; vb[8] = 0.f; // 3rd vertex

    unsigned* ib = (unsigned*) rtcSetNewGeometryBuffer(geom,
        RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(unsigned), 1);
    ib[0] = 0; ib[1] = 1; ib[2] = 2;

    rtcCommitGeometry(geom);
    rtcAttachGeometry(scene, geom);
    rtcReleaseGeometry(geom);
    rtcCommitScene(scene);

    CandlelightRTC::LogInfo("Begin drawing...");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawer.DrawCanvas();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    CandlelightRTC::LogInfo("Closing...");

    drawer.Release();
    glfwTerminate();
    return 0;
}