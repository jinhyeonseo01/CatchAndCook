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
#include "PathStamp.h"
#include "ShowGameMoeny.h"
#include "BilboardComponent.h"
#include "AnimationSpriteComponent.h"

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

		{
			auto object = SceneManager::main->GetCurrentScene()->Find(L"OnBoardEvent");
			auto eventComponent = object->AddComponent<EventComponent>();
			eventComponent->SetBindTag(GameObjectTag::Player);
			eventComponent->SetBindMessage(L"F키를 눌러 탑승하세요", vec3(0.35f, 0.7f, 0.01f), vec2(0.5f, 0.5f), false);

			eventComponent->BindOnCollisionBegin([=](shared_ptr<Collider> collider)
				{
					eventComponent->ShowEventMessage(true);
				});

			eventComponent->BindOnCollisionEnd([=](shared_ptr<Collider> collider)
				{
					eventComponent->ShowEventMessage(false);
				});

			eventComponent->BindOnUpdateBlock([](shared_ptr<Collider> collider)
				{
					if (Input::main->GetKeyDown(KeyCode::F))
					{
						auto player = SceneManager::main->GetCurrentScene()->Find(L"player");
						player->GetComponent<PlayerController>()->SetOnBoard();
					}
				});
		}
	}

	
	shared_ptr<GameObject> showGameMoeny = SceneManager::main->GetCurrentScene()->CreateGameObject(L"ShowGameMoeny");
	showGameMoeny->AddComponent<ShowGameMoeny>();

	/*{
		auto object =SceneManager::main->GetCurrentScene()->Find(L"Fire_animationSprite");

		if (object)
		{

			vector<shared_ptr<Texture>> _textures;
			std::wstring path = L"../Resources/Textures/Sprite/jin/fire/";

			for (const auto& entry : fs::directory_iterator(path))
			{
				std::wstring& path2 = entry.path().filename().wstring();
				shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(path + path2, path + path2);
				_textures.push_back(texture);
			}

			object->GetComponent<MeshRenderer>()->GetMaterial(0)->SetPass(RENDER_PASS::Forward);
			object->AddComponent<BilboardComponent>();
			auto animationSpriteComponent = object->AddComponent<AnimationSpriteComponent>();
			animationSpriteComponent->SetTextures(_textures);
			animationSpriteComponent->SetRoop(true);
			animationSpriteComponent->SetFrameRate(2.0f);
		}
		
	}*/

}

void TestScene_jin::Update()
{
	ColliderManager::main->SetCellSize(5);


	PathStamp::main->Run();

 	Scene::Update();

}

void TestScene_jin::RenderBegin()
{
	Scene::RenderBegin();
}

void TestScene_jin::Rendering()
{

	Scene::Rendering();

	
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

	if (Scene::_changeScene)
	{
		Scene::_changeScene = false;
		if (_changeSceneType == SceneType::MainMenu)
			SceneManager::main->ChangeScene(SceneManager::main->GetCurrentScene(), SceneManager::main->FindScene(SceneType::MainMenu), false, false);
		else
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

