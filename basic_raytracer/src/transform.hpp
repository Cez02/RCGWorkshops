#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp> 
#include <glm/gtx/quaternion.hpp>

namespace CandlelightRTC {

    struct transform_t {
    public:
        glm::vec3 Position;
        glm::vec3 Scale;
        glm::quat Rotation;

        glm::mat4 toMat4(){
            return glm::translate(Position) * glm::toMat4(Rotation) * glm::scale(Scale);
        }

        transform_t(glm::vec3 position = glm::zero<glm::vec3>(), glm::vec3 scale = glm::vec3(1), glm::quat rotation = glm::identity<glm::quat>()){
            Position = position;
            Scale = scale;
            Rotation = rotation;
        }
    };


}