#pragma once

class ModelNode : public std::enable_shared_from_this<ModelNode>
{
private:
	std::vector<int> _meshIndexList;
	std::vector<int> _boneIndexList;

	std::weak_ptr<Model> _model;
	std::weak_ptr<ModelNode> _parent;
	std::weak_ptr<ModelNode> _originParent;
	std::vector<std::weak_ptr<ModelNode>> _childs;
	std::string _name;
	std::string _originalName;

	bool _isDynamic = true;

public:
	void SetName(const std::string& name)
	{
		_name = name;
		_originalName = _name;
		if (name.find("$AssimpFbx$") != std::string::npos)
		{
			vector<string> originalList = str::split(name, "_$AssimpFbx$");
			_originalName = originalList[0];
		}
	};
	std::string& GetName() { return  _name; };
	std::string& GetOriginalName() { return  _originalName; };

	void SetLocalSRT(const Matrix& matrix)
	{
		_localTransform = matrix;

		Vector3 pos, scale;
		Quaternion rot;
		_localTransform.Decompose(scale, rot, pos);
		SetLocalRotation(rot);
		SetLocalPosition(pos);
		SetLocalScale(scale);
	};
	Matrix& GetLocalSRT() { return _localTransform; };

	void SetLocalPreRotation(const Quaternion& quat) { _localPreRotation = quat; hasPreRotation = true; };
	Quaternion& GetLocalPreRotation() { return _localPreRotation; };
	void SetLocalPostRotation(const Quaternion& quat) { _localPostRotation = quat; hasPostRotation = true;};
	Quaternion& GetLocalPostRotation() { return _localPostRotation; };
	void SetLocalRotation(const Quaternion& quat) { _localRotation = quat; };
	Quaternion& GetLocalRotation() { return _localRotation; };
	void SetLocalPosition(const Vector3& position) { _localPosition = position; hasPosition = true; };
	Vector3& GetLocalPosition() { return _localPosition; };
	void SetLocalScale(const Vector3& scale) { _localScale = scale; hasScale = true; };
	Vector3& GetLocalScale() { return _localScale; };

	bool hasPosition = false;
	bool hasScale = false;
	bool hasPreRotation = false;
	bool hasPostRotation = false;


	void AddBoneIndex(int index) { _boneIndexList.push_back(index); };
	bool IsBone() const { return _boneIndexList.size() != 0; };

	bool IsMesh() const { return _meshIndexList.size() != 0; };

	void SetDynamic(bool isDynamic) { _isDynamic = isDynamic; };
	bool IsDynamic() const { return _isDynamic; };

	void Init(shared_ptr<Model> model, aiNode* node);

	void SetParent(const std::shared_ptr<ModelNode>& object);
	std::shared_ptr<ModelNode> GetParent() const;
	void SetOriginalParent(const std::shared_ptr<ModelNode>& object);
	std::shared_ptr<ModelNode> GetOriginalParent() const;

	void AddChild(const std::shared_ptr<ModelNode>& object);
	std::vector<std::weak_ptr<ModelNode>>& GetChilds();

	void CalculateTPoseNode(const std::shared_ptr<ModelNode>& parent);

	std::shared_ptr<GameObject> CreateGameObject(const std::shared_ptr<Scene>& scene,
		const std::shared_ptr<GameObject>& parent);



public:
	Matrix _localTransform = Matrix::Identity;

	Matrix _localTPose = Matrix::Identity;
	Matrix _localInvertTPose = Matrix::Identity;
	Matrix _globalTPose = Matrix::Identity;
	Matrix _globalInvertTPose = Matrix::Identity;
	Matrix _parent_globalTPose = Matrix::Identity;
	Matrix _parent_globalInvertTPose = Matrix::Identity;

	Quaternion _globalTPoseQuat = Quaternion::Identity;
	Quaternion _globalInvertTPoseQuat = Quaternion::Identity;
	Quaternion _parent_globalTPoseQuat = Quaternion::Identity;
	Quaternion _parent_globalInvertTPoseQuat = Quaternion::Identity;




	Vector3 _localPosition = Vector3::Zero;
	Quaternion _localPreRotation = Quaternion::Identity;
	Quaternion _localRotation = Quaternion::Identity;
	Quaternion _localPostRotation = Quaternion::Identity;
	Vector3 _localScale = Vector3::One;
};
