#pragma once

#include "ICBufferInjector.h"
#include "Material.h"

#define p1(n) float padding##n;
#define p2(n) float padding##n[2];
#define p3(n) float padding##n[3];

struct alignas(16) TransformParam
{
	Matrix localToWorld;
	Matrix worldToLocal;
	Vector3 worldPos; p1(0);
};

struct alignas(16) RectTransformParam
{
	Matrix localToWorld;
	Matrix worldToLocal;
	Vector3 worldPos; p1(0);
	Matrix normalizeToLocal;
	Matrix overlayMatrix;
};

struct alignas(16) GUISpriteParam
{
	Vector4 GUISprite_offsetSize;
	Vector4 GUISprite_border;      // ← 새 필드
	Vector2 GUISprite_targetSize;
	p2(0);
};

struct BoneParam
{
	Matrix boneMatrixs[256];
	Matrix boneInvertMatrixs[256];
};

struct alignas(16) TestSubMaterialParam
{
	vec2 uv;
	p2(0);
};


struct alignas(16) DefaultMaterialParam
{
	Vector4 color;
	Vector4 _baseMapST;
};

CBUFFER_INJECTOR("DefaultMaterialParam", DefaultMaterialParam, 1000, BufferType::DefaultMaterialParam, std::shared_ptr<Material>,
	// data <- source
	data.color = Vector4(source->GetPropertyVector("_Color"));
	data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
)


//환경 쉐이더
struct alignas(16) EnvMaterialParam
{
	Vector4 color;
	Vector4 _baseMapST;
	Vector4 emissionColor = Vector4(0, 0, 0, 0);
	float emission = 0;
	float _smoothness = 0.2;
	float _metallic = 0.1;
	float padding;
};

CBUFFER_INJECTOR("EnvMaterialParam", EnvMaterialParam, 8000, BufferType::EnvMaterialParam, std::shared_ptr<Material>,
	// data <- source
	data.color = Vector4(source->GetPropertyVector("_Color"));
	data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
	data.emissionColor = Vector4(source->GetPropertyVector("_EmissionColor"));
	data.emission = source->GetPropertyFloat("_Emission");
	data._metallic = source->GetPropertyFloat("_Metallic");
	data._smoothness = source->GetPropertyFloat("_Smoothness");
)

struct alignas(16) PlayerMaterialParam
{
	Vector4 temp;
};

CBUFFER_INJECTOR("PlayerMaterialParam", PlayerMaterialParam, 2560, BufferType::PlayerMaterialParam, std::shared_ptr<Material>,
	data.temp = Vector4(1, 0, 0, 1);
	// data <- source
	//data.color = Vector4(source->GetPropertyVector("_Color"));
	//data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
)



struct ObjectMaterialParam
{
	unsigned int o_select = 0;
	Vector3 o_selectColor;
	unsigned int o_hit = 0;
	unsigned int o_hitValue = 0;
	Vector2 padding;
	Vector3 o_hitColor;
};

CBUFFER_INJECTOR("ObjectMaterialParam", ObjectMaterialParam, 1024, BufferType::ObjectMaterialParam, std::shared_ptr<Material>,
	//data.temp = Vector4(1, 0, 0, 1);
	// data <- source
	//data.color = Vector4(source->GetPropertyVector("_Color"));
	//data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
)

struct WaterParam
{
	Vector3 waterScale = Vector3(1, 1, 1);
	float padding;
};

CBUFFER_INJECTOR("WaterParam", WaterParam, 32, BufferType::WaterParam, std::shared_ptr<Material>,
	//data.temp = Vector4(1, 0, 0, 1);
	// data <- source
	data.waterScale = Vector3(source->GetPropertyVector("_waterScale"));
	//data.color = Vector4(source->GetPropertyVector("_Color"));
	//data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
)

struct alignas(16) TerrainDetailsParam
{
	Vector3 fieldSize; p1(0);
	int detailsCount; p3(1);
	int blendCount; p3(2);
	Vector4 tileST[12];
	Vector4 textureActive[12];
	//Vector4 color;
};

CBUFFER_INJECTOR("TerrainDetailsParam", TerrainDetailsParam, 256, BufferType::TerrainDetailsParam, std::shared_ptr<Material>,
	data.fieldSize = Vector3(source->GetPropertyVector("fieldSize"));
	data.detailsCount = source->GetPropertyInt("detailsCount");
	data.blendCount = source->GetPropertyInt("blendCount");
	for(int i=0;i<12;i++) {
		data.tileST[i] = Vector4(source->GetPropertyVector(std::format("_tileST{0}", i)));
		data.textureActive[i] = Vector4(source->GetPropertyVector(std::format("_active{0}",i)));
	}
	/*data.tileOffset = Vector2(source->GetPropertyVector("tileOffset"));
	data.color = Vector4(source->GetPropertyVector("_Color"));*/
)

////////////////////////////////////////////////////////////////////////////

struct alignas(16) SeaDefaultMaterialParam
{
	vec4 color;
	vec4 clipingColor;
};

CBUFFER_INJECTOR("SeaDefaultMaterialParam", SeaDefaultMaterialParam, 2560, BufferType::SeaDefaultMaterialParam, std::shared_ptr<Material>,
	data.color = Vector4(source->GetPropertyVector("_Color"));
	data.clipingColor = Vector4(source->GetPropertyVector("_ClipingColor"));
)