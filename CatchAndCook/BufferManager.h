#pragma once
#include "StructuredBuffer.h"


class InstanceBufferPool;

enum class BufferType
{
	GlobalParam,
	TransformParam,
	RectTransformParam,
	MateriaParam,
	BoneParam,
	CameraParam,
	MateriaSubParam,
	SpriteWorldParam,
	GUISpriteParam,
	SpriteTextureParam,
	TestParam,
	DefaultMaterialParam,
	SeaDefaultMaterialParam,
	EnvMaterialParam,
	LightHelperParam,
	TerrainDetailsParam,
	SeaParam,
	FogParam,
	UnderWaterParam,
	SeaPlantParam,
	SeaFIshParam,
	SeaGrassParam,
	ScatteringData,
	VolumetricData,
	HookData,
	HPData,
	ParicleHelperParams,
	ProgressCircle,


	PlayerMaterialParam,
	InstanceOffsetParam,
	// Instance
	TransformInstanceParam,
	GizmoInstanceParam,
	GizmoTextInstanceParam,
	ForwardLightParam,
	VignetteParam,


	ObjectMaterialParam,
	ObjectSettingParam,
	LightDataParam,
	GrassParam,
	WaterParam,
	ShadowCasterParams,
	ShadowCascadeIndexParams,
	GodRayParam,
	FXAAParams,
	DOFParam,
};

class CBufferPool;
class DescritporTable;
class TextureBufferPool;

class BufferManager
{

public:
	void Init();
	void Reset();

	void CreateBufferPool(uint32 index , BufferType type, uint32 size, uint32 count);
	void CreateBufferPool_Static(BufferType type, uint32 size, uint32 count);

	shared_ptr<CBufferPool>& GetBufferPool(BufferType type) { return _map[CURRENT_CONTEXT_INDEX][type]; }
	shared_ptr<CBufferPool>& GetBufferPool_Static(BufferType type) { return _map_notReset[type]; }
	shared_ptr<CBufferPool>& CreateAndGetBufferPool(BufferType type,uint32 size,uint32 count);
	shared_ptr<CBufferPool>& CreateAndGetBufferPool_Static(BufferType type, uint32 size, uint32 count);

	void CreateInstanceBufferPool(uint32 index,BufferType type, uint32 elementSize, uint32 elementCount, uint32 bufferCount);
	shared_ptr<InstanceBufferPool>& GetInstanceBufferPool(BufferType type) {return _instanceMap[CURRENT_CONTEXT_INDEX][type];}

	void CreateStructuredBufferPool(uint32 index, BufferType type, const string& name, uint32 elementSize, uint32 elementCount);

	shared_ptr<StructuredBuffer>& GetStructuredBufferPool(BufferType type) 
	{
		return _structuredMap[CURRENT_CONTEXT_INDEX][type];
	}

	BufferType& GetStructuredNameToBufferType(const string& name){
		return _structuredNameMappingTable[name];
	}

	shared_ptr<TextureBufferPool>& GetTextureBufferPool() { return _textureBufferPool; }
	shared_ptr<DescritporTable>& GetTable() { return _table[CURRENT_CONTEXT_INDEX]; }

private:
	array<shared_ptr<DescritporTable>, MAX_FRAME_COUNT> _table;
	array<std::unordered_map<BufferType, std::shared_ptr<CBufferPool>>, MAX_FRAME_COUNT> _map;
	array<std::unordered_map<BufferType, std::shared_ptr<InstanceBufferPool>>, MAX_FRAME_COUNT> _instanceMap;
	array<std::unordered_map<BufferType, std::shared_ptr<StructuredBuffer>>, MAX_FRAME_COUNT>_structuredMap;

	unordered_map<string, BufferType> _structuredNameMappingTable;
	unordered_map<BufferType, shared_ptr<CBufferPool>> _map_notReset;

	shared_ptr<TextureBufferPool> _textureBufferPool;

};

