#pragma once

#include "StructuredBuffer.h"


struct InstanceOffsetParam
{

	//std::array<int, 128> offsets;
	vec4 offset[10];

};
union InstanceID
{
	struct
	{
		uint32 meshID;
		uint32 materialID;
	};

	uint64 id;
};

class InstancingManager
{

public:
	static unique_ptr<InstancingManager> main;
	
public:
	void RenderNoInstancing(RenderObjectStrucutre& RoS);
	void Render();
	void Clear();
	void AddObject(RenderObjectStrucutre& obj);
	

private:
	map<uint64,vector<RenderObjectStrucutre>> _objectMap;

};

