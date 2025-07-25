#include "pch.h"
#include "ShowGameMoeny.h"
#include "TextManager.h"
#include "MeshRenderer.h"
void ShowGameMoeny::Destroy()
{
}

ShowGameMoeny::ShowGameMoeny()
{
}

ShowGameMoeny::~ShowGameMoeny()
{
}

void ShowGameMoeny::Init()
{
	{
		shared_ptr<GameObject> text = SceneManager::main->GetCurrentScene()->CreateGameObject(L"goldText");
		auto& renderer = text->AddComponent<MeshRenderer>();
		auto& sprite = text->AddComponent<TextSprite>();

		float Xsize = 0.8f;
		float Ysize = 0.1f;

		sprite->SetLocalPos(vec3(0.06f, 0.03f, 0));
		sprite->SetSize(vec2(Xsize, Ysize));
		/*sprite->SetText(L"3333333333333333333333333333333333333333");*/
		sprite->CreateObject(1920* Xsize, 1080* Ysize, L"Arial", FontColor::WHITE, 60);

		//sprite->SetClipingColor(vec4(0.5f, 0.5f, 0.5f, 0.5f));
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
		material->SetPass(RENDER_PASS::UI);
		renderer->AddMaterials({ material });

		_text = text;
	};
}

void ShowGameMoeny::Start()
{

	
}


void ShowGameMoeny::Update()
{
	if (prevMoney == InGameGlobal::main->gold)
		return;

	prevMoney = InGameGlobal::main->gold;
	_text->GetComponent<TextSprite>()->SetText(std::to_wstring(InGameGlobal::main->gold));

}

void ShowGameMoeny::Update2()
{
}

void ShowGameMoeny::Enable()
{
}

void ShowGameMoeny::Disable()
{
}

void ShowGameMoeny::RenderBegin()
{
}

void ShowGameMoeny::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ShowGameMoeny::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ShowGameMoeny::SetDestroy()
{
}
