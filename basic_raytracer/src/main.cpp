#include <embree4/rtcore.h>
#include <glad/glad.h>

#include "common.hpp"
#include "GLDrawer.hpp"
#include "objects.hpp"
#include "logger.hpp"
#include "scene.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 480

void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
    CandlelightRTC::LogError(std::to_string(error) + ": " + std::string(str));
}

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
    CandlelightRTC::GLDrawer *drawer = new CandlelightRTC::GLDrawer();

    drawer->Setup(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Setup Scene

    CandlelightRTC::LogInfo("Setting up scene...");

    RTCDevice device = rtcNewDevice("verbose=3");
    rtcSetDeviceErrorFunction(device, errorFunction, NULL);

    CandlelightRTC::Scene scene;

    CandlelightRTC::Camera camera;
    camera.getAspectRatio() = 1;
    camera.getMaxRayDistance() = std::numeric_limits<float>::infinity();
    camera.getNearPlaneDistance() = 1;
    camera.getTransform() = CandlelightRTC::transform_t(
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1),
        glm::quat(glm::vec3(0, 0, 0))
    );

    CandlelightRTC::LogInfo("Creating scene object...");

    scene.Setup(device, drawer, camera);

    CandlelightRTC::LogInfo("Begin drawing...");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //scene.DrawScene(WINDOW_WIDTH, WINDOW_HEIGHT);
        drawer->DrawCanvas();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    CandlelightRTC::LogInfo("Closing...");

    drawer->Release();
    glfwTerminate();
    return 0;
}