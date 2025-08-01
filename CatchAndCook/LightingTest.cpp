#include "pch.h"
#include "LightingTest.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "random"
#include "Mesh.h"
#include "ImguiManager.h"
#include "WaterController.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Sprite.h"
#include "TextManager.h"
#include "SpriteAction.h"
#include "testComponent.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "WaterHeight.h"
#include "Terrain.h"
#include "BoidsMove.h"
#include <random>
#include "Model.h"
#include "ModelMesh.h"
#include "Collider.h"
#include "PlantComponent.h"
#include "PathStamp.h"
#include "PathFinder.h"
void LightingTest::Init()
{
	Scene::Init();

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
		material->SetPass(RENDER_PASS::Forward);

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
		material->SetPass(RENDER_PASS::Forward);

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
		material->SetPass(RENDER_PASS::Forward);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(0, 0, 1, 0)));
	}
#pragma endregion



	{
		ShaderInfo info;
		info._zTest = true;
		info._zWrite = false;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap", L"cubemap.hlsl", GeoMetryProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap", L"Textures/cubemap/output.dds", TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		gameObject->_transform->SetLocalRotation(vec3(0, 90.0f, 0) * D2R);

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
		meshRenderer->SetCulling(false);

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("g_tex_0", texture);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}


	ResourceManager::main->LoadAlway<SceneLoader>(L"test", L"../Resources/Datas/Scenes/TestScene2.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test");
	sceneLoader->Load(GetCast<Scene>());

	{
		auto plant = Find(L"2");

		if (plant)
		{
			auto finder =plant->AddComponent<PathFinder>();
			finder->SetPass(L"BezierData.txt");
			finder->SetMoveSpeed(50.0f);
			finder->SetStartIndex(30);
		}
	}

	{
		auto plant = Find(L"3");

		if (plant)
		{
			auto finder = plant->AddComponent<PathFinder>();
			finder->SetPass(L"BezierData.txt");
			finder->SetMoveSpeed(250.0f);
			finder->SetStartIndex(100);
		}
	}

	{
		auto plant = Find(L"4");

		if (plant)
		{
			auto finder = plant->AddComponent<PathFinder>();
			finder->SetPass(L"BezierData.txt");
			finder->SetMoveSpeed(50.0f);
			finder->SetStartIndex(150);
		}
	}

	/*{
		auto plant = Find(L"6");

		if (plant)
		{
			plant->GetComponent<MeshRenderer>()->GetMaterials()[0]->SetShader(ResourceManager::main->Get<Shader>(L"Plant"));
			plant->AddComponent<PlantComponent>();
		}
	}*/


}

void LightingTest::Update()
{
	Scene::Update();

	PathStamp::main->Run();

}

void LightingTest::RenderBegin()
{
	Scene::RenderBegin();
}

void LightingTest::Rendering()
{
	Scene::Rendering();

}

void LightingTest::RenderEnd()
{
	Scene::RenderEnd();
}

void LightingTest::Finish()
{
	Scene::Finish();
}
