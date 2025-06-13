#include "pch.h"
#include "ParticleManager.h"
#include "ParticleComponent.h"
#include "ParticleRenderer.h"
std::unique_ptr<ParticleManager> ParticleManager::main =nullptr;

void ParticleManager::Init()
{
	for (int i = 0; i < 100; ++i)
	{
		shared_ptr<StructuredBuffer> strBuffer = make_shared<StructuredBuffer>();
		strBuffer->InitDefualtHeap(sizeof(ParticleData), _MaxParticle);
		_strBufferPool.push(strBuffer);
	}

	_currentPoolCount += 100;

}

shared_ptr<GameObject> ParticleManager::GenParticle(float autodestroyTime, int particleCount, vec3 worldPos, ParticleMoveType moveType, ParticleColorType colorType)
{
	shared_ptr<StructuredBuffer> buffer =AllocParticleBuffer();

	shared_ptr<GameObject> object = SceneManager::main->GetCurrentScene()->CreateGameObject(L"Particle");

	auto& particleComponent = object->AddComponent<ParticleComponent>();
	particleComponent->SetParticle(buffer, autodestroyTime, particleCount, worldPos, moveType, colorType);

	object->AddComponent<ParticleRenderer>();

	return object;
}

shared_ptr<StructuredBuffer> ParticleManager::AllocParticleBuffer() 
{
	if (_strBufferPool.empty())
	{
		cout << "버퍼 추가됨" <<"현재 풀갯수"<< _currentPoolCount << endl;
		Init();
	}

	shared_ptr<StructuredBuffer> buffer =  _strBufferPool.front();
	_strBufferPool.pop();
	return buffer;
}

void ParticleManager::RecycleParticleBuffer(shared_ptr<StructuredBuffer> buffer)
{
	_strBufferPool.push(buffer);
}






