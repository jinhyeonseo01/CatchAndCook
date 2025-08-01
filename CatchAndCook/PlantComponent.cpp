#include "pch.h"
#include "PlantComponent.h"

COMPONENT(PlantComponent)
int PlantComponent::idGenator=1;

PlantComponent::PlantComponent()
{
}

PlantComponent::~PlantComponent()
{
}

void PlantComponent::Init()
{
	cout << " PlantComponent Init" << endl;
	_plantInfo.id = idGenator++;
}

void PlantComponent::Start()
{
	if (auto renderer =GetOwner()->GetRenderer())
	{
		renderer->AddStructuredSetter(static_pointer_cast<PlantComponent>(shared_from_this()), BufferType::SeaPlantParam);
		_renderBase = renderer;
	}
}

void PlantComponent::Update()
{
}

void PlantComponent::Update2()
{
}

void PlantComponent::RenderBegin()
{



}

void PlantComponent::Enable()
{
}

void PlantComponent::Disable()
{
}


void PlantComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void PlantComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

bool PlantComponent::IsExecuteAble()
{
	return false;
}

void PlantComponent::SetDestroy()
{

}

void PlantComponent::SetData(StructuredBuffer* buffer, Material* material)
{


	_plantInfo.amplitude = material->GetPropertyFloat("_Amplitude");
	_plantInfo.frequency = material->GetPropertyFloat("_frequency");

	BoundingBox& box = _renderBase.lock()->GetOriginBound();
	_plantInfo.boundsSizeY =box.Extents.y;
	_plantInfo.boundsCenterY = box.Center.y;

	buffer->AddData(_plantInfo);
}



