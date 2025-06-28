#pragma once


class AnimationKeyFrame;

class AnimationNode
{
public:
	AnimationNode();
	virtual ~AnimationNode();

	void Init(std::shared_ptr<Animation> animation);

	Matrix CalculateTransformMatrix(const double& time, bool xyLock = false) const;
	Matrix CalculateTransformMatrixMapping(const std::shared_ptr<ModelNode>& _originModelNode, const std::shared_ptr<ModelNode>& _animModelNode, const double& time, bool xyLock = false) const;
	Vector3 CalculatePosition(const double& time) const;
	Quaternion CalculateRotation(const double& time) const;
	Vector3 CalculateScale(const double& time) const;

	Vector3 CalculateDeltaPosition(const double& prevTime, const double& time) const;
	Quaternion CalculateDeltaRotation(const double& prevTime, const double& time) const;
	Vector3 CalculateDeltaScale(const double& prevTime, const double& time) const;

private:
	void SetKeyFrames(aiAnimation* anim, aiNodeAnim* animNode);
	void SetPosition(aiAnimation* anim, aiNodeAnim* animNode);
	void SetRotation(aiAnimation* anim, aiNodeAnim* animNode);
	void SetScale(aiAnimation* anim, aiNodeAnim* animNode);
	void SetPose(Matrix& matrix);
	void SetOffsetPosition(const Vector3& position);
	void SetOffsetScale(const Vector3& scale);
	void SetOffsetPreRotation(const Quaternion& scale);
	void SetOffsetPostRotation(const Quaternion& scale);

	void CheckMoveNode();

	static int FindKeyFrameIndex(const vector<AnimationKeyFrame>& vec, const double& time);


public:
	void SetNodeName(const string& name){
		_nodeName = name;
	}
	string& GetNodeName(){
		return _nodeName;
	}

	void SetRoot(bool tf) {
		_isRoot = tf;
	}
	bool IsRoot() const {
		return _isRoot;
	}
	string _nodeName;
	bool _isRoot = false;

	std::weak_ptr<Animation> _animation;

	Vector3 offsetPosition = Vector3::Zero;
	Quaternion offsetRotation = Quaternion::Identity;
	Quaternion offsetPreRotation = Quaternion::Identity;
	Quaternion offsetPostRotation = Quaternion::Identity;
	Vector3 offsetScale = Vector3::One;

	bool hasPosition = false;
	bool hasScale = false;
	bool hasPreRotation = false;
	bool hasPostRotation = false;

	vector<AnimationKeyFrame> _keyFrame_positions;
	vector<AnimationKeyFrame> _keyFrame_rotations;
	vector<AnimationKeyFrame> _keyFrame_scales;


	friend class Animation;
};
