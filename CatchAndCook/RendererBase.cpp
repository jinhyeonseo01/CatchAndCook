#include "pch.h"
#include "RendererBase.h"


RenderStructuredSetter::~RenderStructuredSetter()
{

}

RenderCBufferSetter::~RenderCBufferSetter()
{
}

void RendererBase::AddStructuredSetter(const std::shared_ptr<RenderStructuredSetter>& setter, BufferType type)
{
	auto it = _structuredSetters.find(type);
	if(it == _structuredSetters.end())
	{
		_structuredSetters.emplace(type, setter);
		//type _structuredSetters
	}
}

void RendererBase::RemoveStructuredSetter(const shared_ptr<RenderStructuredSetter>& object)
{
	for(auto it = _structuredSetters.begin(); it != _structuredSetters.end(); )
	{
		if(it->second == object)
			it = _structuredSetters.erase(it); // 삭제 후 iterator를 반환받음
		else
			++it;
	}
}

void RendererBase::RemoveStructuredSetter(const BufferType& type)
{
	for (auto it = _structuredSetters.begin(); it != _structuredSetters.end(); )
	{
		if (it->first == type)
			it = _structuredSetters.erase(it); // 삭제 후 iterator를 반환받음
		else
			++it;
	}
}

void RendererBase::AddCbufferSetter(const std::shared_ptr<RenderCBufferSetter> setter)
{
	auto it = ranges::find(_cbufferSetters, setter);
	if(it == _cbufferSetters.end())
		_cbufferSetters.push_back(setter);
}

void RendererBase::RemoveCbufferSetter(const shared_ptr<RenderCBufferSetter> object)
{
	auto it = ranges::find(_cbufferSetters, object);
	if(it != _cbufferSetters.end())
		_cbufferSetters.erase(it);
}

std::shared_ptr<RenderStructuredSetter>& RendererBase::FindStructuredSetter(const BufferType& type)
{
	return _structuredSetters[type];
}

void RendererBase::SetInstanceBuffer(InstanceBufferContainer* instanceBuffer)
{
	_staticInstanceBuffer = instanceBuffer;
}

bool RendererBase::HasInstanceBuffer()
{
	return _staticInstanceBuffer != nullptr;
}
