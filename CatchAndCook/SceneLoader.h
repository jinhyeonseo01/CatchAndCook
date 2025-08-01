#pragma once

using json = nlohmann::json;


class SceneLoader
{
public:
	void Init(const std::wstring& path);
	std::vector<std::shared_ptr<GameObject>> Load(const std::shared_ptr<Scene>& scene);

    template <class T, typename std::enable_if<std::is_base_of_v<IGuid, T>, int>::type = 0>
    static std::shared_ptr<T> CreateObject(const std::wstring& guid)
    {
        std::shared_ptr<T> object = std::make_shared<T>(); //
        object->SetGUID(guid);
        object->InitGuid();
        return object;
    }

    SceneLoader();
    virtual ~SceneLoader();

    std::unordered_map<std::wstring, json*> refJson_GameObjectTable;
    std::unordered_map<std::wstring, json*> refJson_ComponentTable;
    std::unordered_map<std::wstring, json*> refJson_MaterialTable;

    std::vector<std::shared_ptr<Component>> componentCache;
    std::vector<std::shared_ptr<GameObject>> gameObjectCache;
    std::vector<std::shared_ptr<Material>> materialCache;

    std::shared_ptr<Scene> _scene;

    void PrevProcessingGameObject(json& data);
    void PrevProcessingComponent(json& data);
    void PrevProcessingMaterial(json& data);

    void LinkGameObject(json& jsonData);
    void LinkComponent(json& jsonData);
    void LinkMaterial(json& jsonData);

    void PostProcessingComponent();

    void ProcessingAnimationModelLoad(json& jsonData);
    void ProcessingAnimationMapping(json& jsonData);

public:
    json referenceJson;
    json animationBoneMappingJson;
    json animationModelListJson;
    std::wstring _path;
};

