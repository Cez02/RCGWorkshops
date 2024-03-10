#pragma once

#include "mesh.hpp"
#include "common.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

using namespace Assimp;

namespace CandlelightRTC {

    class Model {
    private:
        std::vector<MeshPtr> m_Meshes;
        RTCScene m_RTCScene;

        void processNode(aiNode *node, const aiScene *scene, RTCDevice device);
        MeshPtr processMesh(aiMesh *mesh, const aiScene *scene, RTCDevice device);

    public:
        void LoadModel(std::string directory, RTCDevice device);
        
        RTCScene &getRTCScene();

        std::vector<MeshPtr> &getMeshes();

        static std::shared_ptr<Model> GetSphereModel(RTCDevice device);
        static std::shared_ptr<Model> GetPlaneModel(RTCDevice device);

    };

    typedef std::shared_ptr<Model> ModelPtr;
}