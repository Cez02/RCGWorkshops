#include <embree3/rtcore.h>
#include <glad/glad.h>

#include "common.hpp"
#include "GLDrawer.hpp"
#include "objects.hpp"
#include "logger.hpp"
#include "scene.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>

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

    RTCDevice device = rtcNewDevice(NULL);
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

    
    auto timeLastFrame = std::chrono::high_resolution_clock::now();
    float deltaTime;

    float cameraSpeed = 0.01f;

    /* Loop until the user closes the window */
    while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        scene.DrawScene(WINDOW_WIDTH, WINDOW_HEIGHT);
        drawer->DrawCanvas();

        glfwSwapBuffers(window);
        glfwPollEvents();


        // input

        if(glfwGetKey(window, GLFW_KEY_A))
            scene.getCamera().getTransform().Position += -scene.getCamera().getTransform().Right() * cameraSpeed * deltaTime;
        if(glfwGetKey(window, GLFW_KEY_D))
            scene.getCamera().getTransform().Position += scene.getCamera().getTransform().Right() * cameraSpeed * deltaTime;

        if(glfwGetKey(window, GLFW_KEY_W))
            scene.getCamera().getTransform().Position += -scene.getCamera().getTransform().Forward() * cameraSpeed * deltaTime;
        if(glfwGetKey(window, GLFW_KEY_S))
            scene.getCamera().getTransform().Position += scene.getCamera().getTransform().Forward() * cameraSpeed * deltaTime;

        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
            scene.getCamera().getTransform().Position += -scene.getCamera().getTransform().Up() * cameraSpeed * deltaTime;
        if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
            scene.getCamera().getTransform().Position += scene.getCamera().getTransform().Up() * cameraSpeed * deltaTime;

        deltaTime = ((float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeLastFrame).count() / 1000.0f);
        CandlelightRTC::LogInfo("FPS: " + std::to_string(1.0f / deltaTime));

        timeLastFrame = std::chrono::high_resolution_clock::now();
    }

    CandlelightRTC::LogInfo("Closing...");

    drawer->Release();
    glfwTerminate();
    return 0;
}