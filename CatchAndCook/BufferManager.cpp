﻿#include "pch.h"
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
void BufferManager::Init()
{

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(2000,10,5);

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_table[i] = make_shared<DescritporTable>();
		_table[i]->Init(60000);
	}

	for(int i=0; i<MAX_FRAME_COUNT; ++i)
	{
		CreateBufferPool(i,BufferType::GlobalParam,sizeof(GlobalParam),1);
		CreateBufferPool(i,BufferType::TransformParam,sizeof(TransformParam),1000);
		CreateBufferPool(i,BufferType::MateriaParam,sizeof(MaterialParams),1000);
		CreateBufferPool(i,BufferType::MateriaSubParam,sizeof(TestSubMaterialParam),1000);
		CreateBufferPool(i,BufferType::CameraParam,sizeof(CameraParams),128);
		CreateBufferPool(i,BufferType::SpriteTextureParam,sizeof(SprtieTextureParam),255);
		CreateBufferPool(i,BufferType::SpriteWorldParam,sizeof(SpriteWorldParam),255);
		CreateBufferPool(i,BufferType::LightParam,sizeof(LightParams),1);
		CreateBufferPool(i,BufferType::BoneParam,sizeof(BoneParam),256);
		CreateBufferPool(i,BufferType::SeaParam, sizeof(TerrainDetailsParam), 5);
		CreateBufferPool(i,BufferType::FogParam, sizeof(FogParam), 5);
		CreateBufferPool(i,BufferType::UnderWaterParam, sizeof(UnderWaterParam), 5);
		CreateBufferPool(i,BufferType::InstanceOffsetParam, sizeof(InstanceOffsetParam), 10000);

	}
	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		CreateStructuredBufferPool(i, BufferType::TransformParam,"TransformDatas",sizeof(Instance_Transform),20000);
		CreateStructuredBufferPool(i, BufferType::SeaPlantParam, "PlantInfos", sizeof(PlantInfo), 20000);
	/*	CreateStructuredBufferPool(i, BufferType::ForwardLightParam, "ForwardLightDatas", sizeof(ForwardLightParams), 10000);*/
		CreateStructuredBufferPool(i, BufferType::ObjectSettingParam, "ObjectSettingDatas", sizeof(ObjectSettingParam), 5000);
	}

	for(int i=0; i < MAX_FRAME_COUNT; ++i)
	{
		CreateInstanceBufferPool(i, BufferType::TransformInstanceParam, sizeof(Instance_Transform), 10000, 128);
		CreateInstanceBufferPool(i, BufferType::GizmoInstanceParam, sizeof(Instance_Gizmo), 100000, 1);
	}
	


}

void BufferManager::Reset()
{
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

