#include "pch.h"
#include "LightManager.h"
#include "Core.h"
#include "BufferPool.h"
#include "CameraManager.h"
#include "Transform.h"
#include "Camera.h"
#include "ImguiManager.h"

unique_ptr<LightManager> LightManager::main ;

void LightManager::Init()
{

}

void LightManager::PushLight(const std::shared_ptr<Light>& light, const SceneType& sceneType)
{
	vector<std::shared_ptr<Light>>& lightVec = _lightsSceneCollector[sceneType];

	if (std::ranges::find(lightVec, light) == lightVec.end())
		lightVec.push_back(light);
}

void LightManager::RemoveLight(const std::shared_ptr<Light>& light, const SceneType& sceneType)
{
	vector<std::shared_ptr<Light>>& lightVec = _lightsSceneCollector[sceneType];

	auto it = std::ranges::find(lightVec, light);
	if (it != lightVec.end())
		lightVec.erase(it);
}

void LightManager::SetData()
{
	_strBuffer = Core::main->GetBufferManager()->GetStructuredBufferPool(BufferType::LightDataParam);
	_strBuffer->Clear();
	Update();

	CBufferContainer* container = Core::main->GetBufferManager()->GetBufferPool(BufferType::LightHelperParam)->Alloc(1);
	memcpy(container->ptr, (void*)&_lightParmas, sizeof(LightHelperParams));

	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(3, container->GPUAdress);
	Core::main->GetCmdList()->SetComputeRootConstantBufferView(3, container->GPUAdress);

	TableContainer tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(1);
	Core::main->GetBufferManager()->GetTable()->CopyHandle(tableContainer.CPUHandle, _strBuffer->GetSRVHandle(), 0);
	Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_LIGHTPARM_TABLE_INDEX, tableContainer.GPUHandle);
	Core::main->GetCmdList()->SetComputeRootDescriptorTable(11, tableContainer.GPUHandle);
}

void LightManager::Clear()
{
	_lightsSceneCollector.clear();
}

std::shared_ptr<Light> LightManager::GetMainLight()
{

	vector<std::shared_ptr<Light>>& _lights = _lightsSceneCollector[SceneManager::main->GetCurrentScene()->GetSceneType()];

	for (auto& light : _lights)
	{
		if (light->material.lightType == 0)
		{
			_mainLights = light;
		}
	}

	return _mainLights;
}

std::vector<std::shared_ptr<Light>>& LightManager::GetLights()
{
	vector<std::shared_ptr<Light>>& _lights = _lightsSceneCollector[SceneManager::main->GetCurrentScene()->GetSceneType()];

	return _lights;
}


void LightManager::Update()
{

	vector<std::shared_ptr<Light>>& _lights = _lightsSceneCollector[SceneManager::main->GetCurrentScene()->GetSceneType()];

	_lightParmas.eyeWorldPos = CameraManager::main->GetActiveCamera()->GetCameraPos();
	_lightParmas.lightCount = _lights.size();
	_lightParmas.mainLight = *GetMainLight().get();

	for (auto& ele : _lights)
	{
		if (ele) 
		{
			if(ele->onOff==1)
				_strBuffer->AddData(*ele); 
		}
	}
}

//
//void ForwardLightSetter::Init(GameObject* object)
//{
//	this->object = object;
//}
//
//void ForwardLightSetter::SetData(StructuredBuffer* buffer, Material* material)
//{
//	std::vector<std::shared_ptr<Light>> _lightForwards;
//	ForwardLightParams params;
//	Vector3 worldPos = this->object->_transform->GetWorldPosition();
//
//	_lightForwards.reserve(LightManager::main->_lights.size());
//	_lightForwards.insert(_lightForwards.end(), LightManager::main->_lights.begin(), LightManager::main->_lights.end());
//	std::ranges::sort(_lightForwards, [&](const std::shared_ptr<Light>& light1, const std::shared_ptr<Light>& light2) {
//		return (light1->position - worldPos).LengthSquared() < (light2->position - worldPos).LengthSquared();
//		});
//
//	for (int i = 0; i < std::min(5, static_cast<int>(_lightForwards.size())); i++)
//	{
//		auto& light = _lightForwards[i];
//		params.lights[params.lightCount] = *light.get();
//		params.lightCount++;
//	}
//
//	buffer->AddData(params);
//}
//
//
//
//ForwardLightSetter::~ForwardLightSetter()
//{
//
//}
