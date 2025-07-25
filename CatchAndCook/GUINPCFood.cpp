#include "pch.h"
#include "GUINPCFood.h"

#include "GUIInventory.h"
#include "Transform.h"


COMPONENT(GUINPCFood)


void GUINPCFood::SetFood()
{
	vector<ItemData> itemDatas;
	itemDatas.push_back((ItemData(10, -1)));
	itemDatas.push_back((ItemData(11, -1)));
	itemData = itemDatas[rand() % itemDatas.size()];

	auto item = GetOwner()->GetChildByName(L"Slot_0")->GetComponent<GUIItem>();
	item->PopItemData();
	item->PushItemData(itemData);

	eat = false;
	player = GetOwner()->GetScene()->Find(L"player");
	GetOwner()->GetChildByName(L"Text")->SetActiveSelf(false);
}

GUINPCFood::~GUINPCFood()
{
}

bool GUINPCFood::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUINPCFood::Init()
{
	Component::Init();
}

void GUINPCFood::Start()
{
	Component::Start();
}

void GUINPCFood::Update()
{
	Component::Update();

	Vector3 playerPos = player->_transform->GetWorldPosition();
	Vector3 myPos = GetOwner()->_transform->GetWorldPosition();
	playerPos.y = 0;
	myPos.y = 0;

	bool nearDis = false;
	if (Vector3::Distance(playerPos, myPos) <= 1.0f)
	{
		if (GUIInventory::main->GetItemDataIndex(GUIInventory::main->selectIndex) == itemData)
		{
			nearDis = true;
		}
	}
	GetOwner()->GetChildByName(L"Text")->SetActiveSelf(nearDis);
	if (nearDis && Input::main->GetKeyDown(KeyCode::F)) {
		eat = true;
		GUIInventory::main->PopItemDataIndex(GUIInventory::main->selectIndex);
	}
}

void GUINPCFood::Update2()
{
	Component::Update2();
}

void GUINPCFood::Enable()
{
	Component::Enable();
}

void GUINPCFood::Disable()
{
	Component::Disable();
	player = nullptr;
}

void GUINPCFood::RenderBegin()
{
	Component::RenderBegin();
}

void GUINPCFood::RenderEnd()
{
	Component::RenderEnd();
}

void GUINPCFood::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUINPCFood::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUINPCFood::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUINPCFood::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUINPCFood::SetDestroy()
{
	Component::SetDestroy();
}

void GUINPCFood::Destroy()
{
	Component::Destroy();
}

void GUINPCFood::Reset()
{
	Component::Reset();
}
