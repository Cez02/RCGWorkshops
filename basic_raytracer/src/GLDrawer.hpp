#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

namespace CandlelightRTC {

    class GLDrawer {
    private:
        GLuint m_CanvasTexture;
        GLuint m_ShaderProgram, m_VAO, m_VBO;

    public:
        void Setup(GLuint canvasWidth, GLuint canvasHeight);
        void Release();

        void SetCanvasPixel(GLuint x, GLuint y, glm::vec3 color);

        void DrawCanvas();
    };

}