#pragma once

struct ParticleData
{
	vec3 color{};
	int life=1.0f;

	vec3 worldPos{};
	float p1{};

	vec3 dir{};
	float velocity{};
};


enum class ParticleMoveType
{
	RadialSpread,
};

enum class ParticleColorType
{
	Random,
	Black,
	White,
	Red,
};

class ParticleComponent : public Component
{

public:
	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Update2();
	virtual void Enable();
	virtual void Disable();
	virtual void RenderBegin();
	virtual void RenderEnd();
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current);
	virtual void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene);
	virtual void SetDestroy() override;
	virtual void Destroy();

public:
	void SetParticle(shared_ptr<StructuredBuffer> strBuffer,
		float autoDestroyTime,int particleCount,vec3 worldPos,ParticleMoveType moveType, ParticleColorType colorType);

private:
	int _particleCount = 0;
	float _autoDestroyTime = 0;
	float _currTime = 0;
	shared_ptr<StructuredBuffer> _strBuffer;
};

