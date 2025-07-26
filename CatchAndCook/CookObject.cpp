#include "pch.h"
#include "CookObject.h"
#include "Collider.h"
#include "GUICookProgress.h"
#include "GUIInventory.h"
#include "GUIItemBox.h"
#include "PhysicsComponent.h"
#include "Transform.h"
#include "AnimationSpriteComponent.h"
#include "MeshRenderer.h"
#include  "BilboardComponent.h"
COMPONENT(CookObject)

bool CookObject::HasOwnItem()
{
	return ownItemData.itemCode != -1;
}

bool CookObject::PushItemData(const ItemData& item)
{
	if (ownItemData.itemCode == -1)
	{
		ownItemData = item;
		return true;
	}
	return false;
}

ItemData CookObject::PopItemData()
{
	ItemData temp = ownItemData;
	ownItemData.Clear();
	return temp;
}

CookObject::~CookObject()
{
}

bool CookObject::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void CookObject::Init()
{
	Component::Init();
}

void CookObject::Start()
{
	Component::Start();
	
	{
		auto object = GetOwner()->GetChildByNameRecursive(L"particle");

		if (object)
		{
			_particleGenPos = object->_transform->GetWorldPosition();
		}

	
	}

	{
		vector<shared_ptr<GameObject>> objects = GetOwner()->GetChildByNameRecursiveAll(L"animationSprite");

		if (objects.empty() == false)
		{
			if (GetOwner()->HasTag(GameObjectTag::CookType_Cut))
			{

			}

			if (GetOwner()->HasTag(GameObjectTag::CookType_Bake))
			{
				vector<shared_ptr<Texture>> _textures;
				std::wstring path = L"../Resources/Textures/Sprite/jin/fire/";

				for (const auto& entry : fs::directory_iterator(path))
				{
					std::wstring& path2 = entry.path().filename().wstring();
					shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(path + path2, path + path2);
					_textures.push_back(texture);
				}

				for (auto& ele : objects)
				{
					ele->GetComponent<MeshRenderer>()->GetMaterial(0)->SetPass(RENDER_PASS::Forward);
					ele->AddComponent<BilboardComponent>();
					auto animationSpriteComponent = ele->AddComponent<AnimationSpriteComponent>();
					animationSpriteComponent->SetTextures(_textures);
					animationSpriteComponent->SetRoop(true);
					ele->SetActiveSelf(false);
					_animationSprites.push_back(ele);
				}
			}

			if (GetOwner()->HasTag(GameObjectTag::CookType_Wash))
			{

			}

			if (GetOwner()->HasTag(GameObjectTag::CookType_Boil))
			{

				GetOwner()->GetChildByName(L"kk")->GetComponent<MeshRenderer>()->GetMaterial(0)->SetPass(RENDER_PASS::Forward);

				vector<shared_ptr<Texture>> _textures;
				std::wstring path = L"../Resources/Textures/Sprite/jin/fire/";

				for (const auto& entry : fs::directory_iterator(path))
				{
					std::wstring& path2 = entry.path().filename().wstring();
					shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(path + path2, path + path2);
					_textures.push_back(texture);
				}

				for (auto& ele : objects)
				{
					ele->GetComponent<MeshRenderer>()->GetMaterial(0)->SetPass(RENDER_PASS::Forward);
					ele->AddComponent<BilboardComponent>();
					auto animationSpriteComponent = ele->AddComponent<AnimationSpriteComponent>();
					animationSpriteComponent->SetTextures(_textures);
					animationSpriteComponent->SetRoop(true);
					ele->SetActiveSelf(false);
					_animationSprites.push_back(ele);
				}
			}
		}
	}

	





}

void CookObject::Update()
{
	Component::Update();

	if (cookType == 0 && onTrigger)
	{
		if (Input::main->GetKeyDown(KeyCode::F))
		{

			std::vector<shared_ptr<GameObject>> cookUIs;
			GetOwner()->GetScene()->Finds(L"CookUI_Cooking", cookUIs);
			if (cookUIs.size() != 0)
			{
				for (auto& ui : cookUIs)
					if (ui->GetActiveSelf() == false)
						selectedUI = ui;
				if (selectedUI && GUIInventory::main && GUIInventory::main->GetItemDataIndex(GUIInventory::main->selectIndex).itemCode != -1
					&& GUIInventory::main && GUIInventory::main->GetItemDataIndex(GUIInventory::main->selectIndex).itemCookType != 2
					&& !HasOwnItem())
				{
					if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
						target->SetActiveSelf(false);

					PushItemData(GUIInventory::main->PopItemDataIndex(GUIInventory::main->selectIndex));

					selectedUI->SetActiveSelf(true);
					selectedUI->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.75);
					cookType = 1;

					auto process = selectedUI->GetComponent<GUICookProgress>();
					process->type = 0;

					//KSH 
					for (auto& ele : _animationSprites)
					{
						ele->SetActiveSelf(true);
					}

					if (GetOwner()->HasTag(GameObjectTag::CookType_Cut))
					{
						process->SetMaxTime(7);
					}
					if (GetOwner()->HasTag(GameObjectTag::CookType_Bake))
					{
						process->SetMaxTime(11);
						process->type = 2;

						//KSH
						ParticleManager::main->GenParticle(20.0f, 200, 0.15f, 0.3f, _particleGenPos, vec3(0, 0, 0), ParticleMoveType::CookFire, ParticleColorType::Red, { 0,0,0,0 }
						, ResourceManager::main->Get<Texture>(L"smokeTexture"));
					}
					if (GetOwner()->HasTag(GameObjectTag::CookType_Wash))
					{
						process->SetMaxTime(5);

						//KSH
						ParticleManager::main->GenParticle(8.0f, 200, 0.15f, 0.3f, _particleGenPos, vec3(0, 0, 0), ParticleMoveType::CookFire, ParticleColorType::Red, { 0,0,0,0 }
						, ResourceManager::main->Get<Texture>(L"bubble"));
					}
					if (GetOwner()->HasTag(GameObjectTag::CookType_Boil))
					{
						process->SetMaxTime(9);
						process->type = 1;
						//KSH
						ParticleManager::main->GenParticle(15.0f, 200, 0.15f, 0.3f, _particleGenPos, vec3(0, 0, 0), ParticleMoveType::CookFire, ParticleColorType::Red, { 0,0,0,0 }
						, ResourceManager::main->Get<Texture>(L"smokeTexture"));

					}
					process->Clear();
					process->Play();
				}
			}
		}
	}
	if (cookType == 1)
	{
		auto process = selectedUI->GetComponent<GUICookProgress>();


		if (process->IsSuccessed())
		{
			cookType = 2;

			//KSH
			for (auto& ele : _animationSprites)
			{
				ele->SetActiveSelf(false);
			}

			//KSH (성공시 이펙트인데 밑으로 옮겨야되는데 어디로 옮겨야될지모르겠음.)


		}
		
	}

	if (cookType == 2 && onTrigger)
	{
		if (Input::main->GetKeyDown(KeyCode::F))
		{
			cookType = 0;
			auto process = selectedUI->GetComponent<GUICookProgress>();
			process->Clear();
			selectedUI->SetActiveSelf(false);
			selectedUI = nullptr;

			if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
			{
				target->SetActiveSelf(true);
				target->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.65);
			}

			auto itemData = PopItemData();
			ItemData nextItemData = itemData;
			vector<ItemData> resultItems;


			bool isFish = (itemData.itemCode == 3 || itemData.itemCode == 4);
			bool isMeat = (itemData.itemCode == 0 || itemData.itemCode == 1 || itemData.itemCode == 2);
			bool isNonCook = (itemData.itemCookType == 1 || itemData.itemCookType == -1);

			if (GetOwner()->HasTag(GameObjectTag::CookType_Cut))
			{
				nextItemData.itemCookType = 2;

				if ((isFish) && // 고기
					isNonCook) {// 씻거나, 기본
					nextItemData.itemCode = 0;
					nextItemData.itemCookType = -1;

					if (itemData.itemCode == 3)
						nextItemData.itemCode = 0;
					if (itemData.itemCode == 4)
						nextItemData.itemCode = 1;
					for (int i = 0; i < 2; i++)
						resultItems.push_back(nextItemData);
				}
				else
					resultItems.push_back(nextItemData);
			}

			if (GetOwner()->HasTag(GameObjectTag::CookType_Bake))
			{

				nextItemData.itemCookType = 0;
				if (itemData.itemCookType == 3) // 실패
					nextItemData.itemCookType = 2;
				resultItems.push_back(nextItemData);
			}
			if (GetOwner()->HasTag(GameObjectTag::CookType_Wash))
			{
				nextItemData.itemCookType = 1;
				resultItems.push_back(nextItemData);
			}
			if (GetOwner()->HasTag(GameObjectTag::CookType_Boil))
			{
				nextItemData.itemCookType = 3;
				if (itemData.itemCookType == 1)  // 실패
					nextItemData.itemCode = 2;

				if (itemData.itemCode == 7 && itemData.itemCookType == 1) {
					nextItemData.itemCode = 8; nextItemData.itemCookType = -1;
				}
				resultItems.push_back(nextItemData);
			}
		


			for (auto& item : resultItems)
			{
				if (GUIInventory::main->HasEmptySlot())
				{
					GUIInventory::main->PushItemData(item);
				}
				else if (GUIItemBox::main->HasEmptySlot())
				{
					GUIItemBox::main->PushItemData(item);
				}
			}
		}
	}
}

void CookObject::Update2()
{
	Component::Update2();
}

void CookObject::Enable()
{
	Component::Enable();
}

void CookObject::Disable()
{
	Component::Disable();
}

void CookObject::RenderBegin()
{
	Component::RenderBegin();
}

void CookObject::RenderEnd()
{
	Component::RenderEnd();
}

void CookObject::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				if (cookType == 0)
				{
					std::vector<std::shared_ptr<GameObject>> nameList;
					if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
					{
						target->SetActiveSelf(true);
						target->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.65);
					}
				}
				onTrigger = true;
			}
		}
	}
}

void CookObject::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)

{
	Component::CollisionEnd(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				if (cookType == 0)
				{
					std::vector<std::shared_ptr<GameObject>> nameList;
					if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
					{
						target->SetActiveSelf(false);
					}
				}

				onTrigger = false;
			}
		}
	}
}

void CookObject::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void CookObject::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void CookObject::SetDestroy()
{
	Component::SetDestroy();
}

void CookObject::Destroy()
{
	Component::Destroy();
}
