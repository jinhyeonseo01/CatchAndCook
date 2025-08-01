#pragma once

// SCENE -> 트리구조 / MATERIAL CAMERA MESHRENDER SCRIPTS LIGHT  / RENDERPASS / MODEL / RESOURCEMANAGER / INSTANCING
// 1.12 전까지 엔진구조완성.


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