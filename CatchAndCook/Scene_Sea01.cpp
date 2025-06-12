#include "pch.h"
#include "Scene_Sea01.h"
#include "testComponent.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "Transform.h"
#include "PathStamp.h"
#include "PlayerController.h"
#include "CameraComponent.h"
#include "SeaPlayerController.h"
#include "FishMonster.h"
#include "LightManager.h"
#include "PlantComponent.h"
#include "SkinnedMeshRenderer.h"
#include "Profiler.h"
#include "ComputeManager.h"
#include "ShadowManager.h"
#include "TerrainManager.h"
#include "CameraManager.h"
#include "InstancingManager.h"
#include "Gizmo.h"
#include "Mesh.h"
#include <filesystem>
#include "WaterController.h"
#include "Volumetric.h"


void Scene_Sea01::Init()
{
	namespace fs = std::filesystem;

	Scene::Init();

	caustics = make_shared<Texture>();
	caustics->Init(L"../Resources/Textures/test.jpg");

	lopeTexture = make_shared<Texture>();
	lopeTexture->Init(L"../Resources/Textures/start.jpg");

	//Volumetric::main = make_unique<Volumetric>();
	//Volumetric::main->Init();

	//vector<wstring> paths;
	//paths.reserve(240);

	//std::wstring orginPath = L"../Resources/Textures/Caustics/";
	//std::wstring path = L"../Resources/Textures/Caustics/";

	//for (const auto& entry : fs::directory_iterator(path))
	//{
	//	paths.push_back(orginPath + entry.path().filename().wstring());
	//}


	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap", L"cubemap.hlsl", GeoMetryProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap", L"Textures/cubemap/Sky_0.png.dds", TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("_BaseMap", texture);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(300.0f));
		meshRenderer->SetCulling(false);
	}

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::BACK;
		info.cullingType = CullingType::NONE;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"seatest", L"seatest.hlsl", GeoMetryProp,
			ShaderArg{ {{"VS_Main","vs"},{"PS_Main","ps"},{"HS_Main","hs"},{"DS_Main","ds"}} }, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"grid_orgin");
		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
		auto a = gameObject->AddComponent<WaterController>();
		a->Setting(L"sea_color_sea.bin", L"sea_move_real.bin");

		//meshRenderer->SetDebugShader(ResourceManager::main->Get<Shader>(L"DebugNormal_Sea"));
		gameObject->_transform->SetLocalPosition(vec3(0, 500.0f, 0));

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("_cubeMap", ResourceManager::main->_cubemap_skyTexture);
		material->SetUseMaterialParams(true);
		material->SetShadowCasting(false);
		meshRenderer->AddMaterials({ material });

		auto mesh = GeoMetryHelper::LoadGripMeshControlPoints(20000.0f, 20000.0f, 1000, 1000, false);
		mesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		meshRenderer->AddMesh(mesh);
		meshRenderer->SetCulling(false);

	};



	std::shared_ptr<Light> light = std::make_shared<Light>();
	light->onOff = 1;
	light->direction = vec3(-0.024f, 1.0f, 0.899f);
	light->position = vec3(769.f, 1282.f, 750.0f);
	light->direction.Normalize();

	light->material.ambient = vec3(0.4f, 0.4f, 0.4f);
	light->material.diffuse = vec3(1.0f, 1.0f, 1.0f);
	light->material.specular = vec3(0, 0, 0);
	light->material.shininess = 32.0f;
	light->material.lightType = static_cast<int32>(LIGHT_TYPE::DIRECTIONAL_LIGHT);
	light->strength = vec3(1.0f, 1.0f, 1.0f);
	LightManager::main->PushLight(light);
	LightManager::main->_lightParmas.mainLight = *light.get();

#pragma region DebugXYZ
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"color", L"color.hlsl", ColorProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"X");

		root->_transform->SetLocalPosition(vec3(3000.0f, 0, 0.0f));
		root->_transform->SetLocalScale(vec3(3000.0f, 1.0f, 1.0f));
		auto meshRenderer = root->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::NoEffectPostProcessing);

		meshRenderer->AddMaterials({ material });

		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(1, 0, 0, 0)));


	}

	{


		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"color", L"color.hlsl", ColorProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"Y");

		root->_transform->SetLocalPosition(vec3(0, 0, 3000.0f));
		root->_transform->SetLocalScale(vec3(1.0f, 1.0f, 3000.0f));
		auto meshRenderer = root->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::NoEffectPostProcessing);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(0, 1, 0, 0)));
	}

	{


		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"color", L"color.hlsl", ColorProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"Z");

		root->_transform->SetLocalPosition(vec3(0, 3000.0f, 0.0f));
		root->_transform->SetLocalScale(vec3(1.0f, 3000.0f, 1.0f));
		auto meshRenderer = root->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::NoEffectPostProcessing);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(0, 0, 1, 0)));
	}
#pragma endregion

	ColliderManager::main->SetCellSize(100);


	ResourceManager::main->LoadAlway<SceneLoader>(L"test2", L"../Resources/Datas/Scenes/sea2.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test2");
	sceneLoader->Load(GetCast<Scene>());
	auto player = Find(L"seaPlayer");


	if (player)
	{
		player->AddComponent<SeaPlayerController>();

		vector<shared_ptr<GameObject>> childs;
		player->GetChildsAll(childs);

		for (auto& child : childs)
		{
			auto skinnedMeshRenderer = child->GetComponent<SkinnedMeshRenderer>();
			if (skinnedMeshRenderer)
			{
				skinnedMeshRenderer->SetCulling(false);
			}
		}

	}



	

	{
		auto plant = Find(L"2");

		if (plant)
		{
			auto finder = plant->AddComponent<FishMonster>();
			finder->ReadPathFile(L"TutleMove");
		}
	}

	{
		auto plant = Find(L"shark");

		if (plant)
		{
			auto finder = plant->AddComponent<FishMonster>();
			finder->ReadPathFile(L"SharkMove");
		}
	}

	std::ranges::sort(_gameObjects, [&](const auto& a, const auto& b) {
		return a->GetName() < b->GetName();
		});






}

void Scene_Sea01::Update()
{
	

	Scene::Update();

	PathStamp::main->Run();
}

void Scene_Sea01::RenderBegin()
{
	Scene::RenderBegin();
}

void Scene_Sea01::Rendering()
{
	GlobalSetting();

	TableContainer conatiner = Core::main->GetBufferManager()->GetTable()->Alloc(3);
	{
		Core::main->GetBufferManager()->GetTable()->CopyHandle(conatiner.CPUHandle, caustics->GetSRVCpuHandle(), 0);
		Core::main->GetBufferManager()->GetTable()->CopyHandle(conatiner.CPUHandle, lopeTexture->GetSRVCpuHandle(), 1);
		Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(GLOBAL_SRV_INDEX, conatiner.GPUHandle);
		_globalParam.caustics = 1;
	}



	auto& cmdList = Core::main->GetCmdList();
	Core::main->GetRenderTarget()->ClearDepth();

	//Profiler::Set("PASS : Shadow", BlockTag::CPU);  
	//ShadowPass(cmdList);
	//Profiler::Fin();

	Profiler::Set("PASS : Deferred", BlockTag::CPU);
	DeferredPass(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : FinalPass", BlockTag::CPU);
	FinalRender(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : Forward", BlockTag::CPU);
	ForwardPass(cmdList);
	Profiler::Fin();

	//Profiler::Set("PASS : Transparent", BlockTag::CPU);
	//TransparentPass(cmdList); // Position,
	//Profiler::Fin();

	Profiler::Set("PASS : Compute", BlockTag::CPU);
	ComputePass(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : NoEffect", BlockTag::CPU);
	NoEffectPass(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : UI", BlockTag::CPU);
	UiPass(cmdList);
	Profiler::Fin();
}

void Scene_Sea01::DebugRendering()
{
	Scene::DebugRendering();
}

void Scene_Sea01::RenderEnd()
{
	Scene::RenderEnd();
}

void Scene_Sea01::Finish()
{
	Scene::Finish();

	/*if (Input::main->GetKeyDown(KeyCode::F6))
	{
		SceneManager::main->ChangeScene(SceneManager::main->GetCurrentScene(), SceneManager::main->FindScene(SceneType::TestScene2), true, true);
	}*/
}

void Scene_Sea01::SetSeaGlobalData()
{

}

void Scene_Sea01::ShadowPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	/*auto light = LightManager::main->GetMainLight();
	auto a = ShadowManager::main->SeaCalculateBounds(CameraManager::main->GetCamera(CameraType::SeaCamera).get(), light.get(), { 3000 });
	for (auto& b : a)
	{
		Gizmo::Width(5.0f);
		Gizmo::Box(b, Vector4(0, 1, 0, 1));
		Gizmo::WidthRollBack();
	}*/

	{
		auto light = LightManager::main->GetMainLight();
		if (light == nullptr)
		{
			cout << "없음" << endl;
			return;
		}

		auto boundings = ShadowManager::main->SeaCalculateBounds(CameraManager::main->GetCamera(CameraType::SeaCamera).get(), light.get(), { 500, 1000 ,2000, 3000 });
		auto lastShadowPos = ShadowManager::main->_lightTransform[ShadowManager::main->_lightTransform.size() - 1];
		TerrainManager::main->CullingInstancing(lastShadowPos.first, lastShadowPos.second);
		auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Shadow)];

		ShadowManager::main->SetData(nullptr);
		ShadowManager::main->RenderBegin();

		int i = 0;

		for (auto& bounding : boundings)
		{
			ShadowCascadeIndexParams shadowCasterParams;

			auto* cbuffer2 = Core::main->GetBufferManager()->GetBufferPool(BufferType::ShadowCascadeIndexParams)->Alloc(1);
			shadowCasterParams.cascadeIndex = i;
			memcpy(cbuffer2->ptr, &shadowCasterParams, sizeof(ShadowCascadeIndexParams));
			Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(7, cbuffer2->GPUAdress);
			Core::main->GetShadowBuffer()->RenderBegin(i);

			for (auto& [shader, vec] : targets)
			{
				cmdList->SetPipelineState(shader->_pipelineState.Get());

				for (auto& renderStructure : vec)
				{
					if (renderStructure.renderer->IsCulling() == true)
					{
						if (bounding.Intersects(renderStructure.renderer->GetBound()) == false)
						{
							continue;
						}
					}

		/*			SettingPrevData(renderStructure, RENDER_PASS::PASS::Shadow);*/

					if (renderStructure.renderer->isInstancing() == false)
					{
						g_debug_shadow_draw_call++;
						InstancingManager::main->RenderNoInstancing(renderStructure);
					}
					else
					{
						g_debug_shadow_draw_call++;
						InstancingManager::main->AddObject(renderStructure);
					}
				}


				InstancingManager::main->Render();
			}

			Core::main->GetShadowBuffer()->RenderEnd();
			i++;
		}

		ShadowManager::main->RenderEnd();
	}
}

void Scene_Sea01::FinalRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Core::main->GetRenderTarget()->RenderBegin();

	auto mesh = ResourceManager::main->Get<Mesh>(L"finalMesh");
	auto shader = _finalShader->GetShader();

	cmdList->SetPipelineState(shader->_pipelineState.Get());

	//RenderObjectStrucutre ROS = { _finalShader.get(), mesh.get(), nullptr };
	//SettingPrevData(ROS, RENDER_PASS::PASS::Deferred);
	//_finalShader->SetData();

	mesh->Redner();
}

void Scene_Sea01::ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	ComputeManager::main->Dispatch(cmdList);
}

void Scene_Sea01::NoEffectPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	{ // Forward
		auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::NoEffectPostProcessing)];

		for (auto& [shader, vec] : targets)
		{
			if (shader)
			{
				cmdList->SetPipelineState(shader->_pipelineState.Get());
			}

			for (auto& ele : vec)
			{
				g_debug_forward_count++;

				if (ele.renderer->IsCulling() == true)
				{
					if (CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound()) == false)
					{
						g_debug_forward_culling_count++;
						continue;
					}
				}

				if (ele.renderer->isInstancing() == false)
				{
					InstancingManager::main->RenderNoInstancing(ele);
				}
				else
				{
					InstancingManager::main->AddObject(ele);
				}
			}

			InstancingManager::main->Render();
		}

	} 
}

void Scene_Sea01::ParticlePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

	auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::ParticlePass)];

	for (auto& [shader, vec] : targets)
	{
		for (auto& ele : vec)
		{
			ele.renderer->Rendering(nullptr, nullptr, 1);
		}
	}

}





void Scene_Sea01::UiPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	{
		auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::UI)];

		for (auto& [shader, vec] : targets)
		{
			cmdList->SetPipelineState(shader->_pipelineState.Get());

			for (auto& renderStructure : vec)
			{
				auto& [material, mesh, target] = renderStructure;
				target->Rendering(material, mesh);
			}
		}
	}
}

void Scene_Sea01::TransparentPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	//{ 
	//	auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Transparent)];

	//	std::vector<RenderObjectStrucutre> vec;
	//	vec.reserve(2048);
	//	for (auto& [shader, vec2] : targets)
	//		vec.insert(vec.end(), vec2.begin(), vec2.end());

	//	Vector3 cameraPos = CameraManager::main->GetActiveCamera()->GetCameraPos();
	//	Vector3 cameraDir = CameraManager::main->GetActiveCamera()->GetCameraLook();

	//	auto tangentDistanceSquared = [&](const Vector3& center) -> float {
	//		Vector3 offset = center - cameraPos;
	//		float projection = offset.Dot(cameraDir);
	//		return offset.LengthSquared() - projection * projection;
	//		};

	//	std::ranges::sort(vec, [&](const RenderObjectStrucutre& a, const RenderObjectStrucutre& b) {
	//		return tangentDistanceSquared(a.renderer->_bound.Center) < tangentDistanceSquared(b.renderer->_bound.Center);
	//		});

	//	Shader* prevShader = nullptr;
	//	for (auto& ele : vec)
	//	{
	//		Shader* shader = ele.material->GetShader().get();
	//		if (shader != nullptr && shader != prevShader)
	//			cmdList->SetPipelineState(shader->_pipelineState.Get());

	//		g_debug_forward_count++;

	//		if (ele.renderer->IsCulling() == true)
	//		{
	//			if (CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound()) == false)
	//			{
	//				g_debug_forward_culling_count++;
	//				continue;
	//			}
	//		}

	//		SettingPrevData(ele, RENDER_PASS::PASS::Transparent);
	//		InstancingManager::main->RenderNoInstancing(ele);

	//		prevShader = shader;
	//	}
	//}
}

void Scene_Sea01::ForwardPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	{ // Forward
		auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Forward)];

		for (auto& [shader, vec] : targets)
		{
			cmdList->SetPipelineState(shader->_pipelineState.Get());

			for (auto& ele : vec)
			{
				g_debug_forward_count++;

				if (ele.renderer->IsCulling() == true)
				{
					if (CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound()) == false)
					{
						g_debug_forward_culling_count++;
						continue;
					}
				}

				if (ele.renderer->isInstancing() == false)
				{
					InstancingManager::main->RenderNoInstancing(ele);
				}
				else
				{
					InstancingManager::main->AddObject(ele);
				}
			}

			InstancingManager::main->Render();
		}

	}
}

void Scene_Sea01::DeferredPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Core::main->GetGBuffer()->RenderBegin();
	auto camera = CameraManager::main->GetActiveCamera();

	TerrainManager::main->CullingInstancing(camera->GetCameraPos(), camera->GetCameraLook());

	{ // Deferred
		auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Deferred)];

		for (auto& [shader, vec] : targets)
		{
			cmdList->SetPipelineState(shader->_pipelineState.Get());

			for (auto& ele : vec)
			{
				g_debug_deferred_count++;

				if (ele.renderer->IsCulling() == true)
				{
					if (CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound()) == false)
					{
						g_debug_deferred_culling_count++;
						continue;
					}
				}

				if (ele.renderer->isInstancing() == false)
				{
					InstancingManager::main->RenderNoInstancing(ele);
				}
				else
				{
					InstancingManager::main->AddObject(ele);
				}
			}

			InstancingManager::main->Render();

		}
	}

	Core::main->GetGBuffer()->RenderEnd();
}
