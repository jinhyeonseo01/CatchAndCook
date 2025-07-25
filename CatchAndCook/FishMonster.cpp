#include "pch.h"
#include "FishMonster.h"
#include "Transform.h"
#include "Gizmo.h"
#include "AnimationListComponent.h"
#include "SkinnedHierarchy.h"
#include "Animation.h"
#include "PercentComponent.h"
#include "GraphPathFinder.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "itemBoxComponent.h"
COMPONENT(FishMonster)

FishMonster::FishMonster()
{

}

FishMonster::~FishMonster()
{
}

void FishMonster::Init()
{

}

void FishMonster::Start()
{
	_animations = GetOwner()->GetComponentWithChilds<AnimationListComponent>()->GetAnimations();

	_skined = GetOwner()->GetRoot()->GetComponentWithChilds<SkinnedHierarchy>();


    if (_animations.find("idle") != _animations.end())
    {
        _skined->Play(_animations["idle"], 0.5f);
    }

    else
    {
        wcout << GetOwner()->GetName() << "idle 못찾음" << endl;
    }

    if (_animations.find("die") != _animations.end())
    {
        _animations["die"]->_isLoop = false;
        _animations["die"]->_speedMultiplier = 0.3f;
    }

    auto percentageComponet = GetOwner()->GetChildByNameRecursive(L"HPBar")->GetComponent<PercentComponent>();

    if (percentageComponet)
    {
        percentageComponet->BindingPercentage(&_hp);
    }

    else
    {
        cout << "percentageComponet" << "못찾음" << endl;
    }

     _pathFinder = GetOwner()->GetRoot()->GetComponent<GraphPathFinder>();

     if (_pathFinder)
     {
         _pathFinder->SetAutoPliotMode(true);
     }
     else
     {
         cout << "GraphPathFinder" << "못찾음" << endl;
     }


     if (GetOwner()->GetName().find(L"ray") == 0)
     {
         if (_animations.find("idle") != _animations.end())
         {
             _animations["idle"]->_speedMultiplier = 2.0f;
         }

         GetOwner()->SetName(L"ray");
     }

     if (GetOwner()->GetName().find(L"redFish") == 0)
     {
         GetOwner()->SetName(L"redFish");
     }

}

void FishMonster::Update()
{
	float dt = Time::main->GetDeltaTime();

    if (_state != FishMonsterState::Die)
    {
        _pathFinder->CalculatePath(_moveSpeed);
    }
}

void FishMonster::Update2()
{
    UpdateState(Time::main->GetDeltaTime());
}

void FishMonster::Enable()
{
}

void FishMonster::Disable()
{
}

void FishMonster::RenderBegin()
{
}

void FishMonster::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void FishMonster::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void FishMonster::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void FishMonster::SetDestroy()
{
}

void FishMonster::Destroy()
{

}

void FishMonster::Reset()
{
    GetOwner()->SetActiveSelf(true);
    SetState(FishMonsterState::Idle);
    _hp = 100.0f;
    _hit_to_IdleTime = 0.0f;
    
}

void FishMonster::UpdateState(float dt)
{

    switch (_state)
    {
    case FishMonsterState::Idle:
     
        break;
    case FishMonsterState::Die:
    
        if (_skined->IsPlay() == false)
        {
            _hit_to_IdleTime = 0;
            GetOwner()->SetActiveSelf(false);
            auto& object = SceneManager::main->GetCurrentScene()->Find(L"itemBox");
           
            if (object)
            {
                object->SetActiveSelf(false);

                auto& exmeshRenderer = object->GetComponent<MeshRenderer>();
                auto& extransform = object->GetComponent<Transform>();

                std::vector<std::shared_ptr<Material>>& materials = exmeshRenderer->GetMaterials();
                std::vector<std::shared_ptr<Mesh>>& meshes = exmeshRenderer->GetMeshes();

                auto& itemBox = SceneManager::main->GetCurrentScene()->CreateGameObject(GetOwner()->GetName());
                auto& meshRenderer = itemBox->AddComponent<MeshRenderer>();
                meshRenderer->SetMesh(meshes);
                meshRenderer->SetMaterials(materials);

                auto& transform = itemBox->GetComponent<Transform>();
                transform->SetLocalScale(extransform->GetLocalScale());
                transform->SetLocalPosition(GetOwner()->_transform->GetWorldPosition());

                auto collider = itemBox->AddComponent<Collider>();
                collider->SetBoundingBox(vec3(0, 0.09795811f, 0), vec3(0.1711108f/2, 0.1970621f/2, 0.1503567f/2));
                collider->SetTrigger(true);
                auto itemboxComponent  = itemBox->AddComponent<itemBoxComponent>();
                
            }

        }

		break;
	case FishMonsterState::Hit:
    {
        _hit_to_IdleTime += dt;
        constexpr float LimitTime = 5.0f;

        if (_hit_to_IdleTime >= LimitTime)
        {
            _hit_to_IdleTime = 0;
            SetState(FishMonsterState::Idle);
        }

    }
		break;
	default:
		break;
	}
}

void FishMonster::SetState(FishMonsterState state)
{
	if (_state == state)
		return;

	_state = state;

	switch (_state)
	{
	case FishMonsterState::Idle:
        _moveSpeed = _originMoveSpeed;

        if (_animations.find("idle") != _animations.end())
        {
            _skined->Play(_animations["idle"], 0.5f);
        }
        if (_skined->_speedMultiple >= 1.5f)
        {
            _skined->_speedMultiple = 1.0f;
        }

		break;
	case FishMonsterState::Die:

        if (_animations.find("die") != _animations.end())
        {
            _skined->Play(_animations["die"], 0.5f);
        }

        ParticleManager::main->GenParticle(8.0f, 500, 30.0f, 130.0f, GetOwner()->_transform->GetWorldPosition(), vec3(0, 0, 0), ParticleMoveType::RadialSpread, ParticleColorType::Red, { 0,0,0,0 }
        , ResourceManager::main->Get<Texture>(L"success"));

		break;
	case FishMonsterState::Hit:
        _moveSpeed *= 2.0f;

        if (_animations.find("run") != _animations.end())
        {
            _skined->Play(_animations["run"], 0.5f);
        }
        else
        {
            _skined->_speedMultiple = 7.0f;
        }
		break;
	default:
		break;
	}
}

void FishMonster::EventDamage(int damage)
{
    _hp -= damage;
    _hit_to_IdleTime = 0;

    if (_hp <= 0)
    {
        SetState(FishMonsterState::Die);
    }
    else
    {
        SetState(FishMonsterState::Hit);
    }

}


