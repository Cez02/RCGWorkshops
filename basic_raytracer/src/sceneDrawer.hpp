#pragma once

#include "scene.hpp"
#include "GLDrawer.hpp"

#include "common.hpp"

namespace CandlelightRTC {

    class SceneDrawer {
    private:
        int m_MaxDepth = 3;
        ScenePtr m_CurrentScene;
        GLDrawer *m_Drawer;

        colorrgba_v_t FollowRay(glm::vec3 origin, glm::vec3 direction, int depth);
        void CalculateSample(int i, int j, int screenWidth, int screenHeight, glm::vec3 origin, colorrgba_v_t *res);

    public:
        void Setup(GLDrawer *drawer);

        void DrawScene(ScenePtr scene, int screenWidth, int screenHeight);
    };

}