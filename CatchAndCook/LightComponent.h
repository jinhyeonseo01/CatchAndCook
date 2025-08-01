#pragma once
#include "LightManager.h"


class LightComponent : public Component
{
public:
	~LightComponent() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void SetDestroy() override;
	void Destroy() override;

public:
	void SetSkyTime(float currentSkyTime){ this->skyTime = currentSkyTime; };
	void SetLight(const std::shared_ptr<Light>& light) { this->light = light; };
	std::shared_ptr<Light>& GetLight() { return light; };

public:
	std::shared_ptr<Light> light;

public:
	LIGHT_TYPE type;
	Vector3 color = Vector3::One;
	float intensity = 1;
	float range = 1;
	float innerSpotAngle = 360 * D2R;
	float spotAngle = 360 * D2R;
	float shadowAngle = 360 * D2R;

	float skyTime = -1;

	
	static std::weak_ptr<LightComponent> mainLight;
	static std::shared_ptr<LightComponent> GetMainLight() { return mainLight.lock(); }


};

