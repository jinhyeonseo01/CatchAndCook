#pragma once
#include "ParticleComponent.h"



class ParticleManager
{
public:
	static std::unique_ptr<ParticleManager> main;

public:
	void Init();
	shared_ptr<GameObject> GenParticle(float autodestroyTime, int particleCount, float particleSize, vec3 worldPos, ParticleMoveType moveType, ParticleColorType colorType);
	shared_ptr<StructuredBuffer> AllocParticleBuffer();
	void RecycleParticleBuffer(shared_ptr<StructuredBuffer> buffer);

public:
	
private:
	static constexpr int _MaxParticle = 500;
	int _currentPoolCount = 0;
private:
	std::queue<shared_ptr<StructuredBuffer>> _strBufferPool;

};

