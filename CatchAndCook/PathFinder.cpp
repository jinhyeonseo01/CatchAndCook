#include "pch.h"
#include "PathFinder.h"
#include "Transform.h"
#include "Gizmo.h"
#include "simple_mesh_ext.h"
#include <random>
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "RendererBase.h"
unordered_map<wstring, FishPath> PathFinder::_pathList;


static uniform_real_distribution<float> randomMoveSpeed(0.5f, 2.0f);
static uniform_real_distribution<float> randomSpeed(0.7, 1.3f);

COMPONENT(PathFinder)

PathFinder::PathFinder()
{
}

PathFinder::~PathFinder()
{
}

void PathFinder::Init()
{
	
}

void PathFinder::Start()
{

   _firstQuat = GetOwner()->_transform->GetWorldRotation();

   auto renderer = GetOwner()->GetComponentWithChilds<MeshRenderer>();

   if (renderer)
   {

       renderer->AddStructuredSetter(
           std::static_pointer_cast<PathFinder>(shared_from_this()),
           BufferType::SeaFIshParam);


       const auto& materials = renderer->GetMaterials();

       if (materials.empty())
           return;

       const auto& mat = materials[0];

       auto& dre = InGameGlobal::main->GetRandomMachine();

       int pathIndex = static_cast<int>(mat->GetPropertyFloat("_Path"));
       std::wstring pathName = L"path" + std::to_wstring(pathIndex);
       SetPass(pathName);

       _moveSpeed = mat->GetPropertyFloat("_MoveSpeed") * randomMoveSpeed(dre);
       _info.fishSpeed = mat->GetPropertyFloat("_Speed");
       _info.fishWaveAmount = mat->GetPropertyFloat("_Power") * randomSpeed(dre);
       const BoundingBox& box = renderer->GetOriginBound();
       _info.boundsSizeZ = box.Extents.z;
       _info.boundsCenterZ = box.Center.z;

       _pathOffset = GenerateRandomPointInSphere(mat->GetPropertyFloat("_Radius"));
       _player = SceneManager::main->GetCurrentScene()->Find(L"seaPlayer");

       const vector<vec3>& myPath = _pathList[_pathName].path;
       _currentIndex = (std::rand() % myPath.size());

       if (_currentIndex >= myPath.size() - 1)
       {
           _forward = false;
       }

       GetOwner()->_transform->SetWorldPosition(myPath[_currentIndex]);
   };


   if (SceneManager::main->GetCurrentScene()->GetSceneType() == SceneType::TestScene2)
   {
       vec3  _pathOffset = vec3(50, 50, 50);

       std::wstring pathName = L"path" + std::to_wstring(99);
       SetPass(pathName);
       SetMoveSpeed(10.0f);



       const vector<vec3>& myPath = _pathList[_pathName].path;
       _currentIndex = (std::rand() % myPath.size());

       if (_currentIndex >= myPath.size() - 1)
       {
           _forward = false;
       }

       GetOwner()->_transform->SetWorldPosition(myPath[_currentIndex]);
   };

    



}
void PathFinder::Update()
{

    if (_pathList.find(_pathName) == _pathList.end())
    {
        return;
    }

    const vector<vec3>& myPath = _pathList[_pathName].path;
    int nextIndex = _forward ? _currentIndex + 1 : _currentIndex - 1;
    vec3 start = myPath[_currentIndex] + _pathOffset;
    vec3 end = myPath[nextIndex] + _pathOffset;

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
        if(!_pathList[_pathName].AreyouDraw)
        {
            for (size_t i = 0; i + 1 < myPath.size(); ++i)
            {
                vec3 c = _pathList[_pathName]._pathColor;
                Gizmo::main->Line(
                    myPath[i], myPath[i + 1],
                    vec4(c.x, c.y, c.z, 1.0f)
                );
            }
            _pathList[_pathName].AreyouDraw = true;
        }
    }
}




void PathFinder::Update2()
{

}

void PathFinder::RenderBegin()
{
}

void PathFinder::Enable()
{
}

void PathFinder::Disable()
{
}

void PathFinder::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void PathFinder::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}




void PathFinder::ReadPathFile(const std::wstring& fileName)
{
	const wstring path = L"../Resources/Graph/";;

    std::ifstream file(path + fileName);
    if (!file.is_open())
    {
        std::wcout << L"Failed to open file: " << fileName << std::endl;
        exit(0);
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

	size_t h = std::hash<wstring>{}(fileName);
	float hue = float(h % 360) / 360.f;
	_pathList[fileName]._pathColor = vec3(hue, hue, hue);

     //cout << "라인 데이터: " << _pathList[fileName].path.size() << "개 읽음." << std::endl;
}

vec3 PathFinder::GenerateRandomPointInSphere(float radius)
{
	float u = Range(0.0f, 1.0f);
	float theta = Range(0.0f, 2.0f *3.14f);
	float phi = acos(2.0f * u - 1.0f);
	float r = radius * cbrt(Range(0.0f, 1.0f));

	float x = r * sin(phi) * cos(theta);
	float y = r * sin(phi) * sin(theta);
	float z = r * cos(phi);

	return vec3(x, y, z);
}

float PathFinder::Range(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}

void PathFinder::ClearDebugDraw()
{
	for (auto& path : _pathList)
	{
		path.second.AreyouDraw = false;
	}

    static bool coutPrintPathCount = false;

    if (coutPrintPathCount == false)
    {
        for (auto& ele : _pathList)
        {
            wcout << "Path Name:" << ele.first << " ," << "갯수:" << ele.second.count << endl;
        }

        coutPrintPathCount = true;
    }
}

void PathFinder::SetPass(const wstring& path)
{
    if (_pathList.find(path) == _pathList.end())
    {
        ReadPathFile(path);
    }

    _pathList[path].count++;
    _pathName = path;
}

//void PathFinder::SetData(Material* material)
//{
//	FishInfo info;
//	info.fishSpeed = material->GetPropertyFloat("_Speed");
//	info.fishWaveAmount = material->GetPropertyFloat("_Power");
//
//	BoundingBox& box = _renderBase.lock()->GetOriginBound();
//	info.boundsSizeZ = box.Extents.z;
//	info.boundsCenterZ = box.Center.z;
//
//	auto buffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SeaFIshParam)->Alloc(1);
//
//	memcpy(buffer->ptr, &info, sizeof(FishInfo));
//	int index = material->GetShader()->GetRegisterIndex("FishInfo");
//
//	if (index != -1)
//		Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, buffer->GPUAdress);
//}


void PathFinder::SetData(StructuredBuffer* buffer, Material* material)
{

	buffer->AddData(_info);
}
