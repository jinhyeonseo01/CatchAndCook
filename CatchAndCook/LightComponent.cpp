#include "pch.h"
#include "LightComponent.h"

#include "LightManager.h"
#include "Transform.h"
#include "MeshRenderer.h"

std::weak_ptr<LightComponent> LightComponent::mainLight;


LightComponent::~LightComponent()
{
}

bool LightComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void LightComponent::Init()
{
	Component::Init();

	light = std::make_shared<Light>();

	light->material.ambient = vec3(0.2f, 0.2f, 0.2f);
	light->material.diffuse = vec3(1.0f, 1.0f, 1.0f);
	light->material.specular = vec3(1.0f, 1.0f, 1.0f);
	light->material.shininess = 61.0f;
	//light->material.lightType = static_cast<int32>(LIGHT_TYPE::POINT_LIGHT);
	//light->strength = vec3(1.0f, 1.0f, 1.0f);
}

void LightComponent::Start()
{
	Component::Start();

	light->material.lightType = static_cast<int32>(type);
	light->intensity = intensity;
	light->strength = color;
	light->fallOffStart = 0;
	light->fallOffEnd = range;
	light->spotAngle = spotAngle * D2R;
	light->innerSpotAngle = innerSpotAngle * D2R;
	light->direction = GetOwner()->_transform->GetForward();
	light->position = GetOwner()->_transform->GetWorldPosition();


	if (light->material.lightType == 0)
		mainLight = GetCast<LightComponent>();
}

void LightComponent::Update()
{
	Component::Update();
}

void LightComponent::Update2()
{
	Component::Update2();


	if (light->material.lightType == 0 && abs(InGameGlobal::main->skyTime - skyTime) > 0.001)
	{
		skyTime += (InGameGlobal::main->skyTime - skyTime) * Time::main->GetDeltaTime() * 5;
		if (abs(InGameGlobal::main->skyTime - skyTime) <= 0.001)
			skyTime = InGameGlobal::main->skyTime;

		if (auto sky = SceneManager::main->GetCurrentScene()->Find(L"Skybox"))
		{
			SceneManager::main->GetCurrentScene()->GetGlobalParam().SkyBlend = skyTime;
			float cycle = skyTime;
			float tY = std::fmod(cycle + 2.0f, 4.0f) / 4.0f;
			float angleY = std::lerp(67.0f, -67.0f, tY);


			Quaternion qy = Quaternion::CreateFromAxisAngle(Vector3::UnitY, angleY * D2R);
			GetOwner()->_transform->SetLocalRotation(qy * Quaternion::CreateFromYawPitchRoll(Vector3(55, -115, 0) * D2R));
			light->intensity = cos((std::fmod(skyTime, 4)) / 2 * 3.141592f) * 0.5 + 0.5;
			light->intensity = pow(light->intensity, 0.5);
			if (auto renderer = sky->GetComponent<MeshRenderer>())
			{
				if (auto material = renderer->GetMaterial())
				{
					switch ((int)std::fmod(skyTime, 4))
					{
						case 0:
							{
							material->SetTexture("_BaseMap", ResourceManager::main->_cubemap_skyTexture);
							material->SetTexture("_BaseMap_1", ResourceManager::main->_cubemap_skyETexture);

							ResourceManager::main->_bakedGIFinal1Texture = ResourceManager::main->_bakedGITexture;
							ResourceManager::main->_bakedGIFinal2Texture = ResourceManager::main->_bakedGIETexture;
							break;
							}
						case 1:
						{
							material->SetTexture("_BaseMap", ResourceManager::main->_cubemap_skyETexture);
							material->SetTexture("_BaseMap_1", ResourceManager::main->_cubemap_skyNTexture);

							ResourceManager::main->_bakedGIFinal1Texture = ResourceManager::main->_bakedGIETexture;
							ResourceManager::main->_bakedGIFinal2Texture = ResourceManager::main->_bakedGINTexture;
							break;
						}
						case 2:
						{
							material->SetTexture("_BaseMap", ResourceManager::main->_cubemap_skyNTexture);
							material->SetTexture("_BaseMap_1", ResourceManager::main->_cubemap_skyTexture);
							ResourceManager::main->_bakedGIFinal1Texture = ResourceManager::main->_bakedGINTexture;
							ResourceManager::main->_bakedGIFinal2Texture = ResourceManager::main->_bakedGITexture;
							break;
						}
						case 3:
						{
							material->SetTexture("_BaseMap", ResourceManager::main->_cubemap_skyTexture);
							material->SetTexture("_BaseMap_1", ResourceManager::main->_cubemap_skyTexture);
							ResourceManager::main->_bakedGIFinal1Texture = ResourceManager::main->_bakedGITexture;
							ResourceManager::main->_bakedGIFinal2Texture = ResourceManager::main->_bakedGITexture;
							break;
						}
					}
				}
			}
		}
	}
	light->direction = GetOwner()->_transform->GetForward();
	light->position = GetOwner()->_transform->GetWorldPosition();

	if (Input::main->GetKey(KeyCode::LeftBracket)) {
		GetOwner()->_transform->SetLocalRotation(GetOwner()->_transform->GetLocalRotation() * Quaternion::CreateFromAxisAngle(Vector3::Up, 1.5 * Time::main->GetDeltaTime()));
	}
	if (Input::main->GetKey(KeyCode::RightBracket)) {
		GetOwner()->_transform->SetLocalRotation(GetOwner()->_transform->GetLocalRotation() * Quaternion::CreateFromAxisAngle(Vector3::Up, -1.5 * Time::main->GetDeltaTime()));
	}
}

void LightComponent::Enable()
{
	Component::Enable();
	light->onOff = 1;

	LightManager::main->PushLight(light);
}

void LightComponent::Disable()
{
	Component::Disable();
	light->onOff = 0;
	LightManager::main->RemoveLight(light);
	if (LightManager::main->GetMainLight() == light)
		LightManager::main->_mainLights = nullptr;
}

void LightComponent::RenderBegin()
{
	Component::RenderBegin();

}

void LightComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void LightComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void LightComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void LightComponent::SetDestroy()
{
	Component::SetDestroy();
}

void LightComponent::Destroy()
{
	Component::Destroy();
}

void LightComponent::RenderEnd()
{
	Component::RenderEnd();
}

void LightComponent::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);

	if (nextScene == GetOwner()->GetScene())
		LightManager::main->PushLight(light);
	else
		LightManager::main->RemoveLight(light);
}
