#pragma once
#include "Component.h"

#define PI 3.14159f




class WaterHeight : public Component
{
public:
    ~WaterHeight() override;
    bool IsExecuteAble() override;
    virtual void Init();
    virtual void Start();
    virtual void Update();
    virtual void Update2();
    virtual void Enable();
    virtual void Disable();
    virtual void RenderBegin();
    virtual void CollisionBegin(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other);
    void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
    virtual void SetDestroy() override;
    virtual void Destroy();

    float GetWaveHeight(const vec3& worldPos);


private:
    SeaParam* _seaParam;





};

