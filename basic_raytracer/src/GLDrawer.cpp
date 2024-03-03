#include "GLDrawer.hpp"

#include <iostream>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "common.hpp"
#include "logger.hpp"

inline char const *VERTEX_SHADER = R"END(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoord = aTexCoord;
}
)END";

inline char const *FRAGMENT_SHADER = R"END(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Canvas;

void main()
{
    FragColor = vec4(texture(Canvas, TexCoord).rgb, 1.0);
} 
)END";

namespace CandlelightRTC{
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    void GLDrawer::Setup(GLuint canvasWidth, GLuint canvasHeight)
    {
        // SETUP SHADER PROGRAM
        unsigned int vertex, fragment;

        CandlelightRTC::LogInfo("Compiling vertex shader");

        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &VERTEX_SHADER, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        CandlelightRTC::LogInfo("Compiling fragment shader");

        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &FRAGMENT_SHADER, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        CandlelightRTC::LogInfo("Compiling shader program");

        // shader Program
        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, vertex);
        glAttachShader(m_ShaderProgram, fragment);
        glLinkProgram(m_ShaderProgram);
        checkCompileErrors(m_ShaderProgram, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        CandlelightRTC::LogInfo("Setting up texture geometry");

        float vertices[] = {
            // positions          // texture coords
            -1.0f,  1.0f, 0.0f,    0.0f, 1.0f, // top left
             1.0f,  1.0f, 0.0f,    1.0f, 1.0f, // top right
            -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, // bottom left

             1.0f,  1.0f, 0.0f,    1.0f, 1.0f, // top right
             1.0f, -1.0f, 0.0f,    1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, // bottom left
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        CandlelightRTC::LogInfo("Setting up texture canvas");

        // setup canvas
        glGenTextures(1, &m_CanvasTexture);
        glBindTexture(GL_TEXTURE_2D, m_CanvasTexture);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        GLubyte *data = new GLubyte[canvasHeight * canvasWidth * 4];

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvasWidth, canvasHeight, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, NULL);

        glUniform1i(glGetUniformLocation(m_ShaderProgram, "Canvas"), 0);   

        m_CanvasHeight = canvasHeight;
        m_CanvasWidth = canvasWidth;

        m_CanvasPreBuffer.resize(canvasHeight * canvasWidth);

    }

    void GLDrawer::Reset(GLuint canvasWidth, GLuint canvasHeight)
    {
        m_CanvasHeight = canvasHeight;
        m_CanvasWidth = canvasWidth;

        m_CanvasPreBuffer.resize(canvasHeight * canvasWidth);
    }

    void GLDrawer::Release()
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
    }

    void GLDrawer::SetCanvasPixel(GLuint x, GLuint y, glm::vec3 color)
    {
        colorrgba_t colorConv {
            (GLubyte)(color.r * 255),
            (GLubyte)(color.g * 255),
            (GLubyte)(color.b * 255),
            255
        };

        m_CanvasPreBuffer[y * m_CanvasWidth + x] = colorConv;
    }

    void GLDrawer::PrintBufferToImage()
    {
#define CHANNEL_NUM 3

        uint8_t* pixels = new uint8_t[m_CanvasHeight * m_CanvasWidth * CHANNEL_NUM];

        int index = 0;
        for (int j = m_CanvasHeight - 1; j >= 0; --j)
        {
            for (int i = 0; i < m_CanvasWidth; ++i)
            {
                pixels[index++] = m_CanvasPreBuffer[j * m_CanvasWidth + i].at(0);
                pixels[index++] = m_CanvasPreBuffer[j * m_CanvasWidth + i].at(1);
                pixels[index++] = m_CanvasPreBuffer[j * m_CanvasWidth + i].at(2);
            }
        }

        stbi_write_png("render.png", m_CanvasWidth, m_CanvasHeight, CHANNEL_NUM, pixels, m_CanvasWidth * CHANNEL_NUM);
    }

    void GLDrawer::DrawCanvas()
    {
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            m_CanvasWidth,
            m_CanvasHeight,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            m_CanvasPreBuffer.data()
        );    


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_CanvasTexture);

        glUseProgram(m_ShaderProgram);

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}