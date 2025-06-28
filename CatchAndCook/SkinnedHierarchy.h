#pragma once
#include "Component.h"


class SkinnedHierarchy : public Component, public RenderCBufferSetter, public RenderStructuredSetter
{
public:
	static std::unordered_map<std::string, std::string> _boneNameToHumanMappingTable;

	~SkinnedHierarchy() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;


	Vector3 GetDeltaPosition();

	void SetBoneList(const std::vector<std::shared_ptr<Bone>>& bones);
	void SetNodeList(const std::vector<std::shared_ptr<ModelNode>>& nodes);
	void SetModel(const std::shared_ptr<Model>& model);
	void SetMappingTable(const std::unordered_map<std::string, std::string>& table);

	template <class T>
	typename std::unordered_map<std::string, T>::iterator
		FindByName(const std::string& name, std::unordered_map<std::string, T>& data)
	{
		auto it = data.find(name);
		if (it != data.end())
			return it;

		auto it2 = _boneNameToHumanMappingTable.find(name);
		if (it2 != _boneNameToHumanMappingTable.end())
		{
			auto it3 = _boneHumanNameTable.find(it2->second);
			if (it3 != _boneHumanNameTable.end())
				return data.find(it3->second);
		}
		return data.end();
	}

	template <class T>
	std::string FindNameByName(const std::string& name, std::unordered_map<std::string, T>& data)
	{
		auto it = data.find(name);
		if (it != data.end())
			return name;

		auto it2 = _boneNameToHumanMappingTable.find(name);
		if (it2 != _boneNameToHumanMappingTable.end())
		{
			auto it3 = _boneHumanNameTable.find(it2->second);
			if (it3 != _boneHumanNameTable.end())
				return it3->second;
		}
		return"";
	}
	

	void SetAnimation(const std::shared_ptr<Animation>& animation);
	std::shared_ptr<Animation> GetAnimation();
	void FindNodeObjects();
	void SetData(Material* material) override;

	void Animate(const std::shared_ptr<Animation>& animation, double time);
	double AnimateBlend(const std::shared_ptr<Animation>& currentAnim,
	                    const std::shared_ptr<Animation>& nextAnim,
	                    const double& duration = 0.25);

	Vector3 BlendDeltaPosition(const std::string& name,
	                           const std::shared_ptr<Animation>& currentAnim,
	                           const std::shared_ptr<Animation>& nextAnim, const double& duration = 0.25);

	void Play(const std::shared_ptr<Animation>& animation, const double& duration = 0.25);
	void Play();
	bool IsPlay() { return _isPlaying; }
	void Stop();
	void Pause();
	void SetData(StructuredBuffer* buffer, Material* material) override;

	// Animation Control
	std::shared_ptr<Model> _model;
	std::shared_ptr<Animation> _animation;
	std::shared_ptr<Animation> _nextAnimation;


	double _animationTime = 0;
	double _prevAnimationTime = 0;

	double _animationBlendTime = 0;
	double _prevAnimationBlendTime = 0;

	bool _blendEnd = false;

	double _duration = 0.25;
	double _speedMultiple = 1.2;
	Vector3 rootMoveLock = Vector3(0, 1, 0);
	bool _isPlaying = true;

public:// Animation Data

	// Node Name to HumanName
	std::unordered_map<std::string, std::string> _boneHumanNameTable;

	// Node Name To Object
	std::unordered_map<std::string, std::weak_ptr<GameObject>> nodeObjectTable;
	// Node Object List
	vector<std::weak_ptr<GameObject>> nodeObjectList;

	
	std::weak_ptr<GameObject> _rootBone;

	std::vector<wstring> _boneNameList;
	std::vector<wstring> _nodeNameList;


	//Bone Array Object
	std::array<std::weak_ptr<GameObject>, 256> _boneNodeList;

	std::array<Matrix, 256> _boneOffsetMatrixList;
	std::array<Matrix, 256> _finalMatrixList;
	std::array<Matrix, 256> _finalInvertMatrixList;

	CBufferContainer* _boneCBuffer;
};
