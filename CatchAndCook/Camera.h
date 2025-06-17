#pragma once


class Collider;

enum class CameraType
{
    DebugCamera,
	SeaCamera,
    BoatCamera,
    UiCamera,
    ComponentCamera,

};


struct CameraParams
{

    Matrix ViewMatrix = Matrix::Identity;
    Matrix ProjectionMatrix = Matrix::Identity;
    Matrix VPMatrix = Matrix::Identity;
    Matrix InvertViewMatrix= Matrix::Identity;
    Matrix InvertProjectionMatrix= Matrix::Identity;
    Matrix InvertVPMatrix= Matrix::Identity;

    vec4 cameraPos{}; // worldPos
    vec4 cameraLook{}; // worldDirction
    vec4 cameraUp{}; // world Up Dirction
    vec4 cameraFrustumData{}; // x:fovy, y:aspect, z:near, w:far
    vec4 cameraScreenData{}; // x:screenx, y:screeny, z:offsetx, w:offsety
};

/*************************
*                        *
*       Camera           *
*                        *
**************************/

enum class CameraProjectionMode
{
    Perspective,
    Orthographic
};


class Camera
{

public:
    Camera() = default;
	Camera(CameraType type);

public:

    virtual void Update();
    
    void PushData();
    void SetData();

    void SetCameraPos(const vec3& pos) { _cameraPos = pos; }
    void SetCameraLook(const vec3& look);
    void SetCameraUp(const vec3& up);
    void SetCameraRight(const vec3& right);


    vec3& GetCameraPos() { return _cameraPos; }
    vec3& GetCameraLook() { return _cameraLook; }
    vec3& GetCameraUp() { return _cameraUp; }
    vec3& GetCameraRight() { return _cameraRight; }

    Vector3 GetScreenToWorldPosition(Vector2 mousePosition);
    Vector2 GetWorldToScreenPosition(Vector3 worldPosition);

    void SetCameraRotation(const Quaternion& quat);
    void SetCameraRotation(float yaw, float pitch, float roll);

    CameraParams& GetCameraParam() { return _params; }
    CameraType& GetCameraType() { return _type; }

    bool IsInFrustum(BoundingBox& box);
    void Calculate();
   

private:

public:
	const CameraParams& GetCameraParams() { return _params; }
	float GetNear() { return _near; }

    BoundingFrustum _boundingFrsutum;

    CameraParams _params;

protected:
    vec3 _cameraPos = vec3(0, -300.0f, 0);
    vec3 _cameraLook = vec3(0, 0, 1.0f);
    vec3 _cameraUp = vec3(0, 1.0f, 0);
    vec3 _cameraRight = vec3(1.0f, 0, 0);

    float _yaw = 0;
    float _pitch = 0;
    float _roll = 0;

    float _near = 0.3f;
    float _far = 8000.f;
    float _fov = XM_PI / 3.f;

    vec2 _orthoSize = { 100.0f,100.0f };

    CameraProjectionMode _projmode = CameraProjectionMode::Perspective;
    CameraType _type =CameraType::DebugCamera;
    CBufferContainer* _cbufferContainer;
};


/*************************
*                        *
*     ThirdPersonCamera  *
*                        *
**************************/

class DebugCamera : public Camera
{
public:
    DebugCamera();
    virtual ~DebugCamera();

    virtual void Update();

private:
};

class SeaCamera : public Camera
{
public:
    SeaCamera();
    virtual ~SeaCamera();

    virtual void Update();

private:
};

class UiCamera : public Camera
{

public:
    UiCamera();
    virtual ~UiCamera();
    virtual void Update();

};

class BoatCamera : public Camera
{

public:
    BoatCamera();
    virtual ~BoatCamera();
    virtual void Update();

};



