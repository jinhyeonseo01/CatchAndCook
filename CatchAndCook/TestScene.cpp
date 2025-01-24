#include "pch.h"
#include "TestScene.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "SpriteRenderer.h"
#include "Sprite.h"
#include "TextManager.h"
#include "SpriteAction.h"
#include "TempRenderer.h"

void TestScene::Init()
{
	Scene::Init();

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"test", L"test.hlsl", StaticProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"test gameObject");
		gameObject->_transform->SetLocalPosition(vec3(0, 0.3f, 0.8f));
		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetInjector({ InjectorManager::main->Get(BufferType::MateriaSubParam) });
		material->SetTexture("g_tex_0", texture);

		meshRenderer->AddMaterials({ material });
		meshRenderer->SetMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}


	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap", L"cubemap.hlsl", StaticProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap", L"Textures/cubemap/output.dds", TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
		meshRenderer->SetDrawNormal(false);

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("g_tex_0", texture);

		meshRenderer->AddMaterials({ material });
		meshRenderer->SetMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::WIREFRAME;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"testgrid", L"sea.hlsl", StaticProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"grid");
		auto meshRenderer = gameObject->AddComponent<TempRenderer>();

		gameObject->_transform->SetLocalPosition(vec3(0, 10.0f, 0));

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("g_tex_0", ResourceManager::main->GetNoneTexture());

		meshRenderer->AddMaterials({ material });
		meshRenderer->SetMesh(GeoMetryHelper::LoadGripMesh(300.0f, 300.0f, 100, 100));
	}


	for (int i = 0; i < 1; ++i)
	{
		shared_ptr<GameObject> gameObject = CreateGameObject(L"SpriteTest");
		auto spriteRender = gameObject->AddComponent<SpriteRenderer>();

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/spriteTest.jpg");
		shared_ptr<BasicSprite> sprite = make_shared<BasicSprite>();

		spriteRender->SetSprite(sprite);
		sprite->AddAction(make_shared<DragAction>(KeyCode::LeftMouse));
		sprite->AddAction(make_shared<EnableDisableKeyAction>(KeyCode::I));

		SpriteRect rect;
		rect.left = 1024 / 7 * i;
		rect.top = 0;
		rect.right = 1024 / 7 * (i + 1);
		rect.bottom = 1024 / 4;
		sprite->SetPos(vec3(0 + i * WINDOW_WIDTH / 5, 0, 0.04f));
		sprite->SetSize(vec2(WINDOW_WIDTH / 5, WINDOW_HEIGHT / 3));
		sprite->SetTexture(texture);
		sprite->SetUVCoord(rect);

		{
			shared_ptr<BasicSprite> childSprite = make_shared<BasicSprite>();
			childSprite->SetPos(vec3(200, 0, 0.03f));
			childSprite->SetSize(vec2(50, 50));
			childSprite->SetTexture(ResourceManager::main->Load<Texture>(L"disable", L"Textures/disable.png"));
			childSprite->AddAction(make_shared<DisableMouseAction>(KeyCode::LeftMouse));
			sprite->AddChildern(childSprite);
		}

	};

	{

		shared_ptr<GameObject> gameObject = CreateGameObject(L"AnimationSprite");
		auto spriteRender = gameObject->AddComponent<SpriteRenderer>();
		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"fire", L"Textures/fire.png");
		shared_ptr<AnimationSprite> sprite = make_shared<AnimationSprite>();

		spriteRender->SetSprite(sprite);
		sprite->SetTexture(texture);
		sprite->SetPos(vec3(0, 0, 0.3f));
		sprite->SetSize(vec2(500, 500));
		sprite->SetFrameRate(0.05f);
		sprite->SetClipingColor(vec4(0, 0, 0, 1.0f));		https://imagecolorpicker.com/

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
	};

	{
		shared_ptr<GameObject> gameObject = CreateGameObject(L"TextTest");
		auto spriteRender = gameObject->AddComponent<SpriteRenderer>();
		shared_ptr<TextSprite> sprite = make_shared<TextSprite>();

		spriteRender->SetSprite(sprite);
		sprite->SetPos(vec3(300.0f, 0.0f, 0.000001));
		sprite->SetSize(vec2(300, 300));
		sprite->SetText(L"Ä³Ä¡¾ØÄî");
		sprite->CreateObject(512, 256, L"Arial", FontColor::WHITE, 123);
	}


};

void TestScene::Update()
{
    Scene::Update();
}

void TestScene::RenderBegin()
{
    Scene::RenderBegin();
}

void TestScene::Rendering()
{
    Scene::Rendering();
}

void TestScene::RenderEnd()
{
    Scene::RenderEnd();


}

void TestScene::Finish()
{
    Scene::Finish();
}
