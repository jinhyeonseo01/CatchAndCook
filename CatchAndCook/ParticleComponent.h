#pragma once

struct ParticleData
{
	vec3 color{};
	float lifeTime{};

	vec3 worldPos{};
	float size{};

	vec3 dir{};
	float velocity{};
};





struct ParicleHelperParams
{
	int paricleCount{};
	int textureUse = 0;
	vec2 padding{};
	vec4 clipingColor{};

	float changeDirTime{};
	float pp1;
	float pp2;
	float pp3;
};


enum class ParticleMoveType
{
	RadialSpread,
	BloodUnderwater,
	CookFire,

};

enum class ParticleColorType
{
	Random,
	Black,
	White,
	Red,
	Blue,
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

	int GetParicleCount() { return _helperParams.paricleCount; }
	const ParicleHelperParams& GetParicleHelperParams() { return _helperParams; }
	shared_ptr<StructuredBuffer>& GetStructuredBuffer() { return _strBuffer; }


private:
	void SetParticle(shared_ptr<StructuredBuffer> strBuffer,
		float autoDestroyTime,int particleCount,float size ,float speed, const vec3& worldPos,const  vec3& worldNormal , const ParticleMoveType& moveType, const ParticleColorType& colorType, shared_ptr<Texture> texture,
		 const vec4& clipingColor,float dirChangeTime);

public:
	void SetBlendFactor(const std::array<float, 4>& factor) { _blendFactor = factor; }
	const std::array<float, 4>& GetBlendFactor() const { return _blendFactor; }


private:
	float _autoDestroyTime = 0;
	float _currTime = 0;

	std::array<float, 4> _blendFactor = { 0.5f, 0.5f, 0.5f, 0.5f };

	ParicleHelperParams _helperParams{};
	shared_ptr<StructuredBuffer> _strBuffer;

	friend class ParticleManager;
};

