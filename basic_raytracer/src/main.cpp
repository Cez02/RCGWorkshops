#include <embree4/rtcore.h>
#include <glad/glad.h>

#include "GLDrawer.hpp"
#include "objects.hpp"

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
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GL */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "[LOG] - Setup drawer" << std::endl;
    CandlelightRTC::GLDrawer drawer;

    drawer.Setup(WINDOW_WIDTH, WINDOW_HEIGHT);


    std::cout << "[LOG] - Setup up debug square" << std::endl;

    for(int x = 40; x<= 80; x++){
        for(int y = 40; y<=80; y++){
            drawer.SetCanvasPixel(x, y, glm::vec3(1, 0, 0));
        }
    }

    std::cout << "[LOG] - Start drawing..." << std::endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawer.DrawCanvas();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}