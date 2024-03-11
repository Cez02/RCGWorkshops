#include "model.hpp"

#include "logger.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>


namespace CandlelightRTC {
    void Model::LoadModel(std::string directory, RTCDevice device)
    {
        Assimp::Importer import;
                    // std::cout << "Loading" << std::endl;

        const aiScene *scene = import.ReadFile(directory, 
            aiProcess_CalcTangentSpace | 
            aiProcess_JoinIdenticalVertices | 
            aiProcess_Triangulate | 
            aiProcess_GenNormals |
            aiProcess_GenUVCoords );	
                    // std::cout << "Loaded" << std::endl;

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }

        // directory = path.substr(0, path.find_last_of('/'));

        // std::cout << "Processing nodes" << std::endl;
        processNode(scene->mRootNode, scene, device);
        // std::cout << "Done processing nodes" << std::endl;




        // Go over added meshes and create a parent rtcScene

        m_RTCScene = rtcNewScene(device);

        for(int i = 0; i<m_Meshes.size(); i++){

            auto transMat = glm::identity<glm::mat4>();

            RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_INSTANCE);
            rtcSetGeometryInstancedScene(geom, m_Meshes[i]->getRTCScene());
            rtcSetGeometryTimeStepCount(geom, 1);
            rtcSetGeometryTransform(geom,
                                    0,
                                    RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR,
                                    glm::value_ptr(transMat));
            rtcCommitGeometry(geom);

            rtcAttachGeometry(m_RTCScene, geom);
            rtcReleaseGeometry(geom);
        }

        rtcCommitScene(m_RTCScene);
    }


    void Model::processNode(aiNode *node, const aiScene *scene, RTCDevice device)
    {

        // process all the node's meshes (if any)
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
            m_Meshes.push_back(processMesh(mesh, scene, device));			
        }

        // then do the same for each of its children
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, device);
        }
    }

    MeshPtr Model::processMesh(aiMesh *mesh, const aiScene *scene, RTCDevice device)
    {
        // data to fill
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            // std::cout << "Walking through vertices" << std::endl;

            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;

            // // normals
            // if (mesh->HasNormals())
            // {
            //     vector.x = mesh->mNormals[i].x;
            //     vector.y = mesh->mNormals[i].y;
            //     vector.z = mesh->mNormals[i].z;
            //     // vector = -1.0f * vector;
            //     vertex.normal = vector;
            // }

            // // texture coordinates
            // if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            // {
            //     glm::vec2 vec;
            //     // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            //     // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            //     vec.x = mesh->mTextureCoords[0][i].x; 
            //     vec.y = mesh->mTextureCoords[0][i].y;
            //     vertex.texUVCoords = vec;

            //     // std::cout << "Tangent not working :(" << std::endl;

            //     // // tangent
            //     vector.x = mesh->mTangents[i].x;
            //     vector.y = mesh->mTangents[i].y;
            //     vector.z = mesh->mTangents[i].z;
            //     vertex.tangent = vector;
            //     // // bitangent
            //     vector.x = mesh->mBitangents[i].x;
            //     vector.y = mesh->mBitangents[i].y;
            //     vector.z = mesh->mBitangents[i].z;
            //     vertex.bitangent = vector;
            // }
            // else
            //     vertex.texUVCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vector.x);
            vertices.push_back(vector.y);
            vertices.push_back(vector.z);
        }


        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }

        // process materials
        // aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    

        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN


        // // 1. diffuse maps
        // std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuseMap");
        // textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // // 2. specular maps
        // std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specularMap");
        // textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // // 3. normal maps
        // std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normalMap");
        // textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // // 4. height maps
        // std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "heightMap");
        // textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data

        MeshPtr newMesh = std::make_shared<Mesh>();

        RTCGeometry geo = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

        newMesh->getIndices() = (uintPtr)rtcSetNewGeometryBuffer(geo,
            RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(u_int), indices.size() / 3);
        memcpy(newMesh->getIndices(), indices.data(), indices.size() * sizeof(u_int));

        newMesh->getVertices() = (floatPtr)rtcSetNewGeometryBuffer(geo,
            RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), vertices.size() / 3);
        memcpy(newMesh->getVertices(), vertices.data(), vertices.size() * sizeof(float));

        CandlelightRTC::LogInfo("Finishing...");

        // newMesh->get = vertices.size();
        // newMesh->m_IndexCount = indices.size();

        newMesh->getRTCScene() = rtcNewScene(device);
        rtcAttachGeometry(newMesh->getRTCScene(), geo);
        rtcCommitGeometry(geo);
        rtcCommitScene(newMesh->getRTCScene());
        rtcReleaseGeometry(geo);

        return newMesh;
    }

    RTCScene &Model::getRTCScene()
    {
        return m_RTCScene;
    }

    std::vector<MeshPtr> &Model::getMeshes()
    {
        return m_Meshes;
    }

    static ModelPtr SPHERE_MODEL = nullptr;
    static ModelPtr PLANE_MODEL = nullptr;

    ModelPtr Model::GetSphereModel(RTCDevice device)
    {
        if(SPHERE_MODEL != nullptr)
            return SPHERE_MODEL;

        SPHERE_MODEL = std::make_shared<Model>();

        SPHERE_MODEL->m_Meshes.push_back(Mesh::getSphereMesh(device));

        // SPHERE_MODEL->m_RTCScene = rtcNewScene(device);

        // for(int i = 0; i<SPHERE_MODEL->m_Meshes.size(); i++){

        //     auto transMat = glm::identity<glm::mat4>();

        //     RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_INSTANCE);
        //     rtcSetGeometryInstancedScene(geom, SPHERE_MODEL->m_Meshes[i]->getRTCScene());
        //     rtcSetGeometryTimeStepCount(geom, 1);
        //     rtcSetGeometryTransform(geom,
        //                             0,
        //                             RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR,
        //                             glm::value_ptr(transMat));
        //     rtcCommitGeometry(geom);

        //     rtcAttachGeometry(SPHERE_MODEL->m_RTCScene, geom);
        //     rtcReleaseGeometry(geom);
        // }

        // rtcCommitScene(SPHERE_MODEL->m_RTCScene);

        return SPHERE_MODEL;
    }

    ModelPtr Model::GetPlaneModel(RTCDevice device)
    {
        if(PLANE_MODEL != nullptr)
            return PLANE_MODEL;

        PLANE_MODEL = std::make_shared<Model>();

        PLANE_MODEL->m_Meshes.push_back(Mesh::getPlaneMesh(device));

        PLANE_MODEL->m_RTCScene = rtcNewScene(device);

        for(int i = 0; i<PLANE_MODEL->m_Meshes.size(); i++){

            auto transMat = glm::identity<glm::mat4>();

            RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_INSTANCE);
            rtcSetGeometryInstancedScene(geom, PLANE_MODEL->m_Meshes[i]->getRTCScene());
            rtcSetGeometryTimeStepCount(geom, 1);
            rtcSetGeometryTransform(geom,
                                    0,
                                    RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR,
                                    glm::value_ptr(transMat));
            rtcCommitGeometry(geom);

            rtcAttachGeometry(PLANE_MODEL->m_RTCScene, geom);
            rtcReleaseGeometry(geom);
        }

        rtcCommitScene(PLANE_MODEL->m_RTCScene);

        return PLANE_MODEL;
    }
}