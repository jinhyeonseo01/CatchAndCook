#include "pch.h"
#include "ResourceManager.h"
#include "Global.h"



unique_ptr<ResourceManager> ResourceManager::main =nullptr;
std::wstring ResourceManager::path_model = L"../Resources/";

//ui[일주일]-> pass[1일] -> 바다구현[일주일].
//modelLoad(unity연동수정) -> TerrianSystem -> SkinnedMesh(animation) [15일]-> //navymesh 시스템 

void ResourceManager::Init()
{
	CreateDefaultShader();
	CreateDefaultMesh();
	CreateDefaultMaterial();
	CreateDefaultTexture();
	CreateDefaultModel();
}

void ResourceManager::CreateDefaultModel()
{
	ResourceManager::main->Load<Model>(L"Sphere", path_model +	L"Configs/Sphere.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Rect", path_model +		L"Configs/Rect.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Quad", path_model +		L"Configs/Quad.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Cylinder", path_model +	L"Configs/Cylinder.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Cube", path_model +		L"Configs/Cube.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Capsule", path_model +	L"Configs/Capsule.fbx", VertexType::Vertex_Static);
}

void ResourceManager::CreateDefaultMesh()
{
	{
		auto finalMesh =  GeoMetryHelper::LoadRectangleMesh(1.0f);
		Add<Mesh>(L"finalMesh",finalMesh);
	}
}

void ResourceManager::CreateDefaultShader()
{

	CreateDefaultShaderKSH();
	CreateDefaultShaderJIN();


}

void ResourceManager::CreateDefaultShaderKSH()
{

	{
		ShaderInfo info;
		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.cullingType = CullingType::FRONT;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"D_CullFront", L"DeferredSea.hlsl", StaticProp,
			ShaderArg{}, info);

		shader->SetInjector({ BufferType::SeaDefaultMaterialParam });
		//shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);

	}



	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::SeaDefaultMaterialParam });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"DeferredSeaGrass.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"DeferredSeaGrass", shader);
	}


	{
		ShaderInfo info;
		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"D_SeaFish", L"DeferredSeaFish.hlsl", StaticProp,
			ShaderArg{}, info);
		shader->SetInjector({ BufferType::SeaDefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);
	}

	{
		ShaderInfo info;
		info.renderTargetCount = 4;

	
		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"D_SeaEnv", L"DeferredSea.hlsl", StaticProp,
			ShaderArg{}, info);

		shader->SetInjector({ BufferType::SeaDefaultMaterialParam });
		//shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);

	}

	{
		ShaderInfo info;
		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"DeferredSeaSkinned", L"DeferredSeaSkinned.hlsl", SkinProp,
			ShaderArg{}, info);
		shader->SetInjector({ BufferType::SeaDefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);
	}

	{
		ShaderInfo info;
		info.renderTargetCount = 4;
		info.cullingType = CullingType::NONE;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"SeaPlayer", L"DeferredSeaSkinned.hlsl", SkinProp,
			ShaderArg{}, info);

		shader->SetInjector({ BufferType::SeaDefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);
	}

	{
		ShaderInfo info;
		info.renderTargetCount = 4;
		info.cullingType = CullingType::NONE;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"D_SeaPlantClip", L"DeferredSeaPlantClip.hlsl", StaticProp,
			ShaderArg{}, info);

		shader->SetInjector({ BufferType::SeaDefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);
	}
	

	{
		ShaderInfo info;
		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"D_SeaPlant", L"DeferredSeaPlant.hlsl", StaticProp,
			ShaderArg{}, info);

		shader->SetInjector({ BufferType::SeaDefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);
	}

	{
		ShaderInfo info;
		info.renderTargetCount = 4;
		//info.cullingType = CullingType::WIREFRAME;
		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

#ifdef RECT_TERRAIN
		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"Terrain", L"TerrainQuad.hlsl", GeoMetryProp,
			ShaderArg{ {{"VS_Main","vs"},{"PS_Main","ps"},{"HS_Main","hs"},{"DS_Main","ds"}} }, info);
#else
		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"Terrain", L"Terrain.hlsl", GeoMetryProp,
			ShaderArg{ {{"VS_Main","vs"},{"PS_Main","ps"},{"HS_Main","hs"},{"DS_Main","ds"}} }, info);
#endif
		shader->SetInjector({ BufferType::TerrainDetailsParam });
		shader->SetPass(RENDER_PASS::Deferred);
	}

	{
		ShaderInfo info;
		info._zTest = false;
		info._zWrite = false;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"final.hlsl", GeoMetryProp, ShaderArg{}, info);
		Add<Shader>(L"finalShader", shader);
	}



	{

		ShaderInfo info;
		info._zTest = false;
		info._stencilTest = false;
		info._blendEnable = true;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"SpriteShader.hlsl", ColorProp, ShaderArg{}, info);
		Add<Shader>(L"SpriteShader", shader);
	}






	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::UI);
		shader->Init(L"UiForward.hlsl", StaticProp, ShaderArg{});
		Add<Shader>(L"UiForward", shader);
	}


}

void ResourceManager::CreateDefaultShaderJIN()
{
	{
		ShaderInfo info;
		info._zTest = true;
		info._zWrite = false;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"Skybox", L"Skybox_Field.hlsl", GeoMetryProp,
			ShaderArg{}, info);
	}
	{
		ShaderInfo info;
		info._zTest = false;
		info._zWrite = false;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"final_MainField.hlsl", GeoMetryProp, ShaderArg{}, info);
		Add<Shader>(L"finalShader_MainField", shader);
	}
	//Forward
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"TestForward_Total.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultForward", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"TestForward_Total.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultForward_Skinned", shader);
	}

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Forward);
		shader->SetMacro({ {"INSTANCED",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->Init(L"TestForward_Total.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultForward_Instanced", shader);
	}
	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Forward);
		shader->SetMacro({ {"INSTANCED",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->Init(L"TestForward_Total.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultForward_NOAO_Instanced", shader);
	}
	//Deferred
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"TestDeferred_Total.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultDeferred", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"TestDeferred_Total.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultDeferred_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetMacro({ {"INSTANCED",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"TestDeferred_Total.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultDeferred_Instanced", shader);
	}


	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::EnvMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"Environment.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"Environment", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::EnvMaterialParam });
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"Environment.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"Environment_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::EnvMaterialParam });
		shader->SetMacro({ {"INSTANCED",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"Environment.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"Environment_Instanced", shader);
	}
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::EnvMaterialParam });
		shader->SetMacro({ {"INSTANCED",nullptr}, {"_NO_AO",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"Environment.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"Environment_NOAO_Instanced", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::EnvMaterialParam });
		shader->SetMacro({ {"INSTANCED",nullptr}, {"_NO_AO",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"Environment_Grass.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"Environment_Grass", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"ForwardPreDepthNormal.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"DepthNormal", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"ForwardPreDepthNormal.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DepthNormal_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		info.renderTargetCount = 4;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetMacro({ {"INSTANCED",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"ForwardPreDepthNormal.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DepthNormal_Instanced", shader);
	}


	//ForwardPreDepthNormal.hlsl
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam, BufferType::PlayerMaterialParam });
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"PlayerShader.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"PlayerShader", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam, BufferType::PlayerMaterialParam });
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"PlayerShader_Skin.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"PlayerShader_Skin", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam, BufferType::PlayerMaterialParam });
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"PlayerShader_Face.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"PlayerShader_Face", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"ObjectShader.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"ObjectShader_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"ObjectShader.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"ObjectShader", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;
		info._depthOnly = true;

		info.depthBias = 250;
		info.slopeScaledDepthBias = 2.0;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Shadow);
		shader->Init(L"ShadowCaster.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"ShadowCaster", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;
		info._depthOnly = true;

		info.depthBias = 250;
		info.slopeScaledDepthBias = 2.0;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Shadow);
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->Init(L"ShadowCaster.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"ShadowCaster_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;
		info._depthOnly = true;

		info.depthBias = 250;
		info.slopeScaledDepthBias = 2.0;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Shadow);
		shader->SetMacro({ {"INSTANCED",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->Init(L"ShadowCaster.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"ShadowCaster_Instanced", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;
		info._depthOnly = true;

		info.depthBias = 250;
		info.slopeScaledDepthBias = 2.0;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Shadow);
		shader->Init(L"ShadowCaster_Early.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"ShadowCaster_Early", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;
		info._depthOnly = true;

		info.depthBias = 250;
		info.slopeScaledDepthBias = 2.0;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Shadow);
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->Init(L"ShadowCaster_Early.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"ShadowCaster_Early_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;
		info._depthOnly = true;

		info.depthBias = 250;
		info.slopeScaledDepthBias = 2.0;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Shadow);
		shader->SetMacro({ {"INSTANCED",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->Init(L"ShadowCaster_Early.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"ShadowCaster_Early_Instanced", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;
		info._depthOnly = true;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

		info.depthBias = 250;
		info.slopeScaledDepthBias = 2.0;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Shadow);
		shader->SetInjector({ BufferType::TerrainDetailsParam });
		shader->Init(L"ShadowCaster_Terrain.hlsl", GeoMetryProp,
			ShaderArg{ {{"VS_Main","vs"},{"PS_Main","ps"},{"HS_Main","hs"},{"DS_Main","ds"}} }, info);
		Add<Shader>(L"ShadowCaster_Terrain", shader);
	}


	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._zWrite = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam, BufferType::WaterParam });
		shader->SetPass(RENDER_PASS::Transparent);
		shader->Init(L"WaterShader.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"WaterShader", shader);
	}



	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInstanceProp(GizmoInstanceProp);
		shader->Init(L"Gizmo.hlsl", {}, ShaderArg{ {{"PS_Main","ps"},{"VS_Main","vs"},{"GS_Main","gs"}} }, info);
		shader->SetPass(RENDER_PASS::Debug);
		Add<Shader>(L"Gizmo", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		info.cullingType = CullingType::BACK;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Debug);
		shader->Init(L"Gizmo_Text.hlsl", StaticProp, ShaderArg{ {{"PS_Main","ps"},{"VS_Main","vs"}} }, info);
		Add<Shader>(L"GizmoTexture", shader);
	}
}

void ResourceManager::CreateDefaultMaterial()
{
	_depthNormal = std::make_shared<Material>();
	_depthNormal->SetShader(ResourceManager::main->Get<Shader>(L"DepthNormal"));
	_depthNormal->SetPass(RENDER_PASS::Deferred);

	_depthNormal_Skinned = std::make_shared<Material>();
	_depthNormal_Skinned->SetShader(ResourceManager::main->Get<Shader>(L"DepthNormal_Skinned"));
	_depthNormal_Skinned->SetPass(RENDER_PASS::Deferred);

	_depthNormal_Instanced = std::make_shared<Material>();
	_depthNormal_Instanced->SetShader(ResourceManager::main->Get<Shader>(L"DepthNormal_Instanced"));
	_depthNormal_Instanced->SetPass(RENDER_PASS::Deferred);

	_shadowCaster = std::make_shared<Material>();
	_shadowCaster->SetShader(ResourceManager::main->Get<Shader>(L"ShadowCaster"));
	_shadowCaster->SetPass(RENDER_PASS::Shadow);
	//_shadowCaster->SetSetDataOff(true);

	_shadowCaster_Skinned = std::make_shared<Material>();
	_shadowCaster_Skinned->SetShader(ResourceManager::main->Get<Shader>(L"ShadowCaster_Skinned"));
	_shadowCaster_Skinned->SetPass(RENDER_PASS::Shadow);
	//_shadowCaster_Skinned->SetSetDataOff(true);

	_shadowCaster_Instanced = std::make_shared<Material>();
	_shadowCaster_Instanced->SetShader(ResourceManager::main->Get<Shader>(L"ShadowCaster_Instanced"));
	_shadowCaster_Instanced->SetPass(RENDER_PASS::Shadow);

	_shadowCaster_Terrain = std::make_shared<Material>();
	_shadowCaster_Terrain->SetShader(ResourceManager::main->Get<Shader>(L"ShadowCaster_Terrain"));
	_shadowCaster_Terrain->SetPass(RENDER_PASS::Shadow);
	//_shadowCaster_Instanced->SetSetDataOff(true);


	_shadowCaster_Early = std::make_shared<Material>();
	_shadowCaster_Early->SetShader(ResourceManager::main->Get<Shader>(L"ShadowCaster_Early"));
	_shadowCaster_Early->SetPass(RENDER_PASS::Shadow);
	//_shadowCaster->SetSetDataOff(true);

	_shadowCaster_Early_Skinned = std::make_shared<Material>();
	_shadowCaster_Early_Skinned->SetShader(ResourceManager::main->Get<Shader>(L"ShadowCaster_Early_Skinned"));
	_shadowCaster_Early_Skinned->SetPass(RENDER_PASS::Shadow);
	//_shadowCaster_Skinned->SetSetDataOff(true);

	_shadowCaster_Early_Instanced = std::make_shared<Material>();
	_shadowCaster_Early_Instanced->SetShader(ResourceManager::main->Get<Shader>(L"ShadowCaster_Early_Instanced"));
	_shadowCaster_Early_Instanced->SetPass(RENDER_PASS::Shadow);
}



void ResourceManager::CreateDefaultTexture()
{
	_noneTexture = Load<Texture>(L"none", L"Textures/Configs/noneTexture.png");
	_noneTexture_Black = Load<Texture>(L"none_black", L"Textures/Configs/noneTexture_Black.png");
	_noneTexture_debug = Load<Texture>(L"none_debug", L"Textures/Configs/noneTexture_debug.png");

	_bakedGITexture = Load<Texture>(L"BakedGI", L"Textures/Configs/BakedGI.png");
	_bakedGINTexture = Load<Texture>(L"BakedGI_Night", L"Textures/Configs/BakedGI_Night.png");
	_bakedGIETexture = Load<Texture>(L"BakedGI_Evening", L"Textures/Configs/BakedGI_Evening.png");

	_bakedGIFinal1Texture = _bakedGITexture;
	_bakedGIFinal2Texture = _bakedGITexture;

	_cubemap_skyTexture = Load<Texture>(L"cubemap_sky0", L"Textures/cubemap/Sky_0.png.dds", TextureType::CubeMap);
	_cubemap_skyNTexture = Load<Texture>(L"cubemap_sky1", L"Textures/cubemap/Sky_1.png.dds", TextureType::CubeMap);
	_cubemap_skyETexture = Load<Texture>(L"cubemap_sky2", L"Textures/cubemap/Sky_2.png.dds", TextureType::CubeMap);
}

void ResourceManager::CreateDefaultAnimation()
{

}
