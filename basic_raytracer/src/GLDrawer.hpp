#pragma once

#include "common.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace CandlelightRTC {

    class GLDrawer {
    private:
        GLuint m_CanvasTexture;
        GLuint m_ShaderProgram, m_VAO, m_VBO;

        int m_CanvasWidth, m_CanvasHeight;

        std::vector<colorrgba_t> m_CanvasPreBuffer; 

    public:
        void Setup(GLuint canvasWidth, GLuint canvasHeight);
        void Reset(GLuint canvasWidth, GLuint canvasHeight);
        void Release();

        void SetCanvasPixel(GLuint x, GLuint y, glm::vec3 color);

        void PrintBufferToImage();

        void DrawCanvas();
    };

}