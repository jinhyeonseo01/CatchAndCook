#include "pch.h"
#include "BoidsMove.h"
#include "Transform.h"
#include <random>
//Cohension 
//Alignment
//Separation

vector<GameObject*>BoidsMove:: _objects;

BoidsMove::BoidsMove()
{

}

BoidsMove::~BoidsMove()
{
}

void BoidsMove::SetDestroy()
{
}

void BoidsMove::Init()
{
    auto& urd = InGameGlobal::main->GetRandomMachine();
    uniform_real_distribution<float> dis(20.0f, 30.0f);
    uniform_real_distribution<float> dis2(-1.0f, 1.0f);
    _objects.emplace_back(GetOwner().get());

	_speed = dis(urd);

    GetOwner()->_transform->SetVelocity({ _speed, _speed,_speed });
	GetOwner()->_transform->SetForward({ dis2(urd), dis2(urd), dis2(urd) });
}

void BoidsMove::Start()
{
}
void BoidsMove::Update()
{
    FindNeighbor();

    // 분리, 정렬, 응집력 계산
    const vec3 separation = Separate();
    const vec3 alignment = Align();
    const vec3 cohesion = Cohesion();

    static float separationWeight = 16.0f;
    static float alignmentWeight = 21.0f;
    static float cohesionWeight = 25.0f;

    ImguiManager::main->alignmentWeight = &alignmentWeight;
    ImguiManager::main->separationWeight = &separationWeight;
    ImguiManager::main->cohesionWeight = &cohesionWeight;

    vec3 combinedForce = separation * separationWeight +
        alignment * alignmentWeight +
        cohesion * cohesionWeight;

    if (combinedForce.LengthSquared() > 0.0f)
        combinedForce.Normalize();

    // 현재 오브젝트의 Transform 가져오기 (최적화)
    auto& transform = GetOwner()->_transform;
    vec3 currentForward = transform->GetForward();

    // 방향 보간 및 정규화
    const float dt = Time::main->GetDeltaTime();
    vec3 newForward = vec3::Lerp(currentForward, combinedForce, 7.0f * dt);

    if (newForward.LengthSquared() > 0.0f)
        newForward.Normalize();

    transform->SetForward(newForward);
    transform->SetLocalPosition(transform->GetLocalPosition() + newForward * _speed * dt);
}

void BoidsMove::Update2()
{

}

void BoidsMove::Enable()
{
}

void BoidsMove::Disable()
{
}

void BoidsMove::Destroy()
{
}

void BoidsMove::RenderBegin()
{
}

void BoidsMove::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void BoidsMove::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

bool BoidsMove::IsExecuteAble()
{
	return false;
}

void BoidsMove::UpdateBoids()
{
    GameObject* object = this->GetOwner().get();
    vec3 pos = GetOwner()->_transform->GetLocalPosition();
    vec3 vel = GetOwner()->_transform->GetVelocity();

    vec3 separation(0.0f, 0.0f, 0.0f);
    vec3 alignment(0.0f, 0.0f, 0.0f);
    vec3 cohesion(0.0f, 0.0f, 0.0f);

    int neighborCount = 0;

    for (int32 j = 0; j < _objects.size(); ++j)
    {
        GameObject* other = _objects[j];

        if (other == object) continue;

        vec3 otherPos = other->_transform->GetLocalPosition();
        vec3 diff = pos - otherPos;

        float distance = diff.Length();

        if (distance > 0.0f && distance < _neighborDist)
        {
            if (distance < _desiredSeparation)
            {
                vec3 diffNormalized = diff / distance; 
                separation += diffNormalized * (1.0f / distance);
            }

            alignment += other->_transform->GetVelocity();
            cohesion += otherPos;
            ++neighborCount;
        }
    }

    if (neighborCount > 0)
    {
        alignment /= static_cast<float>(neighborCount);
        cohesion /= static_cast<float>(neighborCount);
        cohesion = cohesion - pos;
    }
    else
    {
        alignment = vec3(0.0f, 0.0f, 0.0f);
        cohesion = vec3(0.0f, 0.0f, 0.0f);
    }

    static float separationWeight = 32.0f;
    static float alignmentWeight = 10.0f;
    static float cohesionWeight = 32.0f;

    ImguiManager::main->alignmentWeight = &alignmentWeight;
    ImguiManager::main->separationWeight = &separationWeight;
    ImguiManager::main->cohesionWeight = &cohesionWeight;


    vec3 acceleration(0.0f, 0.0f, 0.0f);
    acceleration += separation * separationWeight;
    acceleration += alignment * alignmentWeight;
    acceleration += cohesion * cohesionWeight;


    //if (acceleration.Length() > _maxForce) {
    //    acceleration = (acceleration / acceleration.Length()) * _maxForce;
    //}

    float deltaTime = Time::main->GetDeltaTimeNow();
    vel += acceleration * deltaTime;

    float speed = vel.Length();
    if (speed > _maxSpeed) {
        vel = (vel / speed) * _maxSpeed;
    }

    pos += vel * deltaTime;

	this->GetOwner()->_transform->SetForward(vel);
    this->GetOwner()->_transform->SetLocalPosition(pos);
    this->GetOwner()->_transform->SetVelocity(vel);
}



void BoidsMove::FindNeighbor()
{
    _neighbors.clear();

    for (int i = 0; i < _objects.size(); ++i)
    {

		if (_objects[i] == GetOwner().get())
		{
			continue;
		}
		
		float distance = (_objects[i]->_transform->GetLocalPosition() - GetOwner()->_transform->GetLocalPosition()).Length();

		if (distance > 0.0f && distance < _neighborDist)
		{
			_neighbors.emplace_back(_objects[i]);
		}
    }

}

vec3 BoidsMove::Separate()
{
    vec3 separationVec = vec3::Zero;
    int count = 0;

    if (_neighbors.size() > 0)
    {
        for (int i = 0; i < _neighbors.size(); i++)
        {
            vec3 diff = this->GetOwner()->_transform->GetLocalPosition() - _neighbors[i]->_transform->GetLocalPosition();
            float distance = diff.Length();

            // 너무 가까운 경우, 강한 회피 벡터 적용
            if (distance > 0.0f && distance < _desiredSeparation)
            {
                separationVec += diff / (distance * distance); // 거리의 제곱을 나눠서 가까울수록 강한 영향
                count++;
            }
        }

        if (count > 0)
        {
            separationVec /= static_cast<float>(count); // 평균 계산
        }
    }

    separationVec.Normalize();

    return separationVec;
}

vec3 BoidsMove::Align()
{
 
    vec3 alignmentVec = GetOwner()->_transform->GetForward();

    if (_neighbors.size() > 0)
    {
        for (int i = 0; i < _neighbors.size(); i++)
        {
			alignmentVec += _neighbors[i]->_transform->GetForward();
        }
    }
    else
    {
        return alignmentVec;
    }

    alignmentVec /= _neighbors.size();
    alignmentVec.Normalize();
    return alignmentVec;


};

vec3 BoidsMove::Cohesion()
{
 
    vec3 cohesionVec = vec3::Zero;

    if (_neighbors.size() > 0)
    {
        for (int i = 0; i < _neighbors.size(); i++)
        {
            cohesionVec += _neighbors[i]->_transform->GetLocalPosition();
        }
    }
    else
    {

        return cohesionVec;
    }

    cohesionVec /= _neighbors.size();
    cohesionVec -= GetOwner()->_transform->GetLocalPosition();
    cohesionVec.Normalize();
    return cohesionVec;
}


vec3 BoidsMove::LimitForce(vec3& force, float maxForce)
{
    force.Normalize();

    float length = force.Length();
    if (length > maxForce) {
        return force * maxForce;
    }
    return force;
};
