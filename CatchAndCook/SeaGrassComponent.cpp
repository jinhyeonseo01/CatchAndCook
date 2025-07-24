#include "pch.h"
#include "SeaGrassComponent.h"

SeaGrassComponent::SeaGrassComponent()
{
}

SeaGrassComponent::~SeaGrassComponent()
{
}

bool SeaGrassComponent::IsExecuteAble()
{

	return Component::IsExecuteAble();
}

void SeaGrassComponent::Init()
{
}

void SeaGrassComponent::Start()
{
	if (auto renderer = GetOwner()->GetRenderer())
	{
		renderer->AddCbufferSetter(static_pointer_cast<SeaGrassComponent>(shared_from_this()));
		_renderBase = renderer;
	}

}

void SeaGrassComponent::Update()
{
}

void SeaGrassComponent::Update2()
{
}

void SeaGrassComponent::Enable()
{
}

void SeaGrassComponent::Disable()
{
}

void SeaGrassComponent::RenderBegin()
{
}

void SeaGrassComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void SeaGrassComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void SeaGrassComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void SeaGrassComponent::SetDestroy()
{
}

void SeaGrassComponent::Destroy()
{
}

void SeaGrassComponent::SetData(Material* material)
{
	_param.amplitude = material->GetPropertyFloat("_Amplitude");
	_param.frequency = material->GetPropertyFloat("_frequency");
	_param.PushPower = material->GetPropertyFloat("_pushPower");


	BoundingBox& box = _renderBase.lock()->GetOriginBound();
	_param.boundsSizeY = box.Extents.y;
	_param.boundsCenterY = box.Center.y;

	auto buffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SeaGrassParam)->Alloc(1);
	memcpy(buffer->ptr, &_param, sizeof(SeaGrassParam));
	int index = material->GetShader()->GetRegisterIndex("SeaGrassParam");

	if (index != -1)
		Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, buffer->GPUAdress);
	
}
