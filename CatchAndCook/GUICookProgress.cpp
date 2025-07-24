#include "pch.h"
#include "GUICookProgress.h"
#include "RectTransform.h"


COMPONENT(GUICookProgress)

void GUICookProgress::Clear()
{
	isSuccess = false;
	time = 0;
	isPlay = false;
}

void GUICookProgress::Play()
{
	isSuccess = true;
	isPlay = true;

	GetOwner()->GetChildByName(to_wstring(std::format("Text_{0}", type)))->SetActiveSelf(true);
	GetOwner()->GetChildByName(L"Text_End")->SetActiveSelf(false);
}

GUICookProgress::~GUICookProgress()
{
}

bool GUICookProgress::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUICookProgress::Init()
{
	Component::Init();
}

void GUICookProgress::Start()
{
	Component::Start();
	vector<std::shared_ptr<GameObject>> cookUIs;
	GetOwner()->GetChildsAllByName(L"Progress", cookUIs);
	if (cookUIs.size() != 0)
		progressBar = cookUIs[0]->GetComponent<RectTransform>();

	maxWidth = progressBar->_rectTransformData.rectSize.x;
}

void GUICookProgress::Update()
{
	Component::Update();

	if (isPlay) {
		time += Time::main->GetDeltaTime();

		if (!IsSuccessed())
		{
			progressBar->_rectTransformData.rectSize.x = GetProgress() * maxWidth;
			//progressBar->_rectTransformData.rectSize = w;
		}
		else
		{
			GetOwner()->GetChildByName(to_wstring(std::format("Text_{0}", type)))->SetActiveSelf(false);
			GetOwner()->GetChildByName(L"Text_End")->SetActiveSelf(true);
			isPlay = false;
		}
	}
}

void GUICookProgress::Update2()
{
	Component::Update2();
}

void GUICookProgress::Enable()
{
	Component::Enable();
}

void GUICookProgress::Disable()
{
	Component::Disable();
}

void GUICookProgress::RenderBegin()
{
	Component::RenderBegin();
}

void GUICookProgress::RenderEnd()
{
	Component::RenderEnd();
}

void GUICookProgress::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUICookProgress::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUICookProgress::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUICookProgress::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUICookProgress::SetDestroy()
{
	Component::SetDestroy();
}

void GUICookProgress::Destroy()
{
	Component::Destroy();
}
