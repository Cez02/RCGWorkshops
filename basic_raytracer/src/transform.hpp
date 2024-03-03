#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace CandlelightRTC {

    struct transform_t {
    public:
        glm::vec3 Position;
        glm::vec3 Scale;
        glm::quat Rotation;

        glm::mat4 toMat4(){
            glm::mat4 model = glm::mat4(1.0);

            return glm::translate(glm::mat4(1.f), Position) * glm::mat4_cast(Rotation) * glm::scale(glm::mat4(1.f), Scale) * model;
        }

        transform_t(glm::vec3 position = glm::zero<glm::vec3>(), glm::vec3 scale = glm::vec3(1), glm::quat rotation = glm::identity<glm::quat>()){
            Position = position;
            Scale = scale;
            Rotation = rotation;
        }

        glm::vec3 Right(){
            return glm::vec4(1.0, 0.0, 0.0, 1.0) * glm::mat4_cast(Rotation);
        }

        glm::vec3 Forward(){
            return glm::vec4(0.0, 0.0, 1.0, 1.0) * glm::mat4_cast(Rotation);
        }

        glm::vec3 Up(){
            return glm::vec4(0.0, 1.0, 0.0, 1.0) * glm::mat4_cast(Rotation);
        }
    };


}