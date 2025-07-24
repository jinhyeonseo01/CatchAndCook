#include "pch.h"
#include "Gizmo.h"

#include "Mesh.h"
#include "Texture.h"
#include "TextManager.h"


std::unique_ptr<Gizmo> Gizmo::main = nullptr;

void Gizmo::Init()
{
	material = std::make_shared<Material>();
	material->SetShader(ResourceManager::main->Get<Shader>(L"Gizmo"));
	material->SetPass(RENDER_PASS::Debug);
	container = Core::main->GetBufferManager()->GetInstanceBufferPool(BufferType::GizmoInstanceParam)->Alloc();

	textureGizmo.material = std::make_shared<Material>();
    textureGizmo.material->SetShader(ResourceManager::main->Get<Shader>(L"GizmoTexture"));
    textureGizmo.material->SetPass(RENDER_PASS::Debug);
    textureGizmo._mesh = GeoMetryHelper::LoadRectCenterMesh();

    SetCulling(false);
    SetInstancing(false);

  /*  for(int i=0;i<256;i++)
    {
        auto _textHandle =  TextManager::main->AllocTextStrcture(256,256,L"Arial", FontColor::CUSTOM, 30, vec4(1,1,1,1));
        auto _sysMemory = new BYTE[(256 * 256 * 4)]();
        auto _texture = std::make_shared<Texture>();
        _texture->CreateDynamicTexture(DXGI_FORMAT_R8G8B8A8_UNORM,256,256);
        textureGizmo.textTextures.push_back(_texture);
        textureGizmo.sysMemorys.push_back(_sysMemory);
        textureGizmo.handles.push_back(_textHandle);
    }*/
}

void Gizmo::Clear()
{
	container->Clear();
    textureGizmo.Clear();
	lineDatas.clear();
}

void Gizmo::Line(const Vector3& worldStart, const Vector3& worldEnd, const Vector4& Color)
{
    //if (main->_flags == GizmoFlags::None) return;
	Instance_Gizmo giz;
	giz.color = Color;
	giz.position = worldStart;
	giz.position2 = worldEnd;
	giz.strock = main->_width;
	main->container->AddData(giz);
	main->lineDatas.push_back(giz);

}

void Gizmo::Ray(const Vector3& worldStart, const Vector3& dir, float dis, const Vector4& Color)
{
    //if (main->_flags == GizmoFlags::None) return;
	Vector3 normalDir;
	dir.Normalize(normalDir);
	Instance_Gizmo giz;
	giz.color = Color;
	giz.position = worldStart;
	giz.position2 = worldStart + normalDir * dis;
	giz.strock = main->_width;
	main->container->AddData(giz);
	main->lineDatas.push_back(giz);
}

void Gizmo::Box(const BoundingOrientedBox& box, const Vector4& Color)
{
    //if (main->_flags == GizmoFlags::None) return;
    vec3 corners[8];
    box.GetCorners(corners);

    // 앞면 4개 엣지
    Line(Vector3(corners[0].x,corners[0].y,corners[0].z),
         Vector3(corners[1].x,corners[1].y,corners[1].z),Color);
    Line(Vector3(corners[1].x,corners[1].y,corners[1].z),
         Vector3(corners[2].x,corners[2].y,corners[2].z),Color);
    Line(Vector3(corners[2].x,corners[2].y,corners[2].z),
         Vector3(corners[3].x,corners[3].y,corners[3].z),Color);
    Line(Vector3(corners[3].x,corners[3].y,corners[3].z),
         Vector3(corners[0].x,corners[0].y,corners[0].z),Color);

    // 뒷면 4개 엣지
    Line(Vector3(corners[4].x,corners[4].y,corners[4].z),
         Vector3(corners[5].x,corners[5].y,corners[5].z),Color);
    Line(Vector3(corners[5].x,corners[5].y,corners[5].z),
         Vector3(corners[6].x,corners[6].y,corners[6].z),Color);
    Line(Vector3(corners[6].x,corners[6].y,corners[6].z),
         Vector3(corners[7].x,corners[7].y,corners[7].z),Color);
    Line(Vector3(corners[7].x,corners[7].y,corners[7].z),
         Vector3(corners[4].x,corners[4].y,corners[4].z),Color);

    // 앞면-뒷면 연결 4개 엣지
    Line(Vector3(corners[0].x,corners[0].y,corners[0].z),
         Vector3(corners[4].x,corners[4].y,corners[4].z),Color);
    Line(Vector3(corners[1].x,corners[1].y,corners[1].z),
         Vector3(corners[5].x,corners[5].y,corners[5].z),Color);
    Line(Vector3(corners[2].x,corners[2].y,corners[2].z),
         Vector3(corners[6].x,corners[6].y,corners[6].z),Color);
    Line(Vector3(corners[3].x,corners[3].y,corners[3].z),
         Vector3(corners[7].x,corners[7].y,corners[7].z),Color);
}

void Gizmo::Box(const BoundingBox& box, const Vector4& Color)
{
    //if (main->_flags == GizmoFlags::None) return;

    XMFLOAT3 corners[8];
    box.GetCorners(corners);

    // 앞면 4개 엣지
    Line(Vector3(corners[0].x,corners[0].y,corners[0].z),
         Vector3(corners[1].x,corners[1].y,corners[1].z),Color);
    Line(Vector3(corners[1].x,corners[1].y,corners[1].z),
         Vector3(corners[2].x,corners[2].y,corners[2].z),Color);
    Line(Vector3(corners[2].x,corners[2].y,corners[2].z),
         Vector3(corners[3].x,corners[3].y,corners[3].z),Color);
    Line(Vector3(corners[3].x,corners[3].y,corners[3].z),
         Vector3(corners[0].x,corners[0].y,corners[0].z),Color);

    // 뒷면 4개 엣지
    Line(Vector3(corners[4].x,corners[4].y,corners[4].z),
         Vector3(corners[5].x,corners[5].y,corners[5].z),Color);
    Line(Vector3(corners[5].x,corners[5].y,corners[5].z),
         Vector3(corners[6].x,corners[6].y,corners[6].z),Color);
    Line(Vector3(corners[6].x,corners[6].y,corners[6].z),
         Vector3(corners[7].x,corners[7].y,corners[7].z),Color);
    Line(Vector3(corners[7].x,corners[7].y,corners[7].z),
         Vector3(corners[4].x,corners[4].y,corners[4].z),Color);

    // 앞면-뒷면 연결 4개 엣지
    Line(Vector3(corners[0].x,corners[0].y,corners[0].z),
         Vector3(corners[4].x,corners[4].y,corners[4].z),Color);
    Line(Vector3(corners[1].x,corners[1].y,corners[1].z),
         Vector3(corners[5].x,corners[5].y,corners[5].z),Color);
    Line(Vector3(corners[2].x,corners[2].y,corners[2].z),
         Vector3(corners[6].x,corners[6].y,corners[6].z),Color);
    Line(Vector3(corners[3].x,corners[3].y,corners[3].z),
         Vector3(corners[7].x,corners[7].y,corners[7].z),Color);
}

void Gizmo::Box(const vec3& min, const vec3& max, const Vector4& Color)
{
    //if (main->_flags == GizmoFlags::None) return;

	BoundingBox box;
	box.CreateFromPoints(box, min, max);
	Box(box, Color);
}

void Gizmo::Frustum(const BoundingFrustum& frustum, const Vector4& Color)
{
    //if (main->_flags == GizmoFlags::None) return;

    XMFLOAT3 corners[8];
    frustum.GetCorners(corners);

    // 앞면 4개 엣지
    Line(Vector3(corners[0].x,corners[0].y,corners[0].z),
         Vector3(corners[1].x,corners[1].y,corners[1].z),Color);
    Line(Vector3(corners[1].x,corners[1].y,corners[1].z),
         Vector3(corners[2].x,corners[2].y,corners[2].z),Color);
    Line(Vector3(corners[2].x,corners[2].y,corners[2].z),
         Vector3(corners[3].x,corners[3].y,corners[3].z),Color);
    Line(Vector3(corners[3].x,corners[3].y,corners[3].z),
         Vector3(corners[0].x,corners[0].y,corners[0].z),Color);

    // 뒷면 4개 엣지
    Line(Vector3(corners[4].x,corners[4].y,corners[4].z),
         Vector3(corners[5].x,corners[5].y,corners[5].z),Color);
    Line(Vector3(corners[5].x,corners[5].y,corners[5].z),
         Vector3(corners[6].x,corners[6].y,corners[6].z),Color);
    Line(Vector3(corners[6].x,corners[6].y,corners[6].z),
         Vector3(corners[7].x,corners[7].y,corners[7].z),Color);
    Line(Vector3(corners[7].x,corners[7].y,corners[7].z),
         Vector3(corners[4].x,corners[4].y,corners[4].z),Color);

    // 앞면-뒷면 연결 4개 엣지
    Line(Vector3(corners[0].x,corners[0].y,corners[0].z),
         Vector3(corners[4].x,corners[4].y,corners[4].z),Color);
    Line(Vector3(corners[1].x,corners[1].y,corners[1].z),
         Vector3(corners[5].x,corners[5].y,corners[5].z),Color);
    Line(Vector3(corners[2].x,corners[2].y,corners[2].z),
         Vector3(corners[6].x,corners[6].y,corners[6].z),Color);
    Line(Vector3(corners[3].x,corners[3].y,corners[3].z),
         Vector3(corners[7].x,corners[7].y,corners[7].z),Color);
}

void Gizmo::Sphere(const BoundingSphere& sphere, const Vector4& Color)
{
    //if (main->_flags == GizmoFlags::None) return;

    Vector3 center(sphere.Center.x,sphere.Center.y,sphere.Center.z);
    float radius = sphere.Radius;

    // 원(써클)을 얼마나 부드럽게 그릴지 결정 (세그먼트 수)
    constexpr int segments = 24;
    float step = XM_2PI / static_cast<float>(segments);

    // 1) XY 평면 원 (z=0 축)
    for(int i = 0; i < segments; i++)
    {
        float angle1 = i * step;
        float angle2 = (i + 1) * step;

        Vector3 p1 = center + Vector3(radius * cosf(angle1),radius * sinf(angle1),0.0f);
        Vector3 p2 = center + Vector3(radius * cosf(angle2),radius * sinf(angle2),0.0f);

        Gizmo::Line(p1,p2, Color);
    }

    // 2) YZ 평면 원 (x=0 축)
    for(int i = 0; i < segments; i++)
    {
        float angle1 = i * step;
        float angle2 = (i + 1) * step;

        Vector3 p1 = center + Vector3(0.0f,radius * cosf(angle1),radius * sinf(angle1));
        Vector3 p2 = center + Vector3(0.0f,radius * cosf(angle2),radius * sinf(angle2));

        Gizmo::Line(p1,p2, Color);
    }

    // 3) XZ 평면 원 (y=0 축)
    for(int i = 0; i < segments; i++)
    {
        float angle1 = i * step;
        float angle2 = (i + 1) * step;

        Vector3 p1 = center + Vector3(radius * cosf(angle1),0.0f,radius * sinf(angle1));
        Vector3 p2 = center + Vector3(radius * cosf(angle2),0.0f,radius * sinf(angle2));

        Gizmo::Line(p1,p2, Color);
    }
}

void Gizmo::Text(const wstring& text, int fontSize, const Vector3& worldPos, const Vector3& worldDir, const Vector3& Up, const Vector4& Color)
{
    //if (main->_flags == GizmoFlags::None) return;

    Vector3 right = Up.Cross(worldDir);
    Vector3 up = worldDir.Cross(right);
    Matrix trs = Matrix::CreateWorld(worldPos,worldDir, up);
    
    Instance_GizmoText data;
    TransformParam param;
    data.color = Color;
    param.localToWorld = trs;
    param.localToWorld.Invert(param.worldToLocal);
    param.worldPos = worldPos;

    auto texture = main->textureGizmo.textTextures[main->textureGizmo.textAllocator];
    auto handle = main->textureGizmo.handles[main->textureGizmo.textAllocator];
    auto memory = main->textureGizmo.sysMemorys[main->textureGizmo.textAllocator];
    TextManager::main->UpdateToSysMemory(text,handle, memory,4);
    main->textureGizmo.textAllocator++;

    main->textureGizmo.transforms.push_back(param);
    main->textureGizmo.textures.push_back(texture);
}

void Gizmo::Image(const std::shared_ptr<::Texture>& texture, const Vector3& worldPos,
	const Vector3& worldDir, const Vector3& Up, const Vector3& size, const Vector4& Color)
{
	/*if (main->_flags == GizmoFlags::None) return;*/

    Vector3 right = Up.Cross(worldDir);
    Vector3 up = worldDir.Cross(right);
    Matrix trs = Matrix::CreateScale(size) * Matrix::CreateWorld(worldPos,worldDir, up);

    TransformParam param;
    
    param.localToWorld = trs;
    param.localToWorld.Invert(param.worldToLocal);
    param.worldPos = worldPos;

	main->textureGizmo.textures.push_back(texture);
    main->textureGizmo.transforms.push_back(param);
}

void Gizmo::RenderBegin()
{
    //if (main->_flags == GizmoFlags::None) return;

	SceneManager::main->GetCurrentScene()->AddRenderer(material.get(),nullptr,this);
    SceneManager::main->GetCurrentScene()->AddRenderer(textureGizmo.material.get(), textureGizmo._mesh.get(), &textureGizmo);
}

void Gizmo::Rendering(Material* material, Mesh* mesh, int instanceCount)
{
    //if (main->_flags == GizmoFlags::None) return;


	auto& cmdList = Core::main->GetCmdList();

	cmdList->SetPipelineState(material->GetShader()->_pipelineState.Get());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	cmdList->IASetVertexBuffers(1,1,&container->_bufferView);
	cmdList->DrawInstanced(2, lineDatas.size(),0,0);


    
}



void Gizmo::DebugRendering()
{

}

void GizmoTexture::Clear()
{
    textures.clear();
    transforms.clear();
    textAllocator = 0;
}

GizmoTexture::~GizmoTexture()
{
}

void GizmoTexture::Rendering(Material* material, Mesh* mesh,int instanceCount)
{
    if (Gizmo::main->_flags == GizmoFlags::None) return;

    auto& cmdList = Core::main->GetCmdList();

    for(int i=0;i<textAllocator;i++)
    {
        textTextures[i]->UpdateDynamicTexture(sysMemorys[i], 4);
        textTextures[i]->CopyCpuToGpu();
    }
    
    for(int i=0;i<textures.size();i++)
    {
        cmdList->SetPipelineState(material->GetShader()->_pipelineState.Get());

        auto& texture = textures[i];
        auto& transform = transforms[i];
        auto state = texture->_state;
        texture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

        auto _cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::TransformParam)->Alloc(1);
        memcpy(_cbufferContainer->ptr,(void*)&transform,sizeof(TransformParam));
        Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(1,_cbufferContainer->GPUAdress);


		material->SetTexture("_BaseMap", texture);
        material->AllocTextureTable();
        material->PushData();
        material->SetData();

        mesh->Redner();

        texture->ResourceBarrier(state);
    }
}

void GizmoTexture::DebugRendering()
{
}
