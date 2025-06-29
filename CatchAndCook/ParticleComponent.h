#pragma once

struct ParticleData
{
	vec3 color{};
	int textureUse=0;

	vec3 worldPos{};
	float size{};

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

	int GetParicleCount() { return _particleCount; }
	shared_ptr<StructuredBuffer>& GetStructuredBuffer() { return _strBuffer; }

private:
	void SetParticle(shared_ptr<StructuredBuffer> strBuffer,
		float autoDestroyTime,int particleCount,float size , const vec3& worldPos,const  vec3& worldNormal , const ParticleMoveType& moveType, const ParticleColorType& colorType, shared_ptr<Texture> texture);

private:
	int _particleCount = 0;
	float _autoDestroyTime = 0;
	float _currTime = 0;
	shared_ptr<StructuredBuffer> _strBuffer;

	friend class ParticleManager;
};

