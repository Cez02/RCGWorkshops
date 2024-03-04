#include <embree3/rtcore.h>
#include <glad/glad.h>

#include "common.hpp"
#include "GLDrawer.hpp"
#include "objects.hpp"
#include "logger.hpp"
#include "scene.hpp"
#include "sceneDrawer.hpp"

#include <GLFW/glfw3.h>
#include <cstring>
#include <iomanip>

#include <iostream>
#include <chrono>

int WINDOW_HEIGHT = 160;
int WINDOW_WIDTH = 160;

CandlelightRTC::GLDrawer *drawer;

bool PRODUCE_IMAGE = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
    CandlelightRTC::LogError(std::to_string(error) + ": " + std::string(str));
}

void print_usage(){
    std::cout << "basictraycer [OPTIONS]" << std::endl;

    std::vector<std::pair<std::string, std::string>> options {
        {"-i  --image", "Render a still frame to a png image render.png."},
        {"-wd --width", "Set window width, default: 160."},
        {"-ht --height", "Set window height, default: 160."},
        {"-h  --help", "Print the program usage."},
        {"-sn --sample-count", "Set the raytracer sample count per pixel."},
        {"-d  --depth", "Set how many ray bounces per ray are accounted for during rendering."}
    };

    for(auto x : options){
        std::cout <<  "\t" <<std::setw(25) << std::left << x.first;
        std::cout << x.second << std::endl;
    }

    exit(0);
}

int main(int argc, char **argv)
{
    srand(time(0));

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int sampleCount = 100;
    int rayDepth = 10;

    for(int i = 1; i<argc; i++){
        if(!strcmp(argv[i], "-i") || !strcmp(argv[i], "--image"))
            PRODUCE_IMAGE = true;
        else if(!strcmp(argv[i], "-wd") || !strcmp(argv[i], "--width"))
            WINDOW_WIDTH = std::stoi(argv[i+1]);
        else if(!strcmp(argv[i], "-ht") || !strcmp(argv[i], "--height"))
            WINDOW_HEIGHT = std::stoi(argv[i+1]);
        else if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
            print_usage();
        else if(!strcmp(argv[i], "-sn") || !strcmp(argv[i], "--sample-count"))
            sampleCount = std::stoi(argv[i+1]);
        else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--depth"))
            rayDepth = std::stoi(argv[i+1]);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */

    window = glfwCreateWindow(PRODUCE_IMAGE ? 100 : WINDOW_WIDTH, PRODUCE_IMAGE ? 100 : WINDOW_HEIGHT, "Basic raytracer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        CandlelightRTC::LogError("Failed to initialize GLFW window");
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /* Initialize GL */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CandlelightRTC::LogError("Failed to initialize GLAD");
        return -1;
    }

    
    CandlelightRTC::LogInfo("Initializing drawer");
    drawer = new CandlelightRTC::GLDrawer();

    drawer->Setup(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Setup Scene

    CandlelightRTC::LogInfo("Setting up scene...");

    RTCDevice device = rtcNewDevice(NULL);
    rtcSetDeviceErrorFunction(device, errorFunction, NULL);

    CandlelightRTC::ScenePtr scene = new CandlelightRTC::Scene();
    CandlelightRTC::SceneDrawer sceneDrawer;

    CandlelightRTC::Camera camera;
    camera.getAspectRatio() = WINDOW_WIDTH / WINDOW_HEIGHT;
    camera.getMaxRayDistance() = std::numeric_limits<float>::infinity();
    camera.getNearPlaneDistance() = 1;
    camera.getTransform() = CandlelightRTC::transform_t(
        glm::vec3(0, 1.5f, 0),
        glm::vec3(1, 1, 1),
        glm::quat(glm::vec3(glm::radians(15.0f), 0, 0))
    );

    CandlelightRTC::LogInfo("Creating scene object...");

    scene->Setup(device, drawer, camera);
    sceneDrawer.Setup(drawer);

    CandlelightRTC::LogInfo("Begin drawing...");

    
    auto timeLastFrame = std::chrono::high_resolution_clock::now();
    float deltaTime;

    float cameraSpeed = 0.1f;

    /* Loop until the user closes the window */
    while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        sceneDrawer.DrawScene(scene, WINDOW_WIDTH, WINDOW_HEIGHT, sampleCount, rayDepth);
        // scene.DrawScene(WINDOW_WIDTH, WINDOW_HEIGHT);

        if(PRODUCE_IMAGE){
            drawer->PrintBufferToImage();
            break;
        }
        drawer->DrawCanvas();

        glfwSwapBuffers(window);
        glfwPollEvents();


        // input

        if(glfwGetKey(window, GLFW_KEY_A))
            scene->getCamera().getTransform().Position += -scene->getCamera().getTransform().Right() * cameraSpeed * deltaTime;
        if(glfwGetKey(window, GLFW_KEY_D))
            scene->getCamera().getTransform().Position += scene->getCamera().getTransform().Right() * cameraSpeed * deltaTime;

        if(glfwGetKey(window, GLFW_KEY_W))
            scene->getCamera().getTransform().Position += -scene->getCamera().getTransform().Forward() * cameraSpeed * deltaTime;
        if(glfwGetKey(window, GLFW_KEY_S))
            scene->getCamera().getTransform().Position += scene->getCamera().getTransform().Forward() * cameraSpeed * deltaTime;

        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
            scene->getCamera().getTransform().Position += -scene->getCamera().getTransform().Up() * cameraSpeed * deltaTime;
        if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
            scene->getCamera().getTransform().Position += scene->getCamera().getTransform().Up() * cameraSpeed * deltaTime;

        deltaTime = ((float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeLastFrame).count() / 1000.0f);
        CandlelightRTC::LogInfo("FPS: " + std::to_string(1.0f / deltaTime));

        timeLastFrame = std::chrono::high_resolution_clock::now();
    }

    CandlelightRTC::LogInfo("Closing...");

    drawer->Release();
    glfwTerminate();

    delete scene;
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    if(PRODUCE_IMAGE)
        return;

    glViewport(0, 0, width, height);
    WINDOW_HEIGHT = height;
    WINDOW_WIDTH = width;

    drawer->Reset(width, height);
}