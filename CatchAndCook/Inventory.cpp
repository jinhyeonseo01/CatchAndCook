#include "pch.h"
#include "Inventory.h"
#include "MeshRenderer.h"

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
}

void Inventory::Init()
{
	Sprite::Init();


	
}

void Inventory::Start()
{
	Sprite::Start();
}

void Inventory::Update()
{
	Sprite::Update();
}

void Inventory::Update2()
{
	Sprite::Update2();
}

void Inventory::Enable()
{
	Sprite::Enable();
}

void Inventory::Disable()
{
	Sprite::Disable();
}

void Inventory::RenderBegin()
{
	Sprite::RenderBegin();
}

void Inventory::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Inventory::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Inventory::SetDestroy()
{
	Sprite::SetDestroy();
}

void Inventory::Destroy()
{
	Sprite::Destroy();
}

void Inventory::SetData(Material* material)
{
	Sprite::SetData(material);

}

void Inventory::AddItem(const std::shared_ptr<Texture>& itemTexture)
{
	auto& itemObj = SceneManager::main->GetCurrentScene()->CreateGameObject(L"item");
	itemObj->SetParent(GetOwner());

	auto& renderer = itemObj->AddComponent<MeshRenderer>();
	auto& sprite = itemObj->AddComponent<Sprite>();
	sprite->SetTexture(itemTexture);
	sprite->SetSize(vec2(_cellsize.x, _cellsize.y));
	sprite->AddAction(make_shared<DragAction>(KeyCode::LeftMouse));
	vec2 itemIndex = vec2(_itemCount % 5, _itemCount / 5);
	sprite->SetLocalPos(vec3(_startPos.x+ itemIndex.x * 45, _startPos.y+ itemIndex.y * 45, 0.3f));
	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
	material->SetPass(RENDER_PASS::UI);
	renderer->AddMaterials({ material });
	_itemCount++;
	_itemList.push_back(itemObj);
}

