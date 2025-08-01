#pragma once


class AnimationKeyFrame
{
public:
	AnimationKeyFrame();
	virtual ~AnimationKeyFrame();

	void SetPosition(const vec3& position){
		this->position = position;
	}
	void SetRotation(const Quaternion& rotation){
		this->rotation = rotation;
	}
	void SetScale(const vec3& scale){
		this->scale = scale;
	}

	double _time = 0;
	double _tick = 0;
	union
	{
		vec3 position;
		Quaternion rotation;
		vec3 scale;
	};
};
