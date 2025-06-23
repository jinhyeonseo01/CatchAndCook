#include "pch.h"
#include "TestScene_jin.h"
#include "Camera.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "MeshRenderer.h"
#include "testComponent.h"
#include "Transform.h"
#include "Mesh.h"
#include "WaterController.h"
#include "Collider.h"
#include "ComputeManager.h"
#include "PlayerController.h"
#include "testComponent.h"
#include "EventComponent.h"
#include "TextManager.h"


void TestScene_jin::Init()
{
	Scene::Init();

	_finalShader->SetShader(ResourceManager::main->Get<Shader>(L"finalShader_MainField"));
	_finalShader->SetPass(RENDER_PASS::Forward);



	{
		shared_ptr<Shader> shader = ResourceManager::main->Get<Shader>(L"Skybox");
		shared_ptr<Material> material = make_shared<Material>();
		shared_ptr<GameObject> gameObject = CreateGameObject(L"Skybox");

		gameObject->_transform->SetLocalRotation(vec3(0, 90.0f, 0) * D2R);

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetShadowCasting(false);
		material->SetPreDepthNormal(false);
		material->SetTexture("_BaseMap", ResourceManager::main->_cubemap_skyTexture);
		material->SetTexture("_BaseMap_1", ResourceManager::main->_cubemap_skyNTexture);

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
		meshRenderer->SetCulling(false);
		meshRenderer->SetInstancing(false);
		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
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

		shared_ptr<GameObject> gameObject = CreateGameObject(L"sea");
		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
		auto a = gameObject->AddComponent<WaterController>();
		a->Setting(L"sea_color_real.bin", L"sea_move_real.bin");
		//meshRenderer->SetDebugShader(ResourceManager::main->Get<Shader>(L"DebugNormal_Sea"));
		gameObject->_transform->SetLocalPosition(vec3(0, 44.5f, 0));



		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("_cubemap_skyTexture", ResourceManager::main->_cubemap_skyTexture);
		material->SetTexture("_cubemap_skyNTexture", ResourceManager::main->_cubemap_skyNTexture);
		material->SetTexture("_cubemap_skyETexture", ResourceManager::main->_cubemap_skyETexture);
		material->SetUseMaterialParams(true);
		material->SetShadowCasting(false);
		meshRenderer->AddMaterials({ material });

		auto mesh = GeoMetryHelper::LoadGripMeshControlPoints(20000.0f, 20000.0f, 1000, 1000, false);
		mesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		meshRenderer->AddMesh(mesh);
		meshRenderer->SetCulling(false);

	};

	ColliderManager::main->SetCellSize(5);
	ResourceManager::main->Load<SceneLoader>(L"test", L"../Resources/Datas/Scenes/MainField2.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test");
	sceneLoader->Load(GetCast<Scene>());


	{
		shared_ptr<GameObject> BoardText = CreateGameObject(L"BoardText");
		auto& renderer = BoardText->AddComponent<MeshRenderer>();

		auto& sprite = BoardText->AddComponent<TextSprite>();
		sprite->SetLocalPos(vec3(500.0f, 600.0f, 0.000001f));
		sprite->SetSize(vec2(300, 300));
		sprite->SetText(L"Press F To Board");
		sprite->CreateObject(512, 256, L"Arial", FontColor::WHITE, 60);

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
		material->SetPass(RENDER_PASS::UI);

		renderer->AddMaterials({ material });
		BoardText->SetActiveSelf(false);
	};

	{
		auto& object = SceneManager::main->GetCurrentScene()->Find(L"OnBoardEvent");
		auto& eventComponent = object->GetComponent<EventComponent>();
		eventComponent->SetBindTag(GameObjectTag::Player);
		eventComponent->BindOnCollisionBegin([](shared_ptr<Collider>& collider) 
			{
			 auto object= SceneManager::main->GetCurrentScene()->Find(L"BoardText");
			 if (object)
			 {
				 object->SetActiveSelf(true);
			 }
			});
		eventComponent->BindOnCollisionEnd([](shared_ptr<Collider>& collider) 
			{
				auto object = SceneManager::main->GetCurrentScene()->Find(L"BoardText");
				if (object)
				{
					object->SetActiveSelf(false);

				}
			});
		eventComponent->BindOnUpdate([](shared_ptr<Collider>& collider) {
			if (Input::main->GetKeyDown(KeyCode::F))
			{
				auto player = SceneManager::main->GetCurrentScene()->Find(L"player");

				player->GetComponent<PlayerController>()->SetOnBoard();
			}
			});
	}


}

void TestScene_jin::Update()
{
	ColliderManager::main->SetCellSize(5);

	Scene::Update();

}

void TestScene_jin::RenderBegin()
{
	Scene::RenderBegin();
}

void TestScene_jin::Rendering()
{

	Scene::Rendering();

	ParticlePass(Core::main->GetCmdList());
}

void TestScene_jin::DebugRendering()
{
	Scene::DebugRendering();
}

void TestScene_jin::RenderEnd()
{
	Scene::RenderEnd();
}

void TestScene_jin::Finish()
{
	Scene::Finish();

	/*if (Input::main->GetKeyDown(KeyCode::F6))
	{
		SceneManager::main->ChangeScene(SceneManager::main->GetCurrentScene(), SceneManager::main->FindScene(SceneType::Sea01), false, false);
	}*/

	if (Scene::_changeScene)
	{
		Scene::_changeScene = false;
		SceneManager::main->ChangeScene(SceneManager::main->GetCurrentScene(), SceneManager::main->FindScene(SceneType::Sea01), false, false);

	}


}

TestScene_jin::~TestScene_jin()
{
}

void TestScene_jin::ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	ComputeManager::main->DispatchMainField(cmdList);
}

void TestScene_jin::ParticlePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
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
