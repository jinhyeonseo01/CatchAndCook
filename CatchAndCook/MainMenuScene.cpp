#include "pch.h"
#include "MainMenuScene.h"


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

void MainMenuScene::Init()
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

	ColliderManager::main->SetCellSize(5);
	ResourceManager::main->Load<SceneLoader>(L"mainMenu", L"../Resources/Datas/Scenes/MainMenu.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"mainMenu");
	sceneLoader->Load(GetCast<Scene>());





}

void MainMenuScene::Update()
{
	ColliderManager::main->SetCellSize(5);

	Scene::Update();
	ComputeManager::main->_dofRender->_on = true;
	InGameGlobal::main->skyTime += Time::main->GetDeltaTime() * 0.2f;
}

void MainMenuScene::RenderBegin()
{
	Scene::RenderBegin();
}

void MainMenuScene::Rendering()
{

	Scene::Rendering();


}

void MainMenuScene::DebugRendering()
{
	Scene::DebugRendering();
}

void MainMenuScene::RenderEnd()
{
	Scene::RenderEnd();
}

void MainMenuScene::Finish()
{
	Scene::Finish();

	if (Scene::_changeScene)
	{
		ComputeManager::main->_dofRender->_on = false;
		Scene::_changeScene = false;
		InGameGlobal::main->skyTime = 0;
		SceneManager::main->ChangeScene(SceneManager::main->GetCurrentScene(), SceneManager::main->FindScene(SceneType::TestScene2), false, false);
	}




}

MainMenuScene::~MainMenuScene()
{
}

void MainMenuScene::ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	ComputeManager::main->DispatchMainField(cmdList);
}

