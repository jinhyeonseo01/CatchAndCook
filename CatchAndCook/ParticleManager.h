#pragma once
#include "ParticleComponent.h"



class ParticleManager
{
public:
	static std::unique_ptr<ParticleManager> main;

public:
	void Init();
	shared_ptr<GameObject> GenParticle(float autodestroyTime, int particleCount, float particleSize, float speed, const vec3& worldPos,const vec3& worldNormal, const ParticleMoveType& moveType, const ParticleColorType& colorType,
		const vec4& clipingColor, shared_ptr<Texture> texture = nullptr , shared_ptr<Shader> compute =nullptr , shared_ptr<Shader> rendering=nullptr, float dirChangeTime = 0);


	shared_ptr<StructuredBuffer> AllocParticleBuffer();
	void RecycleParticleBuffer(shared_ptr<StructuredBuffer> buffer);

public:
	
private:
	static constexpr int _MaxParticle = 500;
	int _currentPoolCount = 0;
private:

	std::queue<shared_ptr<StructuredBuffer>> _strBufferPool;

};

