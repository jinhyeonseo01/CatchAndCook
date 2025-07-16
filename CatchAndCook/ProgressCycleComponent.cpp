#include "pch.h"
#include "ProgressCycleComponent.h"

void ProgressCycleComponent::Init()
{
}

void ProgressCycleComponent::Start()
{

	if (auto renderer =GetOwner()->GetRenderer())
	{
		renderer->AddCbufferSetter(static_pointer_cast<ProgressCycleComponent>(shared_from_this()));
	}

}

void ProgressCycleComponent::Update()
{
	SetCurrentUV();
}

void ProgressCycleComponent::Update2()
{
}

void ProgressCycleComponent::Enable()
{
}

void ProgressCycleComponent::Disable()
{
}

void ProgressCycleComponent::RenderBegin()
{
}

void ProgressCycleComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ProgressCycleComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ProgressCycleComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void ProgressCycleComponent::SetDestroy()
{
}

void ProgressCycleComponent::Destroy()
{
}

void ProgressCycleComponent::SetData(Material* material)
{
	auto buffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::ProgressCircle)->Alloc(1);
	memcpy(buffer->ptr, &_data, sizeof(_data));
	int index = material->GetShader()->GetRegisterIndex("PROGRESS_PARAM");
	if (index != -1)
	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, buffer->GPUAdress);
}

bool ProgressCycleComponent::isInAnswer()
{
	return _data.IsCorrect();
}

void ProgressCycleComponent::SetCurrentUV()
{
	constexpr float speed = 0.3f;
	static bool isGoingUp = true;
	float range = _data.answerCorrectRange * 0.5f;
	float minUV = range;
	float maxUV = 1.0f - range;

	_data.currentUV += (isGoingUp ? 1.0f : -1.0f) * Time::main->GetDeltaTime() * speed;
	_data.currentUV = clamp(_data.currentUV, minUV, maxUV);

	if (_data.currentUV == maxUV)
		isGoingUp = false;
	else if (_data.currentUV == minUV)
		isGoingUp = true;

}
