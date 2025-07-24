#include "pch.h"
#include "UITestScene.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include <random>
#include "Sprite.h"
#include "SpriteAction.h"
#include "TextManager.h"
#include "Inventory.h"
#include "LightManager.h"

void UITestScene::Init()
{
	Scene::Init();


	{
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
		LightManager::main->PushLight(light, SceneType::UITest);
		LightManager::main->_lightParmas.mainLight = *light.get();
	}

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap", L"cubemap.hlsl", GeoMetryProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap", L"Textures/cubemap/ocean_env.dds", TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("g_tex_0", texture);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
		meshRenderer->SetCulling(false);
	}

	{
		shared_ptr<GameObject> root = CreateGameObject(L"Inventory");
		auto& renderer = root->AddComponent<MeshRenderer>();
		auto& inventory = root->AddComponent<Inventory>();

		inventory->SetLocalPos(vec3(0, 0, 0.3f));
		inventory->SetSize(vec2(300, 300));
		inventory->SetTexture(ResourceManager::main->Load<Texture>(L"inventory", L"Textures/start.jpg"));
		inventory->AddAction(make_shared<DragAction>(KeyCode::LeftMouse));

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
		material->SetPass(RENDER_PASS::UI);
		renderer->AddMaterials({ material });

		{
			inventory->AddItem(ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg"));
			inventory->AddItem(ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/start.jpg"));
			inventory->AddItem(ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/start.jpg"));
			inventory->AddItem(ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/start.jpg"));
			inventory->AddItem(ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/start.jpg"));
			inventory->AddItem(ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/start.jpg"));
			inventory->AddItem(ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/start.jpg"));
			inventory->AddItem(ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/start.jpg"));
		}

	}



	//{
	//	shared_ptr<GameObject> root = CreateGameObject(L"Sprite1");
	//	auto& renderer = root->AddComponent<MeshRenderer>();
	//	auto& sprite = root->AddComponent<Sprite>();
	//	sprite->AddAction(make_shared<DragAction>(KeyCode::LeftMouse));
	//	sprite->SetLocalPos(vec3(300, 0, 0));
	//	sprite->SetSize(vec2(100, 100));
	//	sprite->SetTexture(ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg"));

	//	shared_ptr<Material> material = make_shared<Material>();
	//	material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
	//	material->SetPass(RENDER_PASS::UI);
	//	renderer->AddMaterials({ material });


	//	{
	//		shared_ptr<GameObject> child = CreateGameObject(L"Sprite2");
	//		auto& renderer = child->AddComponent<MeshRenderer>();
	//		auto& sprite = child->AddComponent<Sprite>();
	//		child->SetParent(root);
	//		sprite->SetLocalPos(vec3(-100, 0, 0));
	//		sprite->SetSize(vec2(50, 50));
	//		sprite->SetTexture(ResourceManager::main->Load<Texture>(L"disable", L"Textures/disable.png"));

	//		shared_ptr<Material> material = make_shared<Material>();
	//		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
	//		material->SetPass(RENDER_PASS::UI);
	//		renderer->AddMaterials({ material });

	//		{
	//			shared_ptr<GameObject> child2 = CreateGameObject(L"Sprite3");
	//			auto& renderer = child2->AddComponent<MeshRenderer>();
	//			auto& sprite = child2->AddComponent<Sprite>();
	//			child2->SetParent(child);
	//			sprite->SetSize(vec2(50, 50));
	//			sprite->SetLocalPos(vec3(100, 0, 0));
	//			sprite->SetTexture(ResourceManager::main->Load<Texture>(L"disable", L"Textures/disable.png"));


	//			shared_ptr<Material> material = make_shared<Material>();
	//			material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
	//			material->SetPass(RENDER_PASS::UI);
	//			renderer->AddMaterials({ material });

	//		}
	//	};
	//}

	//{
	//	shared_ptr<GameObject> root = CreateGameObject(L"SpriteTest");
	//	auto& renderer = root->AddComponent<MeshRenderer>();

	//	auto& sprite = root->AddComponent<TextSprite>();
	//	sprite->SetLocalPos(vec3(0,0,0));
	//	sprite->SetSize(vec2(0.2f, 0.2f));
	//	sprite->SetText(L"HelloWorld");
	//	sprite->CreateObject(512, 256, L"Arial", FontColor::WHITE, 60);

	//	shared_ptr<Material> material = make_shared<Material>();
	//	material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
	//	material->SetPass(RENDER_PASS::UI);

	//	renderer->AddMaterials({ material });

	//}

	{
		{

			shared_ptr<GameObject> gameObject = CreateGameObject(L"AnimationSprite");
			auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"fire", L"Textures/fire.png");
			auto& sprite = gameObject->AddComponent<AnimationSprite>();
			sprite->SetLocalPos(vec3(0.5f, 0.5f, 0.0001f));
			sprite->SetSize(vec2(0.3f, 0.3f));
			sprite->SetFrameRate(0.001f);
			sprite->SetClipingColor(vec4(0, 0, 0, 0.0f));		https://imagecolorpicker.com/
			sprite->SetTexture(texture);

			const float TextureSize = 512.0f;

			for (int i = 0; i < 5; ++i)
			{
				float add = i * TextureSize / 5;
				for (int j = 0; j < 5; ++j)
				{

					SpriteRect rect;
					rect.left = 0 + j * TextureSize / 5;
					rect.top = add;
					rect.right = rect.left + TextureSize / 5;
					rect.bottom = rect.top + TextureSize / 5;

					sprite->PushUVCoord(rect);
				}
			}

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
			material->SetPass(RENDER_PASS::UI);
			meshRenderer->AddMaterials({ material });

		};

	}
}

void UITestScene::Update()
{
	Scene::Update();
}

void UITestScene::RenderBegin()
{
	Scene::RenderBegin();
}

void UITestScene::Rendering()
{
	Scene::Rendering();
}

void UITestScene::RenderEnd()
{
	Scene::RenderEnd();
}

void UITestScene::Finish()
{
	Scene::Finish();
}
