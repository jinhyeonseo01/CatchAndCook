#include "pch.h"
#include "FishMonster.h"
#include "Transform.h"
#include "Gizmo.h"
#include "AnimationListComponent.h"
#include "SkinnedHierarchy.h"
#include "Animation.h"
unordered_map<wstring, FishPath> FishMonster::_pathList;

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
	_firstQuat = GetOwner()->_transform->GetWorldRotation();
	_animations = GetOwner()->GetComponent<AnimationListComponent>()->GetAnimations();
	_skined = GetOwner()->GetComponent<SkinnedHierarchy>();
    _player = SceneManager::main->GetCurrentScene()->Find(L"seaPlayer");

    if (_animations.find("idle") != _animations.end())
    {
        _skined->Play(_animations["idle"], 0.5f);
    }

    if (_animations.find("die") != _animations.end())
    {
        _animations["die"]->_isLoop = false;
        _animations["die"]->_speedMultiplier = 0.3f;
    }


}

void FishMonster::Update()
{
	float dt = Time::main->GetDeltaTime();

    if (_state == FishMonsterState::Die)
        return;

    if (_pathList.find(_pathName) == _pathList.end())
    {
        cout << "리턴됨" << endl;
        return;
    }

    const vector<vec3>& myPath = _pathList[_pathName].path;
    int nextIndex = _forward ? _currentIndex + 1 : _currentIndex - 1;
    vec3 start = myPath[_currentIndex];
    vec3 end = myPath[nextIndex];

    if (_segmentLength < 0.0001f)
        _segmentLength = (end - start).Length();

    _distanceMoved += _moveSpeed * Time::main->GetDeltaTime();
    float t = std::clamp(_distanceMoved / _segmentLength, 0.0f, 1.0f);
    vec3 targetPos = vec3::Lerp(start, end, t);

    vec3 currentPos = GetOwner()->_transform->GetWorldPosition();

    vec3 toTarget = targetPos - currentPos;
    toTarget.Normalize();

    vec3 desiredVel = toTarget * _moveSpeed;

    vec3 avoidanceVel(0, 0, 0);
    const float detectionRadius = 200.f;
    const float predictTime = 1.0f;
    auto player = _player.lock();

    if (player)
    {
        vec3 playerPos = player->_transform->GetWorldPosition();

        vec3 futurePos = currentPos + desiredVel * predictTime;
        float distFuture = (playerPos - futurePos).Length();

        if (distFuture < detectionRadius)
        {
            vec3 away = (currentPos - playerPos);
            away.Normalize();
            float strength = (detectionRadius - distFuture) / detectionRadius;
            avoidanceVel = away * detectionRadius * strength;
        }

    }


    vec3 velocity = desiredVel + avoidanceVel;
    vec3 newPos = currentPos + velocity * Time::main->GetDeltaTime();
    GetOwner()->_transform->SetWorldPosition(newPos);
    velocity.Normalize();
    GetOwner()->_transform->LookUpSmooth(velocity, vec3::Up, 3.0f, _firstQuat);


    if (t >= 1.0f)
    {
        _distanceMoved = 0.0f;
        _segmentLength = 0.0f;

        if (_forward)
        {
            _currentIndex++;
            if (_currentIndex >= myPath.size() - 1) { _forward = false; return; }
        }
        else
        {
            _currentIndex--;
            if (_currentIndex <= 0) { _forward = true; return; }
        }
    }


    if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::DrawPath))
    {

        for (size_t i = 0; i + 1 < myPath.size(); ++i)
        {
            vec3 c = _pathList[_pathName]._pathColor;
            Gizmo::main->Line(
                myPath[i], myPath[i + 1],
                vec4(c.x, c.y, c.z, 1.0f)
            );
        }

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


void FishMonster::UpdateState(float dt)
{

    static float _hit_to_IdleTime = 0.0f;

    switch (_state)
    {
    case FishMonsterState::Idle:
     
        break;
    case FishMonsterState::Die:
        if (_skined->IsPlay() == false)
        {
            _hit_to_IdleTime = 0;
            GetOwner()->SetActiveSelf(false);
        }
		break;
	case FishMonsterState::Hit:
    {
        _hit_to_IdleTime += dt;
        constexpr float LimitTime = 15.0f;

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
        _moveSpeed /= 4.0f;

        if (_animations.find("idle") != _animations.end())
        {
            _skined->Play(_animations["idle"], 0.5f);
        }
        else
        {
            _skined->_speedMultiple = 1.0f;
        }

		break;
	case FishMonsterState::Die:
        //_moveSpeed = 0.0f;
          /* SceneManager::main->GetCurrentScene()->AddDestroyQueue(ray.gameObject->GetRoot());*/
        if (_animations.find("die") != _animations.end())
        {
            _skined->Play(_animations["die"], 0.5f);
        }
		break;
	case FishMonsterState::Hit:
        _moveSpeed *= 4.0f;
        if (_animations.find("run") != _animations.end())
        {
            _skined->Play(_animations["run"], 0.5f);
        }
        else
        {
            _skined->_speedMultiple = 8.0f;
        }
		break;
	default:
		break;
	}
}

void FishMonster::EventDamage(int damage)
{
    _hp -= damage;

    if (_hp <= 0)
    {
        SetState(FishMonsterState::Die);
    }
    else
    {
        SetState(FishMonsterState::Hit);
    }

}


void FishMonster::ReadPathFile(const std::wstring& fileName)
{
    const wstring path = L"../Resources/Graph/";;

    std::ifstream file(path + fileName);
    if (!file.is_open())
    {
        std::wcout << L"Failed to open file: " << fileName << std::endl;
        return;
    }


    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        std::istringstream ss(line);
        float x, y, z;
        ss >> x >> y >> z;
        vec3 point(x, y, z);
        _pathList[fileName].path.push_back(point);
    }

    file.close();

    _pathName = fileName;

    size_t h = std::hash<wstring>{}(fileName);
    float hue = float(h % 360) / 360.f;
    _pathList[fileName]._pathColor = vec3(hue, hue, hue);
	//cout << "라인 데이터: " << _fishPath.size() << "개 읽음." << std::endl;
}