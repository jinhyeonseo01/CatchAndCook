#include "pch.h"
#include "Terrain.h"
#include "GameObject.h"
#include "TerrainManager.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include <algorithm>
#include "SeaGrassComponent.h"
#include "Camera.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Gizmo.h"

//#define RECT_TERRAIN

Terrain::Terrain()
{

}

Terrain::~Terrain()
{

}

void Terrain::Init()
{
    GetOwner()->AddComponent<MeshRenderer>();
}

void Terrain::Start()
{
    Component::Start();

    auto shader = ResourceManager::main->Get<Shader>(L"Terrain");

    _material->SetPropertyVector("fieldSize", Vector4(_fieldSize));
    _material->SetShader(shader);
    _material->SetPass(RENDER_PASS::Deferred);
    _material->SetShadowCasting(false);
    auto meshRenderer = GetOwner()->GetComponent<MeshRenderer>();
    meshRenderer->AddMaterials({ _material });

    _terrainShadowMaterial = ResourceManager::main->_shadowCaster_Terrain->Clone();
    _material->CopyProperties(_terrainShadowMaterial);

    if (auto renderer = GetOwner()->GetRenderer())
    {
        renderer->AddCbufferSetter(static_pointer_cast<Terrain>(shared_from_this()));
        dynamic_pointer_cast<MeshRenderer>(renderer)->AddMesh(_gridMesh);
        renderer->SetCulling(false);
    }


    if (SceneManager::main->GetCurrentScene()->GetSceneType() != SceneType::Sea01)
    {
        _instanceBuffers.resize(_instancesObject.size());

        for (int i = 0; i < _instancesObject.size(); i++)
        {
        /*    std::cout << "Instance Count (" << to_string(_instancesObject[i].lock()->GetName()) << ") : " << _instanceDatas[i].size() << std::endl;*/
            auto instanceBuffer = Core::main->GetBufferManager()->GetInstanceBufferPool(BufferType::TransformInstanceParam)->Alloc();
            memcpy(instanceBuffer->ptr, _instanceDatas[i].data(), _instanceDatas[i].size() * sizeof(Instance_Transform));
            instanceBuffer->SetIndex(_instanceDatas[i].size(), sizeof(Instance_Transform));
            _instanceBuffers[i] = instanceBuffer;

            std::vector<std::shared_ptr<MeshRenderer>> renderers;
            _instancesObject[i].lock()->GetComponentsWithChilds<MeshRenderer>(renderers);

            for (auto& renderer : renderers)
            {
                renderer->SetCulling(false);
                renderer->SetInstancing(false);
                renderer->SetInstanceBuffer(instanceBuffer);

                std::vector<std::shared_ptr<Material>> newMaterials;
                for (auto& material : renderer->GetMaterials())
                {

                    auto newMaterial = std::make_shared<Material>();
                    newMaterial = material->Clone();
                    if (wstr::contains(ResourceManager::main->GetKey(material->GetShader()), L"Grass"))
                    {
                        newMaterial->SetShader(ResourceManager::main->Get<Shader>(L"Environment_Grass"));
                        newMaterial->SetShadowCasting(false);
                    }
                    else
                        newMaterial->SetShader(ResourceManager::main->Get<Shader>(L"Environment_Instanced"));
                    newMaterial->SetPass(RENDER_PASS::Deferred);
                    newMaterials.push_back(newMaterial);

                };
                renderer->SetMaterials(newMaterials);
            	renderer->SetSpecialMaterials();
                renderer->AddCbufferSetter(GetCast<Terrain>());
            }
        }
    }
    else
    {
        _instanceBuffers.resize(_instancesObject.size());

        for (int i = 0; i < _instancesObject.size(); i++)
        {
            std::cout << "Instance Count (" << to_string(_instancesObject[i].lock()->GetName()) << ") : " << _instanceDatas[i].size() << std::endl;
            auto instanceBuffer = Core::main->GetBufferManager()->GetInstanceBufferPool(BufferType::TransformInstanceParam)->Alloc();
            memcpy(instanceBuffer->ptr, _instanceDatas[i].data(), _instanceDatas[i].size() * sizeof(Instance_Transform));
            instanceBuffer->SetIndex(_instanceDatas[i].size(), sizeof(Instance_Transform));
            _instanceBuffers[i] = instanceBuffer;

            std::vector<std::shared_ptr<MeshRenderer>> renderers;
            _instancesObject[i].lock()->GetComponentsWithChilds<MeshRenderer>(renderers);

            for (auto& renderer : renderers)
            {
                renderer->SetCulling(false);
                renderer->SetInstancing(false);
                renderer->SetInstanceBuffer(instanceBuffer);

                std::vector<std::shared_ptr<Material>> newMaterials;
                for (auto& material : renderer->GetMaterials())
                {
                    auto newMaterial = std::make_shared<Material>();
                    newMaterial = material->Clone();

                    if (material->GetShader()->_name == "DeferredSeaGrass.hlsl")
                    {
                        newMaterial->SetShader(ResourceManager::main->Get<Shader>(L"DeferredSeaGrass"));
                        newMaterial->SetShadowCasting(false);
                        _instancesObject[i].lock()->AddComponent<SeaGrassComponent>();
                        _instancesObject[i].lock()->SetType(GameObjectType::Static);
                    }

                    else
                    {
                        newMaterial->SetShader(ResourceManager::main->Get<Shader>(L"Environment_Instanced"));
                    }
                    newMaterial->SetPass(RENDER_PASS::Deferred);
                    newMaterials.push_back(newMaterial);
                };

                renderer->SetMaterials(newMaterials);
       /*         renderer->SetSpecialMaterials();*/

            }
        }
    }

    TerrainManager::main->PushTerrain(static_pointer_cast<Terrain>(shared_from_this()));

}

void Terrain::Update()
{
    
}

void Terrain::Update2()
{

}

void Terrain::Enable()
{

}

void Terrain::Disable()
{

}

void Terrain::RenderBegin()
{
    if (SceneManager::main->GetCurrentScene()->GetSceneType() != SceneType::Sea01)
    {
        _grassCBuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::GrassParam)->Alloc(1);

        GrassParam grass_param;
        grass_param.objectCount = _objectPositions.size();
        if (_objectPositions.size() > 1)
        std::ranges::sort(_objectPositions, [&](const Vector3& pos1, const Vector3& pos2) {
                return (_objectPositions[0] - pos1).LengthSquared() < (_objectPositions[0] - pos2).LengthSquared();
            });
        for (int i = 0; i < std::min(_objectPositions.size(), grass_param.objectPos.size()); i++) {
            grass_param.objectPos[i] = Vector4(_objectPositions[i].x, _objectPositions[i].y, _objectPositions[i].z, 1);
        }
        memcpy(_grassCBuffer->ptr, &grass_param, sizeof(GrassParam));
        _objectPositions.clear();
    }

    if (auto meshRenderer = GetOwner()->GetComponent<MeshRenderer>())
    {
        SceneManager::main->GetCurrentScene()->AddRenderer(_terrainShadowMaterial.get(), meshRenderer->_mesh[0].get(), meshRenderer.get());
    }
}

void Terrain::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

void Terrain::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
    Component::CollisionEnd(collider, other);

}

bool Terrain::IsExecuteAble()
{
    return Component::IsExecuteAble();
}

void Terrain::SetDestroy()
{

}

void Terrain::Destroy()
{
    if (auto renderer = GetOwner()->GetRenderer())
    {
        renderer->RemoveCbufferSetter(static_pointer_cast<Terrain>(shared_from_this()));
    }
    TerrainManager::main->RemoveTerrain(GetCast<Terrain>());
}

void Terrain::CullingInstancing(Vector3 worldPos, Vector3 look)
{
    Vector3 cameraPos = worldPos;
    Vector3 cameraLook = look;
    cameraLook.Normalize();

    float maxAngle = ((WINDOW_WIDTH / (float)WINDOW_HEIGHT) * (100) * D2R) / 2;
    float cosMaxAngle = std::cos(maxAngle);
    float maxDistance = 30;

    for (int i = 0; i < _instanceBuffers.size(); i++)
        _instanceBuffers[i]->Free();
    _instanceBuffers.clear();

    _instanceBuffers.resize(_instancesObject.size());
    for (int i = 0; i < _instancesObject.size(); i++)
    {
        auto instanceBuffer = Core::main->GetBufferManager()->GetInstanceBufferPool(BufferType::TransformInstanceParam)->Alloc();
        _instanceBuffers[i] = instanceBuffer;

        std::vector<std::shared_ptr<MeshRenderer>> renderers;
        _instancesObject[i].lock()->GetComponentsWithChilds<MeshRenderer>(renderers);
        for (auto& renderer : renderers)
            renderer->SetInstanceBuffer(instanceBuffer);
    }

    for (int i = 0; i < _instanceDatas.size(); ++i)
    {
        _instanceBuffers[i]->Clear();

        for (int j = 0; j < _instanceDatas[i].size(); j++)
        {
            Vector3 targetDirection = _instanceDatas[i][j].worldPosition - cameraPos;
            bool distanceBool = targetDirection.LengthSquared() < (maxDistance * maxDistance);
            targetDirection.Normalize();
            bool angleBool = targetDirection.Dot(cameraLook) > cosMaxAngle;
            if (angleBool || distanceBool)
            {
                _instanceBuffers[i]->AddData(_instanceDatas[i][j]);
            }
        }
    }
}

void Terrain::SetData(Material* material)
{
    material->SetTexture("heightMap", _heightTexture);

    if (SceneManager::main->GetCurrentScene()->GetSceneType() != SceneType::Sea01)
    {
        int index = material->GetShader()->GetRegisterIndex("GrassParam");
        if (index != -1)
            Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, _grassCBuffer->GPUAdress);
    }
}

void Terrain::SetHeightMap(const std::wstring& rawPath, const std::wstring& pngPath, const vec2& rawSize, const vec3& fieldSize)
{
    _heightTexture = ResourceManager::main->Load<Texture>(pngPath, pngPath, TextureType::Texture2D, false);

    _heightTextureSize = vec2(static_cast<int>(_heightTexture->GetResource()->GetDesc().Width),
        static_cast<int>(_heightTexture->GetResource()->GetDesc().Height));

    _heightRawSize = rawSize;
    _fieldSize = fieldSize;

    cout << "Terrain Size : "<< _fieldSize.x << " " << _fieldSize.y << " " << _fieldSize.z << endl;

#ifdef RECT_TERRAIN
    _gridMesh = GeoMetryHelper::LoadGripMeshControlPoints(_fieldSize.x, _fieldSize.z, _fieldSize.x / 10, _fieldSize.y / 10);
    _gridMesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
#else

    _gridMesh = GeoMetryHelper::LoadGripMesh(_fieldSize.x, _fieldSize.z, _fieldSize.x / 10, _fieldSize.z / 10);
    _gridMesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
#endif // RECT_TERRAIN


    LoadTerrain(rawPath);
}


float Terrain::GetHeight(const Vector2& heightMapPosition) const
{
    const float fx = heightMapPosition.x;
    const float fz = heightMapPosition.y;

    if (fx < 0 || fx >= _heightRawSize.x - 1 || fz < 0 || fz >= _heightRawSize.y - 1)
        return 0;

    const int ix = static_cast<int>(fx);
    const int iz = static_cast<int>(fz);

    // 경계를 넘어가는 경우 방지
    int ix1 = std::min(ix + 1, static_cast<int>(_heightRawSize.x - 1));
    int iz1 = std::min(iz + 1, static_cast<int>(_heightRawSize.y - 1));

    float hx0z0 = _heightRawMapData[iz][ix];
    float hx1z0 = _heightRawMapData[iz][ix1];
    float hx0z1 = _heightRawMapData[iz1][ix];
    float hx1z1 = _heightRawMapData[iz1][ix1];

    float tx = fx - ix;
    float tz = fz - iz;

    float h0 = hx0z0 * (1 - tx) + hx1z0 * tx;  // x 방향 보간
    float h1 = hx0z1 * (1 - tx) + hx1z1 * tx;  // x 방향 보간
    float finalHeight = h0 * (1 - tz) + h1 * tz; // z 방향 보간

    return finalHeight;
}

float Terrain::GetLocalHeight(const Vector3& localPosition)
{
    Vector3 terrainOrigin = GetOwner()->_transform->GetLocalPosition();

    float tempX = localPosition.x - terrainOrigin.x;
    float tempZ = localPosition.z - terrainOrigin.z;

    float finalH = GetHeight(Vector2((tempX / _fieldSize.x) * _heightRawSize.x, (tempZ / _fieldSize.z) * _heightRawSize.y));
    return terrainOrigin.y + finalH;
}
float Terrain::GetWorldHeight(const Vector3& worldPosition)
{
    Matrix matrix;
    Matrix invMatrix;
    GetOwner()->_transform->GetLocalToWorldMatrix_BottomUp(matrix);
    matrix.Invert(invMatrix);
    Vector3 localPosition;
    Vector3::Transform(worldPosition, invMatrix, localPosition);
    float tempX = localPosition.x;
    float tempZ = localPosition.z;
    float finalH = GetHeight(Vector2((tempX / _fieldSize.x) * _heightRawSize.x, (tempZ / _fieldSize.z) * _heightRawSize.y));

    return matrix.Translation().y + finalH;
}

bool Terrain::RayCast(const Ray& ray, const float& dis, RayHit& hit)
{
    vec3 offsetPos = ray.position - GetOwner()->_transform->GetWorldPosition();
    vec3 dir = ray.direction;
    dir.Normalize();

    float prevH = offsetPos.y;
    float between = 1.0f;
    float reusltDis = 0;
    float reusltHeight = 0;


    bool isHit = false;
    while (true)
    {
        Vector3 currentPos = offsetPos + dir * reusltDis;
        float HeightMap = GetLocalHeight(currentPos);
        float currentH = currentPos.y - HeightMap;
        if ((prevH > 0 && currentH <= 0) || (prevH < 0 && currentH >= 0))
        {
            float left = reusltDis - between;
            float right = reusltDis;

            float leftH = prevH;
            float rightH = currentH;
            isHit = true;

            float middle = 0;
            float middleH = 0;

            for (int j = 0; j < 12; j++)
            {
                middle = (left + right) / 2;
                currentPos = offsetPos + dir * middle;
                middleH = currentPos.y - GetLocalHeight(currentPos);
                if (rightH == 0) {
                    middle = right;
                    middleH = rightH;
                    break;
                }
                if (leftH == 0) {
                    middle = left;
                    middleH = leftH;
                    break;
                }
                if (middleH * leftH < 0)
                {
                    rightH = middleH;
                    right = middle;
                }
                if (middleH * rightH < 0)
                {
                    leftH = middleH;
                    left = middle;
                }
            }
            reusltDis = middle;
            reusltHeight = middleH + currentPos.y;
            break;
        }

        prevH = currentH;

        if (reusltDis > dis)
            break;
        reusltDis += between;
    }
    if (reusltDis > dis)
        isHit = false;

    if (isHit)
    {
        Vector3 localPos = offsetPos + dir * reusltDis;
        localPos.y = reusltHeight;

        hit.distance = reusltDis;
        hit.worldPos = localPos + GetOwner()->_transform->GetWorldPosition();
        hit.gameObject = GetOwner();
        hit.collider = nullptr;
        hit.isHit = true;

        Vector3 forward = localPos + Vector3::Forward * 0.2f;
        forward.y = GetLocalHeight(forward);
        Vector3 right = localPos + Vector3::Right * 0.2f;
        right.y = GetLocalHeight(right);
        forward = forward - localPos;
        forward.Normalize();
        right = right - localPos;
        right.Normalize();

        hit.normal = (forward).Cross(right);
        hit.normal.Normalize();
        //hit.
        return true;
    }
    return false;
}


void Terrain::LoadTerrain(const std::wstring& rawData)
{
    _heightRawMapData.resize(_heightRawSize.y, vector(_heightRawSize.x, 0.0f));

    std::ifstream file(rawData, std::ios::binary);
    if (!file) {
        std::wcerr << L"Failed to open raw file: " << rawData << std::endl;
        return;
    }

    std::vector<uint16_t> tempRow(static_cast<int>(_heightRawSize.x));

    for (int z = 0; z < _heightRawSize.y; ++z)
    {
        if (!file.read(reinterpret_cast<char*>(tempRow.data()), _heightRawSize.x * sizeof(uint16_t)))
        {
            std::wcerr << L"Failed to read raw data from file: " << rawData << std::endl;
            return;
        }

        for (int x = 0; x < _heightRawSize.x; ++x) {
            _heightRawMapData[z][x] = static_cast<float>(tempRow[x]) / 65535.0f * _fieldSize.y;
        }
    }

    file.close();
}





