#pragma once
#include "Component.h"


struct FishPath
{
	vector<vec3> path;
	bool AreyouDraw = false;
	vec3 _pathColor;
	int count = 0; //추적용.
};

struct FishInfo
{
	float fishWaveAmount;
	float fishSpeed;
	float boundsCenterZ;
	float boundsSizeZ;
};

class RendererBase;

class PathFinder :public Component, public RenderStructuredSetter
{

public:
	PathFinder();
	~PathFinder();
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void RenderBegin() override;
	void Enable() override;
	void Disable() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	bool IsExecuteAble() { return true; };

	void SetData(StructuredBuffer* buffer = nullptr, Material* material = nullptr) override;
	//virtual void SetData(Material* material = nullptr) override;
public:
	void SetMoveSpeed(float speed) { _moveSpeed = speed; }
	void SetStartIndex(int index) { _currentIndex = index; }
	void SetPass(const wstring& path);
	static void ClearDebugDraw();
private:
	void ReadPathFile(const std::wstring& fileName);
	vec3 GenerateRandomPointInSphere(float radius);
	float Range(float min, float max);

private:
	static unordered_map<wstring, FishPath> _pathList;
	weak_ptr<GameObject> _player;

	vec3 _pathOffset = vec3(0.0f);
	bool _forward = true;
	int _currentIndex = 0;
	float _moveSpeed = 50.0f; 
	float _distanceMoved = 0.0f;
	float _segmentLength = 0.0f;
	wstring _pathName = L"Null";
	Quaternion _firstQuat{};
	FishInfo _info{};

public:

};

