#pragma once

class AnimationNode;

class Animation : public enable_shared_from_this<Animation>
{
public:
	Animation();
	virtual ~Animation();

	void Init(std::shared_ptr<Model> model, aiAnimation* anim, aiNode* root);

	double CalculateTime(double time) const;
	bool IsEndTime(double time);

	void SetName(const wstring& name){
		_name = name;
		_nameStr = to_string(name);
	}
	wstring& GetName(){
		return _name;
	}
	string& GetNameStr() {
		return _nameStr;
	}
	wstring _name;
	string _nameStr;

	void SetModelName(const string& name) {
		_modelName = name;
	}
	string& GetModelName() {
		return _modelName;
	}
	string _modelName;
	std::weak_ptr<Model> _model;

	double _ticksPerSecond;
	double _duration;
	double _totalTime;

	bool _firstLoopEnd=false;

	double _speedMultiplier = 1;

	bool _isLoop = true;
	bool _isApplyTransform = true;

	std::unordered_map<string, std::shared_ptr<AnimationNode>> _nodeTables;
	std::vector<std::shared_ptr<AnimationNode>> _nodeLists;
	std::shared_ptr<AnimationNode> _rootBoneNode;
};

