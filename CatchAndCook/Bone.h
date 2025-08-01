#pragma once


class Bone
{
	std::string _targetNodeName;
	std::string _boneName;
	Matrix _boneTransformMatrix = Matrix::Identity;
	int _index = -1;

public:
	void SetNodeName(const std::string& name) { _targetNodeName = name; };
	std::string& GetNodeName() { return  _targetNodeName; };

	void SetName(const std::string& name) { _boneName = name; };
	std::string& GetName() { return  _boneName; };

	void SetIndex(int index) { _index = index; };
	int GetIndex() { return _index; };

	void SetTransformMatrix(const Matrix& matrix) { _boneTransformMatrix = matrix; };
	Matrix& GetTransformMatrix() { return _boneTransformMatrix; };
};
