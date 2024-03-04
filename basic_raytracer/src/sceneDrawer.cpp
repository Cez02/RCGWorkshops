#include "sceneDrawer.hpp"

#include <glm/glm.hpp>

#include "logger.hpp"

#include <thread>
#include <mutex>
#include <map>

namespace CandlelightRTC {

    colorrgba_v_t SceneDrawer::FollowRay(glm::vec3 origin, glm::vec3 direction, ScenePtr scene, int depth)
    {
        if(depth == 0)
            return glm::vec3(0, 0, 0);

        colorrgba_v_t color;
        RTCRayHit rayhit; 
        RTCIntersectContext context;

        // RTCRayHit rayhit; 
        rayhit.ray.org_x  = origin.x; rayhit.ray.org_y = origin.y; rayhit.ray.org_z = origin.z;
        rayhit.ray.dir_x  = direction.x; rayhit.ray.dir_y = direction.y; rayhit.ray.dir_z =  direction.z;
        rayhit.ray.tnear  = 0.001f;
        rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

        rtcInitIntersectContext(&context);

        rtcIntersect1(scene->getRTCScene(), &context, &rayhit);

        if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
            glm::vec3 n = glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);

            glm::vec3 hitPos = origin + rayhit.ray.tfar * direction;

            glm::vec3 newRandomRay = glm::vec3((float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f);
            newRandomRay = glm::normalize(newRandomRay);

            material_t mat = scene->getMaterials()[rayhit.hit.geomID];

            if(glm::dot(newRandomRay, n) > 0)
                return mat.Color  * FollowRay(hitPos, newRandomRay, scene, depth - 1);
            else
                return mat.Color  * FollowRay(hitPos, -newRandomRay, scene, depth - 1);
        }
        else {
            float a = 0.5*(-direction.y + 1.0);
            return (1.0f-a)*colorrgba_v_t(1.0, 1.0, 1.0) + a*colorrgba_v_t(0.5, 0.7, 1.0);
        }
    }

    int SAMPLE_COUNT = 100; 
    int DEPTH = 10;
    std::map<std::pair<int,int>, std::mutex> locks;

    void SceneDrawer::CalculateSample(int row, ScenePtr scene, int screenWidth, int screenHeight, glm::vec3 origin, colorrgba_v_t *colorRow){
        
        for(int j = 0; j<screenWidth; j++){
            colorRow[j] = glm::vec3(0);

            // Cast ray from pos to pixel at near plane
            for(int sample = 0; sample<SAMPLE_COUNT; sample++){

                glm::vec2 eps = glm::vec2((float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f) * 0.7f;

                // calculate point position on the near plane
                glm::vec3 pointPos = glm::vec3(
                    -0.5f + ((float)j + eps.x)  / (float)(screenWidth - 1),
                    (-0.5f + ((float)row + eps.y)  / (float)(screenHeight - 1)) / scene->getCamera().getAspectRatio() ,
                    scene->getCamera().getNearPlaneDistance()
                );

                

                pointPos = scene->getCamera().getTransform().toMat4() * glm::vec4(pointPos.x, pointPos.y, pointPos.z, 1.0);

                glm::vec3 rayDir = (pointPos - origin);
                colorRow[j] += FollowRay(origin, rayDir, scene, DEPTH);
            }
        }
    }

    void SceneDrawer::Setup(GLDrawer *drawer)
    {
        m_Drawer = drawer;
    }

    void SceneDrawer::DrawScene(ScenePtr scene, int screenWidth, int screenHeight, int sampleCount, int depth)
    {
        SAMPLE_COUNT = sampleCount;
        DEPTH = depth;
        m_CurrentScene = scene;

        scene->getCamera().getAspectRatio() = (float)screenWidth / (float)screenHeight;       

        glm::vec3 positionToRenderFrom = scene->getCamera().getTransform().Position;

        std::vector<std::thread> workers;


        colorrgba_v_t *color[screenHeight];

        for(int i = 0; i<screenHeight; i++){
            color[i] = new colorrgba_v_t[screenWidth];
            for(int j = 0; j<screenWidth; j++)
                color[i][j] = glm::vec3(0);

            workers.push_back(
                std::thread(
                    CalculateSample, i, scene, screenWidth, screenHeight, positionToRenderFrom, color[i]
                )
            );
        }

        for(int i = 0; i<workers.size(); i++)
            workers[i].join();

        for(int i = 0; i<screenHeight; i++){
            for(int j = 0; j<screenWidth; j++){
                color[i][j] = color[i][j] * (1 / (float)sampleCount);

                m_Drawer->SetCanvasPixel(j, i, color[i][j]);
            }

            delete [] color[i];
        }
    }
}