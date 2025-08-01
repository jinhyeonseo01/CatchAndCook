#include "pch.h"
#include "SceneLoader.h"
#include "SceneLoader.h"
#include <nlohmann/json_fwd.hpp>
#include "AnimationListComponent.h"
#include "BufferPool.h"
#include "CameraComponent.h"
#include "Canvas.h"
#include "Collider.h"
#include "Scene.h"
#include "Component.h"
#include "ImageRenderer.h"
#include "InitComponent.h"
#include "LightComponent.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ModelMesh.h"
#include "NavMesh.h"
#include "PhysicsComponent.h"
#include "RectTransform.h"
#include "SkinnedMeshRenderer.h"
#include "Terrain.h"


SceneLoader::SceneLoader()
{

}

SceneLoader::~SceneLoader()
{

}

std::vector<std::shared_ptr<GameObject>> SceneLoader::Load(const std::shared_ptr<Scene>& scene)
{
    _scene = scene;

    for (auto it = referenceJson.begin(); it != referenceJson.end(); ++it)
    {
        auto& key = it.key();
        auto& value = it.value();

        if (key == "GameObjects")
            for (auto& objectJson : value)
                PrevProcessingGameObject(objectJson);
        if (key == "Components")
            for (auto& objectJson : value)
                PrevProcessingComponent(objectJson);
        if (key == "Materials")
            for (auto& objectJson : value)
                PrevProcessingMaterial(objectJson);
    }

    ProcessingAnimationModelLoad(animationModelListJson);
    ProcessingAnimationMapping(animationBoneMappingJson);

    for (auto& ref : refJson_MaterialTable)
        LinkMaterial(*ref.second);
    for (auto& ref : refJson_ComponentTable)
        LinkComponent(*ref.second);
    for (auto& ref : refJson_GameObjectTable)
        LinkGameObject(*ref.second);

    std::vector<std::shared_ptr<GameObject>> result = gameObjectCache;

    std::cout << std::format("Load - {0}", std::to_string(this->_path)) << "\n";
    std::cout << std::format("  GameObject Count - {0}", refJson_GameObjectTable.size()) << "\n";
    std::cout << std::format("  Component Count - {0}", refJson_ComponentTable.size()) << "\n";
    std::cout << std::format("  Material Count - {0}", refJson_MaterialTable.size()) << "\n";

    componentCache.clear();
    gameObjectCache.clear();
    materialCache.clear();
    _scene = nullptr;
    return result;
}


void SceneLoader::Init(const std::wstring& path)
{
    std::ifstream reader{ path };
    if (!reader)
    {
        std::cout << "Json Load Fail : Wrong Path" << "\n";
        return;
    }
    this->_path = path;
    reader >> referenceJson;
    animationBoneMappingJson = referenceJson["AnimationBoneMapping"];
	animationModelListJson = referenceJson["AnimationModels"];
    referenceJson = referenceJson["references"];
}

void SceneLoader::PrevProcessingGameObject(json& data)
{
	std::wstring guid = std::to_wstring(data["guid"].get<std::string>());
	auto gameObject = CreateObject<GameObject>(guid);
    gameObject->SetParent(nullptr);
	refJson_GameObjectTable[guid] = &data;
	gameObjectCache.emplace_back(gameObject);
}

void SceneLoader::PrevProcessingComponent(json& data)
{
    std::wstring guid = std::to_wstring(data["guid"].get<std::string>());
    std::wstring type = std::to_wstring(data["type"].get<std::string>());
    std::shared_ptr<Component> component;
    refJson_ComponentTable[guid] = &data;


    if (type == L"Transform")
    {
        auto transform = CreateObject<Transform>(guid);
        component = transform;
    }
    if (type == L"RectTransform")
    {
        auto transform = CreateObject<RectTransform>(guid);
        component = transform;
    }
    if (type == L"MeshRenderer")
    {
        auto meshRenderer = CreateObject<MeshRenderer>(guid);
        component = meshRenderer;

        auto mesh = data["mesh"];
        auto modelName = std::to_wstring(mesh["modelName"].get<std::string>());
        auto path = std::to_wstring(mesh["path"].get<std::string>());
        //auto pack = ResourceManager::main->Load<Model>(modelName, path, VertexType::Vertex_Static);
        auto pack = ResourceManager::main->Load<Model>(path, path, VertexType::Vertex_Static);
    }
    if (type == L"SkinnedMeshRenderer")
    {
        auto meshRenderer = CreateObject<SkinnedMeshRenderer>(guid);
        component = meshRenderer;

        auto mesh = data["mesh"];
        /*auto pack = ResourceManager::main->Load<Model>(
            std::to_wstring(mesh["modelName"].get<std::string>()),
            std::to_wstring(mesh["path"].get<std::string>()), VertexType::Vertex_Skinned);*/
        auto path = std::to_wstring(mesh["path"].get<std::string>());
        auto pack = ResourceManager::main->Load<Model>(
            path + L"_Skinned",
            path, VertexType::Vertex_Skinned);
    }
    if (type == L"MeshFilter")
    {

    }
    if (type == L"BoxCollider")
    {
        auto collider = CreateObject<Collider>(guid);
        component = collider;
    }
    if(type == L"SphereCollider")
    {
        auto collider = CreateObject<Collider>(guid);
        component = collider;
    }
    if(type == L"Rigidbody")
    {
        auto pc = CreateObject<PhysicsComponent>(guid);
        component = pc;
    }
    if(type == L"Terrain")
    {
        auto terr = CreateObject<Terrain>(guid);
        component = terr;
    }
    if(type == L"Animator")
    {
        auto terr = CreateObject<SkinnedHierarchy>(guid);
        component = terr;
    }
    if (type == L"NavMeshSurface")
    {
        auto terr = CreateObject<NavMesh>(guid);
        component = terr;
    }
    if (type == L"Camera")
    {
        auto terr = CreateObject<CameraComponent>(guid);
        component = terr;
    }
    if (type == L"Tags")
    {
        auto terr = CreateObject<TagsComponent>(guid);
        component = terr;
    }
    if (type == L"Scripts")
    {
        auto terr = CreateObject<ScriptsComponent>(guid);
        component = terr;
    }
    if (type == L"Light")
    {
        auto terr = CreateObject<LightComponent>(guid);
        component = terr;
    }
    if (type == L"AnimationList")
    {
        auto terr = CreateObject<AnimationListComponent>(guid);
        component = terr;
    }
    if (type == L"Image")
    {
        auto terr = CreateObject<ImageRenderer>(guid);
        component = terr;
    }
    if (type == L"Canvas")
    {
        auto terr = CreateObject<Canvas>(guid);
        component = terr;
    }
    componentCache.emplace_back(component);
}

void SceneLoader::PrevProcessingMaterial(json& data)
{
	std::wstring guid = std::to_wstring(data["guid"].get<std::string>());
	auto material = CreateObject<Material>(guid);
	refJson_MaterialTable[guid] = &data;

	for (auto& texture : data["datas"]["textures"]) {
        ResourceManager::main->Load<Texture>(
            std::to_wstring(texture["path"].get<std::string>()),//std::to_wstring(texture["originalName"].get<std::string>()),
            std::to_wstring(texture["path"].get<std::string>()),
                TextureType::Texture2D, false
		);
	}
	materialCache.emplace_back(material);
}

void SceneLoader::LinkGameObject(json& jsonData)
{

    std::wstring guid = std::to_wstring(jsonData["guid"].get<std::string>());
    std::wstring parentGuid = std::to_wstring(jsonData["parent"].get<std::string>());

    std::shared_ptr<GameObject> gameObject = IGuid::FindObjectByGuid<GameObject>(guid);
    std::shared_ptr<GameObject> parentObject = IGuid::FindObjectByGuid<GameObject>(parentGuid);

    if (gameObject != nullptr)
    {
        auto isActive = jsonData["active"].get<bool>();
        auto isStatic = jsonData["static"].get<bool>();
        auto isDeactivate = jsonData["deactivate"].get<bool>();

        gameObject->SetName(std::to_wstring(jsonData["name"].get<std::string>()));
        gameObject->SetScene(_scene);

        for (auto& componentGuid : jsonData["components"])
            gameObject->AddComponent(
	            IGuid::FindObjectByGuid<Component>(std::to_wstring(componentGuid.get<std::string>())));

        if(isDeactivate)
            gameObject->SetType(GameObjectType::Deactivate);
        else
            gameObject->SetType(isStatic ? GameObjectType::Static : GameObjectType::Dynamic); // 수정이 필요함. Unity 쪽의 static 옵션 받아오게 해야함.
        _scene->AddGameObject(gameObject);
        gameObject->SetActiveSelf(isActive);
    	gameObject->Init();
        gameObject->SetParent(parentObject);
    }
/*return gameObject;*/
}

void SceneLoader::LinkComponent(json& jsonData)
{


    std::wstring guid = std::to_wstring(jsonData["guid"].get<std::string>());
    std::wstring type = std::to_wstring(jsonData["type"].get<std::string>());

    if (type == L"Transform")
    {
        auto transform = IGuid::FindObjectByGuid<Transform>(guid);
        auto pos = Vector3(
            jsonData["position"][0].get<float>(),
            jsonData["position"][1].get<float>(),
            jsonData["position"][2].get<float>());
        auto rotation = Quaternion(
            jsonData["rotation"][0].get<float>(),
            jsonData["rotation"][1].get<float>(),
            jsonData["rotation"][2].get<float>(),
            jsonData["rotation"][3].get<float>());
        auto scale = Vector3(
            jsonData["scale"][0].get<float>(),
            jsonData["scale"][1].get<float>(),
            jsonData["scale"][2].get<float>());
        transform->SetLocalPosition(pos);
        transform->SetLocalRotation(rotation);
        transform->SetLocalScale(scale);
    }

    if (type == L"RectTransform")
    {
        auto transform = IGuid::FindObjectByGuid<RectTransform>(guid);
        auto& d = transform->_rectTransformData;
        Vector3 pos = {
        jsonData["anchoredPosition"][0].get<float>(),
        	jsonData["anchoredPosition"][1].get<float>(),
        jsonData["position"][2].get<float>()
        };
  //      pos = Vector3{
		//jsonData["position"][0].get<float>(),
		//jsonData["position"][1].get<float>(),
		//jsonData["position"][2].get<float>()
  //      };
        Quaternion rot = {
            jsonData["rotation"][0].get<float>(),
            jsonData["rotation"][1].get<float>(),
            jsonData["rotation"][2].get<float>(),
            jsonData["rotation"][3].get<float>()
        };
        Vector3 scl = {
            jsonData["scale"][0].get<float>(),
            jsonData["scale"][1].get<float>(),
            jsonData["scale"][2].get<float>()
        };
        transform->SetLocalPosition(pos);
        transform->SetLocalRotation(rot);
        transform->SetLocalScale(scl);

        // 2) RectTransformData 채우기
        d.pivot = Vector2(
            jsonData["pivot"][0].get<float>(),
            jsonData["pivot"][1].get<float>()
        );
        d.anchorMin = Vector2(
            jsonData["anchorMin"][0].get<float>(),
            jsonData["anchorMin"][1].get<float>()
        );
        d.anchorMax = Vector2(
            jsonData["anchorMax"][0].get<float>(),
            jsonData["anchorMax"][1].get<float>()
        );
        d.rectSize = Vector2(
            jsonData["rectSize"][0].get<float>(),
            jsonData["rectSize"][1].get<float>()
        );
        //d.rectSize = Vector2(
        //    jsonData["rectSize"][0].get<float>(),
        //    jsonData["rectSize"][1].get<float>()
        //);

        d.paddingMin = Vector2(
            jsonData["offsetMin"][0].get<float>(),
            jsonData["offsetMin"][1].get<float>()
        );
        d.paddingMax = Vector2(
            jsonData["offsetMax"][0].get<float>(),
            jsonData["offsetMax"][1].get<float>()
        );

    }

    if (type == L"MeshRenderer")
    {
        auto meshRenderer = IGuid::FindObjectByGuid<MeshRenderer>(guid);
        auto meshInfo = jsonData["mesh"];
        auto modelName = std::to_wstring(meshInfo["modelName"].get<std::string>());
        
        //auto model = ResourceManager::main->Get<Model>(modelName);
        auto model = ResourceManager::main->Get<Model>(std::to_wstring(meshInfo["path"].get<std::string>()));
        if (model != nullptr)
        {
	        std::vector<std::shared_ptr<ModelMesh>> meshes = model->FindMeshsByName(meshInfo["meshName"].get<std::string>());
            std::vector<std::shared_ptr<Material>> materials;
            for(auto& materialGuidJson : jsonData["materials"])
            {
                auto materialGuid = std::to_wstring(materialGuidJson.get<std::string>());
                auto material = IGuid::FindObjectByGuid<Material>(materialGuid);
                auto& materialData = (*refJson_MaterialTable[materialGuid]);
                auto shaderName = std::to_wstring(materialData["shaderName"].get<std::string>());
                auto shader = ResourceManager::main->Get<Shader>(shaderName);

                if (_scene->GetSceneType() != SceneType::Sea01)
                {
                    if (shader == nullptr)
                    {
                        shader = ResourceManager::main->Get<Shader>(L"DefaultDeferred");// Deferred DefaultForward_Skinned DefaultForward
                        material->SetShader(shader);
                        material->SetPass(shader->GetPass());
                    }
                    else
                    {
                        material->SetShader(shader);
                        material->SetPass(shader->GetPass());
                    }
                    if (material->GetPass()| RENDER_PASS::Forward)
						material->SetPreDepthNormal(true);
                }

                else
                {
                    if (shader == nullptr)
                    {
                 
                        shader = ResourceManager::main->Get<Shader>(L"D_SeaEnv");
                        material->SetShader(shader);
                        material->SetPass(shader->GetPass());
                    }
                    else
                    {
                    
                        if (shaderName==L"Environment")
                        {
                            shader = ResourceManager::main->Get<Shader>(L"D_SeaEnv");
                        }

                        //if (shaderName == L"DeferredSeaGodRayPrePass")
                        //{
                        //    cout << "걸릿누 ㅋㅋ" << endl;
                        //}

                        material->SetShader(shader);
                        material->SetPass(shader->GetPass());
                    }
                }

                materials.push_back(material);
            }

            meshRenderer->AddMaterials(materials);
            for(auto& mesh : meshes)
				meshRenderer->AddMesh(mesh->GetMesh());
        }
        else
        {
            std::cout << "Json Load - Not Found Model : " << std::to_string(modelName) << "\n";
        }
    }
    if(type == L"SkinnedMeshRenderer")
    {
        auto skinnedmeshRenderer = IGuid::FindObjectByGuid<SkinnedMeshRenderer>(guid);
        auto meshInfo = jsonData["mesh"];
        auto modelName = std::to_wstring(meshInfo["modelName"].get<std::string>());
        auto boneName = std::to_wstring(jsonData["boneRoot"].get<std::string>());
        auto path = std::to_wstring(meshInfo["path"].get<std::string>());

        //auto model = ResourceManager::main->Get<Model>(modelName);
        auto model = ResourceManager::main->Get<Model>(path + L"_Skinned");
        if(model != nullptr)
        {
            std::vector<std::shared_ptr<ModelMesh>> meshes = model->FindMeshsByName(meshInfo["meshName"].get<std::string>());
            std::vector<std::shared_ptr<Material>> materials;
            for (auto& materialGuidJson : jsonData["materials"])
            {
                auto materialGuid = std::to_wstring(materialGuidJson.get<std::string>());
                auto material = IGuid::FindObjectByGuid<Material>(materialGuid);
                auto& materialData = (*refJson_MaterialTable[materialGuid]);
                auto shaderName = std::to_wstring(materialData["shaderName"].get<std::string>());
                auto shader = ResourceManager::main->Get<Shader>(shaderName);

                if (_scene->GetSceneType() != SceneType::Sea01)
                {
                    if (shader == nullptr)
                    {
                        shader = ResourceManager::main->Get<Shader>(L"DefaultForward_Skinned");
                        material->SetShader(shader);
                        material->SetPass(shader->GetPass());
                    }
                    else
                    {
                        material->SetShader(shader);
                        material->SetPass(shader->GetPass());
                    }

                    material->SetPreDepthNormal(true);
                }

                else
                {

                    if (shader == nullptr)
                    {
                        shader = ResourceManager::main->Get<Shader>(L"DeferredSeaSkinned");
                        material->SetShader(shader);
                        material->SetPass(shader->GetPass());
                    }
                    else
                    {
                        material->SetShader(shader);
                        material->SetPass(shader->GetPass());
                    }
                }

                materials.push_back(material);
            };

            skinnedmeshRenderer->AddMaterials(materials);
            for(auto& mesh : meshes)
                skinnedmeshRenderer->AddMesh(mesh->GetMesh());
            skinnedmeshRenderer->SetModel(model);
            skinnedmeshRenderer->SetBoneRootName(boneName);
        } else
        {
            std::cout << "Json Load - Not Found Model : " << std::to_string(modelName) << "\n";
        }
    }
    if (type == L"BoxCollider")
    {
        auto collider = IGuid::FindObjectByGuid<Collider>(guid);
        auto center = Vector3(
            jsonData["center"][0].get<float>(),
            jsonData["center"][1].get<float>(),
            jsonData["center"][2].get<float>());
        auto size = Vector3(
            jsonData["size"][0].get<float>(),
            jsonData["size"][1].get<float>(),
            jsonData["size"][2].get<float>());
        collider->SetTrigger(jsonData["isTrigger"].get<bool>());
        collider->SetBoundingBox(center, size/2);
    }
    if (type == L"SphereCollider")
    {
        auto collider = IGuid::FindObjectByGuid<Collider>(guid);
        auto center = Vector3(
            jsonData["center"][0].get<float>(),
            jsonData["center"][1].get<float>(),
            jsonData["center"][2].get<float>());
        auto radius = jsonData["radius"].get<float>();
        collider->SetTrigger(jsonData["isTrigger"].get<bool>());
        collider->SetBoundingSphere(center, radius);
    }
    if(type == L"Rigidbody")
    {
        auto collider = IGuid::FindObjectByGuid<PhysicsComponent>(guid);

    }

    if(type == L"Animator")
    {
        auto skinnedHierarchy = IGuid::FindObjectByGuid<SkinnedHierarchy>(guid);

        auto& boneMapping =  jsonData["boneMapping"];
        std::unordered_map<std::string, std::string> boneMapTable;

        for (auto it = boneMapping.begin(); it != boneMapping.end(); ++it) {
            boneMapTable[it.key()] = it.value();
            skinnedHierarchy->SetMappingTable(boneMapTable);
        }
    }

    if(type == L"Terrain")
    {
        auto terrain = IGuid::FindObjectByGuid<Terrain>(guid);
        std::wstring rawPath = to_wstring(jsonData["rawPath"].get<string>());
        std::wstring pngPath = to_wstring(jsonData["pngPath"].get<string>());
        float rawSize = jsonData["heightmapResolution"].get<float>();
        auto fieldSize = Vector3(
		    jsonData["size"][0].get<float>(),
		    jsonData["size"][1].get<float>(),
		    jsonData["size"][2].get<float>());


        auto material = make_shared<Material>();

        int diffuseCount = jsonData["layers"].size();
        for(int i = 0; i < diffuseCount; i++)
        {
            auto& layer = jsonData["layers"][i];
            auto diffusePath = to_wstring(layer["diffuse"]["path"].get<string>());

            auto active = Vector4(0,0,0,0);

            auto diffuse = ResourceManager::main->Load<Texture>(diffusePath, diffusePath, TextureType::Texture2D, false, true);
            material->SetTexture(std::format("_detailMap{0}",i), diffuse);
            if(layer.contains("normal"))
            {
                auto normalPath = to_wstring(layer["normal"]["path"].get<string>());
                auto normal = ResourceManager::main->Load<Texture>(normalPath, normalPath, TextureType::Texture2D,false);
                material->SetTexture(std::format("_normalMap{0}",i), normal);
                active.x = 1;
            }
            if(layer.contains("mask"))
            {
                auto maskPath = to_wstring(layer["mask"]["path"].get<string>());
                auto mask = ResourceManager::main->Load<Texture>(maskPath,maskPath,TextureType::Texture2D,false);
                material->SetTexture(std::format("_maskMap{0}",i),mask);
                active.y = 1;
            }
            auto tile = Vector4(
                layer["tileSize"][0].get<float>(),
                layer["tileSize"][1].get<float>(),
                layer["tileOffset"][0].get<float>(),
                layer["tileOffset"][1].get<float>());
            material->SetPropertyVector(std::format("_tileST{0}",i), tile);
            material->SetPropertyVector(std::format("_active{0}",i), active);
        }
        int blendCount = jsonData["layerBlendTextures"].size();
        for(int i = 0; i < blendCount; i++)
        {
            auto& layer = jsonData["layerBlendTextures"][i];
            auto blendPath = to_wstring(layer.get<string>());
            auto blend = ResourceManager::main->Load<Texture>(blendPath,blendPath,TextureType::Texture2D,false);
            material->SetTexture(std::format("_blendMap{0}",i),blend);
        }

        vector<shared_ptr<GameObject>> instances;
        vector<vector<Instance_Transform>> instancesDatas;

        int instanceTableCount = jsonData["instanceTable"].size();
        for(int i = 0; i < instanceTableCount; i++)
        {
            std::shared_ptr<GameObject> obj = IGuid::FindObjectByGuid<GameObject>(to_wstring(jsonData["instanceTable"][i].get<string>()));
            if(obj != nullptr) {
                instances.push_back(obj);
            }
        }

        instancesDatas.resize(instanceTableCount);

        int instanceCount = jsonData["instanceCount"].get<float>();
        for(int i = 0; i < instanceCount; i++)
        {
            auto currentInst = jsonData["instanceDatas"][i];
            int index = currentInst["index"].get<int>();
            auto pos = Vector3(
                currentInst["position"][0].get<float>(),
                currentInst["position"][1].get<float>(),
                currentInst["position"][2].get<float>());
			float angle = currentInst["rotation"].get<float>();
            auto scale = Vector3(
                currentInst["scale"][0].get<float>(),
                currentInst["scale"][1].get<float>(),
                currentInst["scale"][2].get<float>());
            Matrix InvertTRS;

            std::random_device rd;
            std::uniform_real_distribution<float> distReal(-0.1f, 1.0f);
            std::uniform_real_distribution<float> distReal2(0, 360 * D2R);
            scale.y *= 1 + distReal(rd) * 0.2f;
            angle += distReal2(rd);

            auto rot = Quaternion::CreateFromAxisAngle(Vector3::Up, angle);

        	auto trs = Matrix::CreateScale(scale) *
                Matrix::CreateFromQuaternion(rot) *
                Matrix::CreateTranslation(pos);
			trs.Invert(InvertTRS);

            instancesDatas[index].push_back({trs, InvertTRS, pos});
 /*           instancesDatas[index].push_back({trs,InvertTRS});*/
        }

        //instanceTable
        
        material->SetPropertyInt("detailsCount",diffuseCount);
        material->SetPropertyInt("blendCount", blendCount);
        terrain->SetMaterial(material);
        terrain->SetInstances(instances);
		terrain->SetInstanceDatas(instancesDatas);
		terrain->SetHeightMap(rawPath,pngPath, Vector2(rawSize,rawSize), fieldSize);
    }


    if (type == L"NavMeshSurface")
    {
        auto navMesh = IGuid::FindObjectByGuid<NavMesh>(guid);

        std::vector<NavMeshData> datas;
        std::vector< std::array<Vector3, 2>> edgeDatas;
        std::vector<int> tris;

        auto& vertexs = jsonData["vertexs"];
        auto& indexs = jsonData["tris"];
        auto& adjacency = jsonData["adjacency"];
        auto& edges = jsonData["edge"];

        int vertexCount = vertexs.size();
        int edgeCount = edges.size();
        for (int i = 0; i < vertexCount; i++)
        {
            NavMeshData data;
			data.index = i;
            data.position = Vector3(
                vertexs[i][0].get<float>(),
                vertexs[i][1].get<float>(),
                vertexs[i][2].get<float>()
            );
            for (int j = 0; j < adjacency[i].size(); j++)
            {
				int adjIndex = adjacency[i][j].get<int>();
				data.adjectIndexs.push_back(adjIndex);
            }
            datas.push_back(data);
        }
        for (int j = 0; j < indexs.size(); j++)
        {
            tris.push_back(indexs[j][0].get<int>());
            tris.push_back(indexs[j][1].get<int>());
            tris.push_back(indexs[j][2].get<int>());
        }
        for (int i = 0; i < edgeCount; i++)
        {
            std::array<Vector3, 2> edgePos;
			edgePos[0] = Vector3(
				edges[i][0][0].get<float>(),
				edges[i][0][1].get<float>(),
				edges[i][0][2].get<float>()
			);
            edgePos[1] = Vector3(
                edges[i][1][0].get<float>(),
                edges[i][1][1].get<float>(),
                edges[i][1][2].get<float>()
            );
			edgeDatas.push_back(edgePos);
        }

		navMesh->SetNavMeshData(datas);
        navMesh->SetNavMeshEdgeData(edgeDatas);
        navMesh->SetTris(tris);
        NavMeshManager::main->SetTriangles(tris);
        NavMeshManager::main->SetNavMeshData(datas);
    }
    if (type == L"Light")
    {
        auto light = IGuid::FindObjectByGuid<LightComponent>(guid);

        auto type = jsonData["lightType"].get<string>();

        if (type == "Directional")
            light->type = LIGHT_TYPE::DIRECTIONAL_LIGHT;
        if (type == "Point")
            light->type = LIGHT_TYPE::POINT_LIGHT;
        if (type == "Spot")
            light->type = LIGHT_TYPE::SPOT_LIGHT;

        auto color = Vector3(
            jsonData["color"][0].get<float>(),
            jsonData["color"][1].get<float>(),
            jsonData["color"][2].get<float>());
        auto intensity = jsonData["intensity"].get<float>();
        auto range = jsonData["range"].get<float>();
        auto innerSpotAngle = jsonData["innerSpotAngle"].get<float>();
        auto spotAngle = jsonData["spotAngle"].get<float>();
        auto shadowAngle = jsonData["shadowAngle"].get<float>();

		light->color = color;
        light->intensity = intensity;
        light->range = range;
        light->innerSpotAngle = innerSpotAngle;
        light->spotAngle = spotAngle;
        light->shadowAngle = shadowAngle;
    }
    if (type == L"Tags")
    {
        auto compo = IGuid::FindObjectByGuid<TagsComponent>(guid);
		//compo->SetTag(std::to_wstring(jsonData["tag"].get<std::string>()));
        auto& tags = jsonData["tags"];

        int tagsCount = tags.size();
        for (int i = 0; i < tagsCount; i++)
        {
			compo->_tagNames.push_back(tags[i].get<std::string>());
        }
    }
    if (type == L"Scripts")
    {
        auto compo = IGuid::FindObjectByGuid<ScriptsComponent>(guid);
        auto& scripts = jsonData["scripts"];

        int scriptsCount = scripts.size();
        for (int i = 0; i < scriptsCount; i++)
        {
            compo->_scriptNames.push_back(scripts[i].get<std::string>());
        }
    }
    if (type == L"AnimationList")
    {
        auto compo = IGuid::FindObjectByGuid<AnimationListComponent>(guid);
        auto& scripts = jsonData["animationKeys"];

        int scriptsCount = scripts.size();

        for (auto it = scripts.begin(); it != scripts.end(); ++it) {
            compo->_animationKeys.emplace(it.key(), it.value());
        }
    }
    if (type == L"Image")
    {
        auto compo = IGuid::FindObjectByGuid<ImageRenderer>(guid);
        auto& scripts = jsonData["sprite"]["path"];

        auto size = Vector2(
            jsonData["sprite"]["size"]["width"].get<float>(),
            jsonData["sprite"]["size"]["height"].get<float>());
        auto spriteST = Vector4(
            jsonData["sprite"]["uvOffset"]["u"].get<float>(),
            jsonData["sprite"]["uvOffset"]["v"].get<float>(),
            jsonData["sprite"]["uvSize"]["width"].get<float>(),
			jsonData["sprite"]["uvSize"]["height"].get<float>());
        auto border = Vector4(
	    jsonData["sprite"]["border"]["left"].get<float>(),
	    jsonData["sprite"]["border"]["bottom"].get<float>(),
	    jsonData["sprite"]["border"]["right"].get<float>(),
	    jsonData["sprite"]["border"]["top"].get<float>() );


        auto path = std::to_wstring(scripts.get<std::string>());
        if ((path != L""))
        {
            auto texture = ResourceManager::main->Load<Texture>(path, path);
            auto sprites = GUISprite::Create(texture, { SimpleMath::Rectangle{ (int)spriteST.x,(int)spriteST.y, (int)spriteST.z, (int)spriteST.w} });
			sprites[0]->_border = border;
            compo->SetSprite(sprites[0]);
            //sprite->SetTexture(texture);
        }
    }
    if (type == L"Canvas")
    {
        auto compo = IGuid::FindObjectByGuid<Canvas>(guid);
        auto canvasType = jsonData["canvasType"].get<std::string>();
        if (canvasType == "ScreenSpaceOverlay")
			compo->type = CanvasType::Overlay;
        //ScreenSpaceOverlay
        auto resolution = Vector2(
            jsonData["canvasResolution"][0].get<float>(),
            jsonData["canvasResolution"][1].get<float>());
		compo->resolution = resolution;
        compo->match = jsonData["canvasMatch"].get<float>();


    }

    //Canvas
}

void SceneLoader::LinkMaterial(json& jsonData)
{
    std::wstring guid = std::to_wstring(jsonData["guid"].get<std::string>());

    auto material = IGuid::FindObjectByGuid<Material>(guid);
    auto datas = jsonData["datas"];
    auto textures = datas["textures"];
    auto floats = datas["floats"];
    auto ints = datas["ints"];
    auto vectors = datas["vectors"];
    
    for(auto& texture : textures)
    {
        auto a = std::to_wstring(texture["originalName"].get<std::string>());
        material->SetTexture(
            texture["name"].get<std::string>(),
            ResourceManager::main->Get<Texture>(std::to_wstring(texture["path"].get<std::string>())));
    }
    for (auto& data : floats)
        material->SetPropertyFloat(data["name"].get<std::string>(), data["data"].get<float>());
    for (auto& data : ints)
        material->SetPropertyInt(data["name"].get<std::string>(), data["data"].get<int>());
    for (auto& data : vectors)
        material->SetPropertyVector(data["name"].get<std::string>(), Vector4(
            data["data"][0].get<float>(),
            data["data"][1].get<float>(),
            data["data"][2].get<float>(),
            data["data"][3].get<float>()
        ));
}

void SceneLoader::PostProcessingComponent()
{

}


void SceneLoader::ProcessingAnimationModelLoad(json& jsonData)
{
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
		auto modelName = std::to_wstring(it.value().get<std::string>());
		auto path = std::to_wstring(it.value().get<std::string>());
		auto model = ResourceManager::main->Load<Model>(modelName, path, VertexType::Vertex_Skinned);
    }
}

void SceneLoader::ProcessingAnimationMapping(json& jsonData)
{
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        SkinnedHierarchy::_boneNameToHumanMappingTable[it.key()] = it.value();
    }
}