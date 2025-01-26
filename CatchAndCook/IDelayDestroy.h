#pragma once

// SCENE -> Ʈ������ / MATERIAL CAMERA MESHRENDER SCRIPTS LIGHT  / RENDERPASS / MODEL / RESOURCEMANAGER / INSTANCING
// 1.12 ������ ���������ϼ�.


class IDelayDestroy
{
private:
    bool _destroy = false;

public:
    bool IsFirst() const { return _first; };
    void FirstOff() { _first = false; };
    void SetFirst(bool first) { _first = first; };
    bool _first = true;
public:
    IDelayDestroy();
    virtual ~IDelayDestroy();
    bool IsDestroy() const;
    virtual void SetDestroy();
    virtual bool IsExecuteAble();

	friend class GameObject;
    friend class Component;
};