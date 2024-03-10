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

        static colorrgba_v_t FollowRay(glm::vec3 origin, glm::vec3 direction, ScenePtr scene, int depth);
        static void CalculateSample(int row, ScenePtr scene, int screenWidth, int screenHeight, glm::vec3 origin, colorrgba_v_t *colorRow);

    public:
        void Setup(GLDrawer *drawer);

        void DrawScene(ScenePtr scene, int screenWidth, int screenHeight, int sampleCount = 100, int depth = 10, int maxJobsCount = 10);
    };

}