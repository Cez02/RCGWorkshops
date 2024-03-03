#include "sceneDrawer.hpp"

#include <glm/glm.hpp>

#include "logger.hpp"

#include <thread>

namespace CandlelightRTC {

    colorrgba_v_t SceneDrawer::FollowRay(glm::vec3 origin, glm::vec3 direction, int depth)
    {
        if(depth == 0)
            return glm::vec3(0, 0, 0);

        colorrgba_v_t color;
        RTCRayHit rayhit; 
        RTCIntersectContext context;

        // RTCRayHit rayhit; 
        rayhit.ray.org_x  = origin.x; rayhit.ray.org_y = origin.y; rayhit.ray.org_z = origin.z;
        rayhit.ray.dir_x  = direction.x; rayhit.ray.dir_y = direction.y; rayhit.ray.dir_z =  direction.z;
        rayhit.ray.tnear  = 0.f;
        rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

        rtcInitIntersectContext(&context);

        rtcIntersect1(m_CurrentScene->getRTCScene(), &context, &rayhit);

        if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
            glm::vec3 n = glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);

            glm::vec3 hitPos = origin + rayhit.ray.tfar * direction;

            glm::vec3 newRandomRay = glm::vec3((float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f);
            newRandomRay = glm::normalize(newRandomRay);

            if(glm::dot(newRandomRay, n) > 0)
                return 0.8f * FollowRay(hitPos, newRandomRay, depth - 1);
        }
        else {
            float a = 0.5*(direction.y + 1.0);
            return (1.0f-a)*colorrgba_v_t(1.0, 1.0, 1.0) + a*colorrgba_v_t(0.5, 0.7, 1.0);
        }
    }

    void SceneDrawer::CalculateSample(int i, int j, int screenWidth, int screenHeight, glm::vec3 origin, colorrgba_v_t *res){
        glm::vec2 eps = glm::vec2((float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f) * 0.7f;

        // calculate point position on the near plane
        glm::vec3 pointPos = glm::vec3(
            -0.5f + ((float)i + eps.x)  / (float)(screenWidth - 1),
            (-0.5f + ((float)j + eps.y)  / (float)(screenHeight - 1)) / m_CurrentScene->getCamera().getAspectRatio() ,
            m_CurrentScene->getCamera().getNearPlaneDistance()
        );

        pointPos = m_CurrentScene->getCamera().getTransform().toMat4() * glm::vec4(pointPos.x, pointPos.y, pointPos.z, 1.0);


        glm::vec3 rayDir = (pointPos - origin);

        *res += FollowRay(origin, rayDir, 3);
    }

    void SceneDrawer::Setup(GLDrawer *drawer)
    {
        m_Drawer = drawer;
    }

    void SceneDrawer::DrawScene(ScenePtr scene, int screenWidth, int screenHeight)
    {
        m_CurrentScene = scene;

        scene->getCamera().getAspectRatio() = (float)screenWidth / (float)screenHeight;

        int sampleCount = 1;        

        glm::vec3 positionToRenderFrom = scene->getCamera().getTransform().Position;

        std::vector<std::thread> workers;


        glm::vec3 color[screenHeight][screenWidth];

        for(int i = 0; i<screenWidth; i++){
            for(int j = 0; j<screenHeight; j++){
                color[j][i] = glm::vec3(0);

                // Cast ray from pos to pixel at near plane
                for(int sample = 0; sample<sampleCount; sample++){
                    CalculateSample(i, j, screenWidth, screenHeight, positionToRenderFrom, &(color[j][i]));
                }
            }
        }



        for(int i = 0; i<screenWidth; i++){
            for(int j = 0; j<screenHeight; j++){
                color[j][i] = color[j][i] * (1 / (float)sampleCount);

                m_Drawer->SetCanvasPixel(i, j, color[j][i]);
            }
        }
    }
}