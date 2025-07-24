#include "pch.h"
#include "BufferManager.h"
#include "Bone.h"
#include "BufferPool.h"
#include "Material.h"
#include "Camera.h"
#include "InstancingManager.h"
#include "Scene.h"
#include "Sprite.h"
#include "testComponent.h"
#include "LightManager.h"
#include "WaterController.h"
#include "ComputeManager.h"
#include "ObjectSettingComponent.h"
#include "PlantComponent.h"
#include "PathFinder.h"
#include "Terrain.h"
#include "SeaGrassComponent.h"
#include "ShadowManager.h"
#include "ParticleComponent.h"
#include "ProgressCycleComponent.h"

std::string BufferTypeToString(BufferType type)
{
	switch (type)
	{
	case BufferType::GlobalParam: return "GlobalParam";
	case BufferType::TransformParam: return "TransformParam";
	case BufferType::RectTransformParam: return "RectTransformParam";
	case BufferType::MateriaParam: return "MateriaParam";
	case BufferType::BoneParam: return "BoneParam";
	case BufferType::CameraParam: return "CameraParam";
	case BufferType::MateriaSubParam: return "MateriaSubParam";
	case BufferType::SpriteWorldParam: return "SpriteWorldParam";
	case BufferType::GUISpriteParam: return "GUISpriteParam";
	case BufferType::SpriteTextureParam: return "SpriteTextureParam";
	case BufferType::TestParam: return "TestParam";
	case BufferType::DefaultMaterialParam: return "DefaultMaterialParam";
	case BufferType::SeaDefaultMaterialParam: return "SeaDefaultMaterialParam";
	case BufferType::EnvMaterialParam: return "EnvMaterialParam";
	case BufferType::LightHelperParam: return "LightHelperParam";
	case BufferType::TerrainDetailsParam: return "TerrainDetailsParam";
	case BufferType::SeaParam: return "SeaParam";
	case BufferType::FogParam: return "FogParam";
	case BufferType::UnderWaterParam: return "UnderWaterParam";
	case BufferType::SeaPlantParam: return "SeaPlantParam";
	case BufferType::SeaFIshParam: return "SeaFIshParam";
	case BufferType::SeaGrassParam: return "SeaGrassParam";
	case BufferType::ScatteringData: return "ScatteringData";
	case BufferType::VolumetricData: return "VolumetricData";
	case BufferType::HookData: return "HookData";
	case BufferType::HPData: return "HPData";
	case BufferType::ParicleHelperParams: return "ParicleHelperParams";
	case BufferType::ProgressCircle: return "ProgressCircle";
	case BufferType::ChangeSceneData: return "ChangeSceneData";
	case BufferType::PlayerMaterialParam: return "PlayerMaterialParam";
	case BufferType::InstanceOffsetParam: return "InstanceOffsetParam";
	case BufferType::TransformInstanceParam: return "TransformInstanceParam";
	case BufferType::GizmoInstanceParam: return "GizmoInstanceParam";
	case BufferType::GizmoTextInstanceParam: return "GizmoTextInstanceParam";
	case BufferType::ForwardLightParam: return "ForwardLightParam";
	case BufferType::VignetteParam: return "VignetteParam";
	case BufferType::ObjectMaterialParam: return "ObjectMaterialParam";
	case BufferType::ObjectSettingParam: return "ObjectSettingParam";
	case BufferType::LightDataParam: return "LightDataParam";
	case BufferType::GrassParam: return "GrassParam";
	case BufferType::WaterParam: return "WaterParam";
	case BufferType::ShadowCasterParams: return "ShadowCasterParams";
	case BufferType::ShadowCascadeIndexParams: return "ShadowCascadeIndexParams";
	case BufferType::GodRayParam: return "GodRayParam";
	case BufferType::FXAAParams: return "FXAAParams";
	case BufferType::DOFParam: return "DOFParam";
	default: return "Unknown";
	}
}

void BufferManager::Init()
{

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(2000,10,5);

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_table[i] = make_shared<DescritporTable>();
		_table[i]->Init(100000);
	}

	for(int i=0; i<MAX_FRAME_COUNT; ++i)
	{
		CreateBufferPool(i, BufferType::GlobalParam,sizeof(GlobalParam),1);
		CreateBufferPool(i, BufferType::TransformParam,sizeof(TransformParam),1000);
		CreateBufferPool(i, BufferType::RectTransformParam, sizeof(RectTransformParam), 1000);
		CreateBufferPool(i, BufferType::MateriaSubParam,sizeof(TestSubMaterialParam),1000);
		CreateBufferPool(i, BufferType::CameraParam,sizeof(CameraParams),128);
		CreateBufferPool(i, BufferType::SpriteTextureParam,sizeof(SprtieTextureParam),255);
		CreateBufferPool(i, BufferType::SpriteWorldParam,sizeof(SpriteWorldParam),255);
		CreateBufferPool(i, BufferType::GUISpriteParam, sizeof(GUISpriteParam), 1000);
		CreateBufferPool(i, BufferType::LightHelperParam,sizeof(LightHelperParams),1);
		CreateBufferPool(i, BufferType::BoneParam,sizeof(BoneParam),1000);
		CreateBufferPool(i, BufferType::SeaParam, sizeof(TerrainDetailsParam), 5);
		CreateBufferPool(i, BufferType::FogParam, sizeof(FogParam), 5);
		CreateBufferPool(i, BufferType::UnderWaterParam, sizeof(UnderWaterParam), 5);
		CreateBufferPool(i, BufferType::InstanceOffsetParam, sizeof(InstanceOffsetParam), 10000);
		CreateBufferPool(i, BufferType::GrassParam, sizeof(GrassParam), 32);
		CreateBufferPool(i, BufferType::SeaGrassParam, sizeof(SeaGrassParam), 10000);
		CreateBufferPool(i, BufferType::ShadowCasterParams, sizeof(ShadowCasterParams), 5);
		CreateBufferPool(i, BufferType::ShadowCascadeIndexParams, sizeof(ShadowCascadeIndexParams), 10);
		CreateBufferPool(i, BufferType::ParicleHelperParams, sizeof(ParicleHelperParams), 100);
		CreateBufferPool(i, BufferType::ProgressCircle, sizeof(ProgressCycleData), 1);

	}

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		CreateStructuredBufferPool(i, BufferType::TransformParam,"TransformDatas",sizeof(Instance_Transform),100000);
		CreateStructuredBufferPool(i, BufferType::SeaPlantParam, "PlantInfos", sizeof(PlantInfo), 20000);
	/*	CreateStructuredBufferPool(i, BufferType::ForwardLightParam, "ForwardLightDatas", sizeof(ForwardLightParams), 10000);*/
		CreateStructuredBufferPool(i, BufferType::ObjectSettingParam, "ObjectSettingDatas", sizeof(ObjectSettingParam), 5000);
		CreateStructuredBufferPool(i, BufferType::SeaFIshParam, "FIshInfos", sizeof(FishInfo), 20000);
	/*	CreateStructuredBufferPool(BufferType::ForwardLightParam, "ForwardLightDatas", sizeof(ForwardLightParams), 10000);*/
		CreateStructuredBufferPool(i, BufferType::LightDataParam, "g_lights", sizeof(Light), LightManager::_maxLight);
		CreateStructuredBufferPool(i, BufferType::BoneParam, "BoneDatas", sizeof(Matrix), 2 * 256 * 1024);
		CreateStructuredBufferPool(i, BufferType::HPData, "HPData", sizeof(float)*4, 300);
	}

	for(int i=0; i < MAX_FRAME_COUNT; ++i)
	{
		CreateInstanceBufferPool(i, BufferType::TransformInstanceParam, sizeof(Instance_Transform), 10000, 40);
		CreateInstanceBufferPool(i, BufferType::GizmoInstanceParam, sizeof(Instance_Gizmo), 250000, 1);
	}
	


}

void BufferManager::Reset()
{

	static float time = 0;

	time += Time::main->GetDeltaTime();

	if (time > 5.0f)
	{
		Debug();
		time = 0;
	}

	_table[CURRENT_CONTEXT_INDEX]->Reset();

	{
		auto& map = _map[CURRENT_CONTEXT_INDEX];

		for (auto& ele : map)
		{
			ele.second->Reset();
		}
	}

	{
		auto& map = _structuredMap[CURRENT_CONTEXT_INDEX];

		for (auto& ele : map) {
			ele.second->Clear();
		}
	}
}

void BufferManager::Debug()
{

	static unordered_set<SceneType> _show;

	auto& sceneType =  SceneManager::main->GetCurrentScene()->GetSceneType();

	if (_show.find(sceneType) == _show.end())
	{
		_show.insert(sceneType);

		cout << "============================현재씬" << (int)SceneManager::main->GetCurrentScene()->GetSceneType() << "===================================" << endl;

		cout << "\n===================== [Table Info] =====================\n";
		for (int i = 0; i < MAX_FRAME_COUNT; ++i)
		{
			cout << "Table Allocated Count: " << _table[i]->GetCount() << endl;
		}
		cout << "\n================= [Texture Buffer Pool] =================\n";
		_textureBufferPool->PrintCount();
		cout << "\n=================== [General BufferMap] =================\n";
		cout << left << setw(30) << "Buffer Type" << "Count\n";
		cout << string(45, '-') << "\n";
		for (int i = 0; i < MAX_FRAME_COUNT; ++i)
		{
			cout << "현재 FRAME INDEX " << i << endl;
			for (auto& ele : _map[i])
			{
				cout << left << setw(30) << BufferTypeToString(ele.first)
					<< ele.second->GetCount() << "\n";
			}
		}
		cout << "\n=================== [Instance BufferMap] ================\n";
		for (int i = 0; i < MAX_FRAME_COUNT; ++i)
		{
			cout << "현재 FRAME INDEX " << i << endl;

			for (auto& ele : _instanceMap[i])
			{
				cout << "[Buffer Type] " << BufferTypeToString(ele.first) << "\n";
				ele.second->PrintCount();
				cout << string(45, '-') << "\n";
			}
		}

		cout << "\n=============== [Structured Buffer Pool] ===============\n";
		cout << left << setw(30) << "Buffer Type" << "Count\n";
		cout << string(45, '-') << "\n";
		for (int i = 0; i < MAX_FRAME_COUNT; ++i)
		{
			cout << "현재 FRAME INDEX " << i << endl;
			for (auto& ele : _structuredMap[i])
			{
				cout << left << setw(30) << BufferTypeToString(ele.first)
					<< ele.second->GetCount() << "\n";
			}
		}

		cout << "\n==================== [End of Debug] =====================\n\n";
	}


	
}

void BufferManager::CreateBufferPool(uint32 index,BufferType type, uint32 size, uint32 count)
{
	shared_ptr<CBufferPool> transformBuffer = make_shared<CBufferPool>();
	transformBuffer->Init(size, count);
	_map[index][type] = transformBuffer;
}

void BufferManager::CreateBufferPool_Static(BufferType type, uint32 size, uint32 count)
{
	//cout << "CreateBufferPool_Static" << endl;
	//shared_ptr<CBufferPool> transformBuffer = make_shared<CBufferPool>();
	//transformBuffer->Init(size, count);
	//_map_notReset[type] = transformBuffer;
}

void BufferManager::CreateInstanceBufferPool(uint32 index ,BufferType type,uint32 elementSize,uint32 elementCount,uint32 bufferCount)
{
	shared_ptr<InstanceBufferPool> transformBuffer = make_shared<InstanceBufferPool>();
	transformBuffer->Init(elementSize,elementCount,bufferCount);
	_instanceMap[index][type] = transformBuffer;
}

void BufferManager::CreateStructuredBufferPool(uint32 index, BufferType type,const string& name,uint32 elementSize, uint32 elementCount)
{
	shared_ptr<StructuredBuffer> strBuffer = make_shared<StructuredBuffer>();
	strBuffer->Init(elementSize, elementCount);
	_structuredMap[index][type] = strBuffer;

	if (_structuredNameMappingTable.find(name) == _structuredNameMappingTable.end())
	{
		_structuredNameMappingTable[name] = type;
	}

}


shared_ptr<CBufferPool>& BufferManager::CreateAndGetBufferPool(BufferType type, uint32 size, uint32 count)
{
	if(_map[CURRENT_CONTEXT_INDEX].contains(type))
		return _map[CURRENT_CONTEXT_INDEX][type];

	shared_ptr<CBufferPool> transformBuffer = make_shared<CBufferPool>();
	transformBuffer->Init(size,count);
	_map[CURRENT_CONTEXT_INDEX][type] = transformBuffer;
	return _map[CURRENT_CONTEXT_INDEX][type];
}

shared_ptr<CBufferPool>& BufferManager::CreateAndGetBufferPool_Static(BufferType type, uint32 size, uint32 count)
{
	if(_map_notReset.contains(type))
		return _map_notReset[type];
	shared_ptr<CBufferPool> transformBuffer = make_shared<CBufferPool>();
	transformBuffer->Init(size,count);
	_map_notReset[type] = transformBuffer;
	return _map_notReset[type];
}

