#include "pch.h"
#include "GeoMetryHelper.h"
#include "Mesh.h"

shared_ptr<Mesh> GeoMetryHelper::LoadPyramidMesh()
{
    // 리소스 키 생성
    std::wstring key = L"pyramid";

    // 이미 로드된 Mesh가 있다면 반환
    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(key);
    if (mesh)
    {
        return mesh;
    }

    // 피라미드의 크기 설정 (scale은 정점 크기의 절반)
    float s = 1.0f;

    // 정점 데이터: 베이스 4개 + 측면 4면 x 3개 = 총 16개
    vector<Vertex_Static> vertices;
    vertices.reserve(16);

    // -------------------------------
    // 베이스 (밑면) 정점 (y = -s)
    // -------------------------------
    vec3 baseNormal = vec3(0, -1, 0);
    vec3 baseTangent = vec3(1, 0, 0);
    // UV 좌표: 왼쪽 아래 (0,1), 오른쪽 아래 (1,1), 오른쪽 위 (1,0), 왼쪽 위 (0,0)
    Vertex_Static base0(vec3(-s, -s, -s), baseNormal, baseTangent, vec2(0, 1));
    Vertex_Static base1(vec3(s, -s, -s), baseNormal, baseTangent, vec2(1, 1));
    Vertex_Static base2(vec3(s, -s, s), baseNormal, baseTangent, vec2(1, 0));
    Vertex_Static base3(vec3(-s, -s, s), baseNormal, baseTangent, vec2(0, 0));

    vertices.push_back(base0); // 인덱스 0
    vertices.push_back(base1); // 1
    vertices.push_back(base2); // 2
    vertices.push_back(base3); // 3

    // 피라미드의 꼭짓점 (엡섹스)
    vec3 apex = vec3(0, s, 0);

    // -------------------------------
    // 측면: 4개의 삼각형 면
    // 각 면은 별도의 정점 세트를 사용하여 올바른 법선과 UV를 지정
    // (아래 법선 값은 미리 계산된 대략적인 값입니다.)
    // -------------------------------

    // Front Face (앞면): 베이스 에지: base0 -> base1, 그리고 apex
    vec3 frontNormal = vec3(0, -0.4472136f, 0.894427f); // (0, -1/√5, 2/√5)
    Vertex_Static front_v0(vec3(-s, -s, -s), frontNormal, baseTangent, vec2(0, 1));
    Vertex_Static front_v1(vec3(s, -s, -s), frontNormal, baseTangent, vec2(1, 1));
    Vertex_Static front_v2(apex, frontNormal, baseTangent, vec2(0.5f, 0));
    vertices.push_back(front_v0); // 인덱스 4
    vertices.push_back(front_v1); // 5
    vertices.push_back(front_v2); // 6

    // Right Face (오른쪽 면): 베이스 에지: base1 -> base2, 그리고 apex  
    vec3 rightNormal = vec3(0.894427f, -0.4472136f, 0); // (2/√5, -1/√5, 0)
    Vertex_Static right_v0(vec3(s, -s, -s), rightNormal, baseTangent, vec2(0, 1));
    Vertex_Static right_v1(vec3(s, -s, s), rightNormal, baseTangent, vec2(1, 1));
    Vertex_Static right_v2(apex, rightNormal, baseTangent, vec2(0.5f, 0));
    vertices.push_back(right_v0); // 인덱스 7
    vertices.push_back(right_v1); // 8
    vertices.push_back(right_v2); // 9

    // Back Face (뒷면): 베이스 에지: base2 -> base3, 그리고 apex  
    vec3 backNormal = vec3(0, -0.4472136f, -0.894427f); // (0, -1/√5, -2/√5)
    Vertex_Static back_v0(vec3(s, -s, s), backNormal, baseTangent, vec2(0, 1));
    Vertex_Static back_v1(vec3(-s, -s, s), backNormal, baseTangent, vec2(1, 1));
    Vertex_Static back_v2(apex, backNormal, baseTangent, vec2(0.5f, 0));
    vertices.push_back(back_v0); // 인덱스 10
    vertices.push_back(back_v1); // 11
    vertices.push_back(back_v2); // 12

    // Left Face (왼쪽 면): 베이스 에지: base3 -> base0, 그리고 apex  
    vec3 leftNormal = vec3(-0.894427f, -0.4472136f, 0); // (-2/√5, -1/√5, 0)
    Vertex_Static left_v0(vec3(-s, -s, s), leftNormal, baseTangent, vec2(0, 1));
    Vertex_Static left_v1(vec3(-s, -s, -s), leftNormal, baseTangent, vec2(1, 1));
    Vertex_Static left_v2(apex, leftNormal, baseTangent, vec2(0.5f, 0));
    vertices.push_back(left_v0); // 인덱스 13
    vertices.push_back(left_v1); // 14
    vertices.push_back(left_v2); // 15

    // -------------------------------
    // 인덱스 데이터 생성
    // 베이스: 2개의 삼각형, 측면: 각 1개의 삼각형 (총 4면)
    // -------------------------------
    vector<uint32> indices;
    indices.reserve(18);

    // 베이스 (아래 면): 정점 인덱스 0,1,2,3
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    // 측면 삼각형들:
    // Front Face: 정점 4, 5, 6
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);

    // Right Face: 정점 7, 8, 9
    indices.push_back(7);
    indices.push_back(8);
    indices.push_back(9);

    // Back Face: 정점 10, 11, 12
    indices.push_back(10);
    indices.push_back(11);
    indices.push_back(12);

    // Left Face: 정점 13, 14, 15
    indices.push_back(13);
    indices.push_back(14);
    indices.push_back(15);

    // Mesh 초기화 및 리소스 등록
    mesh = make_shared<Mesh>();
    mesh->Init(vertices, indices);
    ResourceManager::main->Add(key, mesh);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleBox(const float scale)
{

    // 리소스 키 생성
    std::wstring key = L"box" + std::to_wstring(scale);

    // 이미 로드된 Mesh가 있다면 반환
    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(key);
    if (mesh)
    {
        return mesh;
    }

    // 
    // 및 Index 데이터 생성

    vector<uint32> indices;
    indices.reserve(36); // 6면 x 2개의 삼각형 x 3개의 인덱스


    float w2 = scale;
    float h2 = scale;
    float d2 = scale;

    vector<Vertex_Static> vec(24);

    // 앞면 (법선: +Z 방향)
    vec[0] = Vertex_Static(vec3(-w2,-h2,-d2),vec3(0,0,-1),vec3(1,0,0),vec2(0,1));
    vec[1] = Vertex_Static(vec3(-w2,+h2,-d2),vec3(0,0,-1),vec3(1,0,0),vec2(0,0));
    vec[2] = Vertex_Static(vec3(+w2,+h2,-d2),vec3(0,0,-1),vec3(1,0,0),vec2(1,0));
    vec[3] = Vertex_Static(vec3(+w2,-h2,-d2),vec3(0,0,-1),vec3(1,0,0),vec2(1,1));

    // 뒷면 (법선: +Z 방향)
    vec[4] = Vertex_Static(vec3(-w2,-h2,+d2),vec3(0,0,1),vec3(-1,0,0),vec2(1,1));
    vec[5] = Vertex_Static(vec3(+w2,-h2,+d2),vec3(0,0,1),vec3(-1,0,0),vec2(0,1));
    vec[6] = Vertex_Static(vec3(+w2,+h2,+d2),vec3(0,0,1),vec3(-1,0,0),vec2(0,0));
    vec[7] = Vertex_Static(vec3(-w2,+h2,+d2),vec3(0,0,1),vec3(-1,0,0),vec2(1,0));

    // 윗면 (법선: +Y 방향)
    vec[8] = Vertex_Static(vec3(-w2,+h2,-d2),vec3(0,1,0),vec3(1,0,0),vec2(0,1));
    vec[9] = Vertex_Static(vec3(-w2,+h2,+d2),vec3(0,1,0),vec3(1,0,0),vec2(0,0));
    vec[10] = Vertex_Static(vec3(+w2,+h2,+d2),vec3(0,1,0),vec3(1,0,0),vec2(1,0));
    vec[11] = Vertex_Static(vec3(+w2,+h2,-d2),vec3(0,1,0),vec3(1,0,0),vec2(1,1));

    // 아랫면 (법선: -Y 방향)
    vec[12] = Vertex_Static(vec3(-w2,-h2,-d2),vec3(0,-1,0),vec3(1,0,0),vec2(1,1));
    vec[13] = Vertex_Static(vec3(+w2,-h2,-d2),vec3(0,-1,0),vec3(1,0,0),vec2(0,1));
    vec[14] = Vertex_Static(vec3(+w2,-h2,+d2),vec3(0,-1,0),vec3(1,0,0),vec2(0,0));
    vec[15] = Vertex_Static(vec3(-w2,-h2,+d2),vec3(0,-1,0),vec3(1,0,0),vec2(1,0));

    // 왼쪽면 (법선: -X 방향)
    vec[16] = Vertex_Static(vec3(-w2,-h2,+d2),vec3(-1,0,0),vec3(0,0,-1),vec2(0,1));
    vec[17] = Vertex_Static(vec3(-w2,+h2,+d2),vec3(-1,0,0),vec3(0,0,-1),vec2(0,0));
    vec[18] = Vertex_Static(vec3(-w2,+h2,-d2),vec3(-1,0,0),vec3(0,0,-1),vec2(1,0));
    vec[19] = Vertex_Static(vec3(-w2,-h2,-d2),vec3(-1,0,0),vec3(0,0,-1),vec2(1,1));

    // 오른쪽면 (법선: +X 방향)
    vec[20] = Vertex_Static(vec3(+w2,-h2,-d2),vec3(1,0,0),vec3(0,0,1),vec2(0,1));
    vec[21] = Vertex_Static(vec3(+w2,+h2,-d2),vec3(1,0,0),vec3(0,0,1),vec2(0,0));
    vec[22] = Vertex_Static(vec3(+w2,+h2,+d2),vec3(1,0,0),vec3(0,0,1),vec2(1,0));
    vec[23] = Vertex_Static(vec3(+w2,-h2,+d2),vec3(1,0,0),vec3(0,0,1),vec2(1,1));


    vector<uint32> idx(36);

    // 앞면
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;
    // 뒷면
    idx[6] = 4; idx[7] = 5; idx[8] = 6;
    idx[9] = 4; idx[10] = 6; idx[11] = 7;
    // 윗면
    idx[12] = 8; idx[13] = 9; idx[14] = 10;
    idx[15] = 8; idx[16] = 10; idx[17] = 11;
    // 아랫면
    idx[18] = 12; idx[19] = 13; idx[20] = 14;
    idx[21] = 12; idx[22] = 14; idx[23] = 15;
    // 왼쪽면
    idx[24] = 16; idx[25] = 17; idx[26] = 18;
    idx[27] = 16; idx[28] = 18; idx[29] = 19;
    // 오른쪽면
    idx[30] = 20; idx[31] = 21; idx[32] = 22;
    idx[33] = 20; idx[34] = 22; idx[35] = 23;


    // Mesh 초기화 및 리소스 등록
    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);
    ResourceManager::main->Add(key, mesh);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleBoxWithColor(const float scale, vec4 color)
{
    // 리소스 키 생성
    //std::wstring key = L"boxColor" + std::to_wstring(scale);

    //// 이미 로드된 Mesh가 있다면 반환
    shared_ptr<Mesh> mesh;
    //if (mesh)
    //{
    //    return mesh;
    //}

    // Vertex 및 Index 데이터 생성
    vector<Vertex_Color> vertices;
    vertices.reserve(24); // 6면 x 4개 정점

    vector<uint32> indices;
    indices.reserve(36); // 6면 x 2개의 삼각형 x 3개의 인덱스


    float w2 = scale;
    float h2 = scale;
    float d2 = scale;

    vector<Vertex_Color> vec(24);

    // 앞면 (법선: (0, 0, -1))
    vec[0] = Vertex_Color(vec3(-w2, -h2, -d2), vec3(0, 0, -1), vec2(0.0f, 1.0f), color);
    vec[1] = Vertex_Color(vec3(-w2, +h2, -d2), vec3(0, 0, -1), vec2(0.0f, 0.0f), color);
    vec[2] = Vertex_Color(vec3(+w2, +h2, -d2), vec3(0, 0, -1), vec2(1.0f, 0.0f), color);
    vec[3] = Vertex_Color(vec3(+w2, -h2, -d2), vec3(0, 0, -1), vec2(1.0f, 1.0f), color);

    // 뒷면 (법선: (0, 0, 1))
    vec[4] = Vertex_Color(vec3(-w2, -h2, +d2), vec3(0, 0, 1), vec2(1.0f, 1.0f), color);
    vec[5] = Vertex_Color(vec3(+w2, -h2, +d2), vec3(0, 0, 1), vec2(0.0f, 1.0f), color);
    vec[6] = Vertex_Color(vec3(+w2, +h2, +d2), vec3(0, 0, 1), vec2(0.0f, 0.0f), color);
    vec[7] = Vertex_Color(vec3(-w2, +h2, +d2), vec3(0, 0, 1), vec2(1.0f, 0.0f), color);

    // 윗면 (법선: (0, 1, 0))
    vec[8] = Vertex_Color(vec3(-w2, +h2, -d2), vec3(0, 1, 0), vec2(0.0f, 1.0f), color);
    vec[9] = Vertex_Color(vec3(-w2, +h2, +d2), vec3(0, 1, 0), vec2(0.0f, 0.0f), color);
    vec[10] = Vertex_Color(vec3(+w2, +h2, +d2), vec3(0, 1, 0), vec2(1.0f, 0.0f), color);
    vec[11] = Vertex_Color(vec3(+w2, +h2, -d2), vec3(0, 1, 0), vec2(1.0f, 1.0f), color);

    // 아랫면 (법선: (0, -1, 0))
    vec[12] = Vertex_Color(vec3(-w2, -h2, -d2), vec3(0, -1, 0), vec2(1.0f, 1.0f), color);
    vec[13] = Vertex_Color(vec3(+w2, -h2, -d2), vec3(0, -1, 0), vec2(0.0f, 1.0f), color);
    vec[14] = Vertex_Color(vec3(+w2, -h2, +d2), vec3(0, -1, 0), vec2(0.0f, 0.0f), color);
    vec[15] = Vertex_Color(vec3(-w2, -h2, +d2), vec3(0, -1, 0), vec2(1.0f, 0.0f), color);

    // 왼쪽면 (법선: (-1, 0, 0))
    vec[16] = Vertex_Color(vec3(-w2, -h2, +d2), vec3(-1, 0, 0), vec2(0.0f, 1.0f), color);
    vec[17] = Vertex_Color(vec3(-w2, +h2, +d2), vec3(-1, 0, 0), vec2(0.0f, 0.0f), color);
    vec[18] = Vertex_Color(vec3(-w2, +h2, -d2), vec3(-1, 0, 0), vec2(1.0f, 0.0f), color);
    vec[19] = Vertex_Color(vec3(-w2, -h2, -d2), vec3(-1, 0, 0), vec2(1.0f, 1.0f), color);

    // 오른쪽면 (법선: (1, 0, 0))
    vec[20] = Vertex_Color(vec3(+w2, -h2, -d2), vec3(1, 0, 0), vec2(0.0f, 1.0f), color);
    vec[21] = Vertex_Color(vec3(+w2, +h2, -d2), vec3(1, 0, 0), vec2(0.0f, 0.0f), color);
    vec[22] = Vertex_Color(vec3(+w2, +h2, +d2), vec3(1, 0, 0), vec2(1.0f, 0.0f), color);
    vec[23] = Vertex_Color(vec3(+w2, -h2, +d2), vec3(1, 0, 0), vec2(1.0f, 1.0f), color);
    vector<uint32> idx(36);

    // 앞면
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;
    // 뒷면
    idx[6] = 4; idx[7] = 5; idx[8] = 6;
    idx[9] = 4; idx[10] = 6; idx[11] = 7;
    // 윗면
    idx[12] = 8; idx[13] = 9; idx[14] = 10;
    idx[15] = 8; idx[16] = 10; idx[17] = 11;
    // 아랫면
    idx[18] = 12; idx[19] = 13; idx[20] = 14;
    idx[21] = 12; idx[22] = 14; idx[23] = 15;
    // 왼쪽면
    idx[24] = 16; idx[25] = 17; idx[26] = 18;
    idx[27] = 16; idx[28] = 18; idx[29] = 19;
    // 오른쪽면
    idx[30] = 20; idx[31] = 21; idx[32] = 22;
    idx[33] = 20; idx[34] = 22; idx[35] = 23;


    // Mesh 초기화 및 리소스 등록
    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleMesh(const float scale)
{

    std::wstringstream wss;
    wss << scale;
    wstring temp = wstring(wss.str());

    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(L"RECTANGLE" + temp);

    if (mesh)
    {
        return mesh;
    }

    vector<Vertex_GeoMetry> vec(4);

    //[0][2]
    //[1][3]
    // 앞면
    vec[0] = Vertex_GeoMetry(vec3(-scale, -scale, 0), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[1] = Vertex_GeoMetry(vec3(-scale, +scale, 0), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[2] = Vertex_GeoMetry(vec3(+scale, +scale, 0), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[3] = Vertex_GeoMetry(vec3(+scale, -scale, 0), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));

    vector<uint32> idx(6);

    // 앞면
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;

    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    ResourceManager::main->Add(L"RECTANGLE" + temp, mesh);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadGripMeshControlPoints(float width, float height, int division_x, int division_z, bool move)
{

    shared_ptr<Mesh> mesh = make_shared<Mesh>();

    const float dx = width / division_x;
    const float dz = height / division_z;

    vector<Vertex_GeoMetry> vertices;

    vertices.reserve((division_x + 1) * (division_z + 1));


    //leftBottom
    vec3 leftBottom = vec3(-0.5f * width, 0, -0.5f * height);

    for (int j = 0; j <= division_z; ++j)
    {
        vec3 startPoint = leftBottom + vec3(0, 0, dz * j);

        //아랫줄
        for (int i = 0; i <= division_x; ++i)
        {
            Vertex_GeoMetry v;
            //xz 평면에서 x 방향으로이동.
            v.position = startPoint;
            v.position.x += dx * i;
            v.normal = vec3(0, 1, 0);
            v.uv = vec2(float(i) / division_x, 1.0f - float(j) / division_z);
            vertices.push_back(v);
        }
    }


    vector<uint32> indices;

    for (int j = 0; j < division_z; ++j)
    {
        const int offset = (division_x + 1) * j;

        for (int i = 0; i < division_x; ++i)
        {
            // 4개의 정점으로 이루어진 패치를 구성
            indices.push_back(offset + i);
            indices.push_back(offset + i + 1);
            indices.push_back(offset + i + division_x + 1);
            indices.push_back(offset + i + 1 + division_x + 1);
        }
    }


    if(move)
    {

        for(auto& vertice : vertices)
        {
            Matrix mat = Matrix::CreateTranslation(width/2,0,height/2);
            vertice.position = vec3::Transform(vertice.position,mat);
        }
    }

    mesh->Init(vertices, indices);

    return mesh;

}

shared_ptr<Mesh> GeoMetryHelper::LoadGripMesh(float width, float height, int division_x, int division_z)
{
    shared_ptr<Mesh> mesh = make_shared<Mesh>();

    const float dx = width / division_x;
    const float dz = height / division_z;

    vector<Vertex_GeoMetry> vertices;

    vertices.reserve((division_x + 1) * (division_z + 1));


    //leftBottom
    vec3 leftBottom = vec3(-0.5f * width, 0, -0.5f * height);

    for (int j = 0; j <= division_z; ++j)
    {
        vec3 startPoint = leftBottom + vec3(0, 0, dz * j);

        //아랫줄
        for (int i = 0; i <= division_x; ++i)
        {
            Vertex_GeoMetry v;
            //xz 평면에서 x 방향으로이동.
            v.position = startPoint;
            v.position.x += dx * i;
            v.normal = vec3(0, 1, 0);
            v.uv = vec2(float(i) / division_x, 1.0f - float(j) / division_z);
            vertices.push_back(v);
        }
    }


    vector<uint32> indices;

    for (int j = 0; j < division_z; ++j)
    {
        const int offset = (division_x + 1) * j;

        for (int i = 0; i < division_x; ++i)
        {
            //윗쪽
            indices.push_back(offset + i);
            indices.push_back(offset + i + division_x + 1);
            indices.push_back(offset + (i + 1) + division_x + 1);

            //아랫쪽
            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + division_x + 1);
            indices.push_back(offset + i + 1);
        }
    }

	for(auto& vertice : vertices)
	{
        Matrix mat = Matrix::CreateTranslation(width/2,0,height/2);
        vertice.position = vec3::Transform(vertice.position,mat);
	}

    mesh->Init(vertices, indices);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadSprtieMesh()
{
    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(L"sprtieMesh");

    if (mesh)
    {
        return mesh;
    }

    vector<Vertex_Color> vec(4);

    // 앞면
    vec[0] = Vertex_Color(vec3(0.0f, 1.0f, 0),vec3(0,0,0), vec2(0.0f, 1.0f));
    vec[1] = Vertex_Color(vec3(0.0f, 0.0f, 0), vec3(0, 0, 0), vec2(0.0f, 0.0f));
    vec[2] = Vertex_Color(vec3(1.0f, 0.0f, 0), vec3(0, 0, 0), vec2(1.0f, 0.0f));
    vec[3] = Vertex_Color(vec3(1.0f, 1.0f, 0), vec3(0, 0, 0), vec2(1.0f, 1.0f));

    vector<uint32> idx = { 0,1,2,0,2,3 };

    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    ResourceManager::main->Add(L"sprtieMesh", mesh);

    return mesh;

}

shared_ptr<Mesh> GeoMetryHelper::LoadRectMesh()
{
    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(L"RectMesh");

    if(mesh) {
        return mesh;
    }

    vector<Vertex_Static> vec(4);

    // 앞면
    vec[0] = Vertex_Static(vec3(0.0f,1.0f,0),vec3(0,0,1), vec3(0,1,0),vec2(0.0f,0.0f));
    vec[1] = Vertex_Static(vec3(0.0f,0.0f,0),vec3(0,0,1),vec3(0,1,0),vec2(0.0f,1.0f));
    vec[2] = Vertex_Static(vec3(1.0f,0.0f,0),vec3(0,0,1),vec3(0,1,0),vec2(1.0f,1.0f));
    vec[3] = Vertex_Static(vec3(1.0f,1.0f,0),vec3(0,0,1),vec3(0,1,0),vec2(1.0f,0.0f));

    vector<uint32> idx = {0,2,1,0,3,2};

    mesh = make_shared<Mesh>();
    mesh->Init(vec,idx);

    ResourceManager::main->Add(L"RectMesh", mesh);

    return mesh;

}

shared_ptr<Mesh> GeoMetryHelper::LoadRectCenterMesh()
{
    // 이미 생성돼 있으면 반환
    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(L"RectMesh");
    if (mesh) return mesh;

    // 중앙이 (0,0,0) 이고, 좌표가 -0.5~+0.5 범위
    vector<Vertex_Static> vec(4);
    vec[0] = Vertex_Static(vec3(-0.5f, +0.5f, 0.0f), vec3(0, 0, 1), vec3(0, 1, 0), vec2(0.0f, 0.0f));
    vec[1] = Vertex_Static(vec3(-0.5f, -0.5f, 0.0f), vec3(0, 0, 1), vec3(0, 1, 0), vec2(0.0f, 1.0f));
    vec[2] = Vertex_Static(vec3(+0.5f, -0.5f, 0.0f), vec3(0, 0, 1), vec3(0, 1, 0), vec2(1.0f, 1.0f));
    vec[3] = Vertex_Static(vec3(+0.5f, +0.5f, 0.0f), vec3(0, 0, 1), vec3(0, 1, 0), vec2(1.0f, 0.0f));

    // 인덱스는 그대로
    vector<uint32> idx = { 0, 2, 1, 0, 3, 2 };

    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    ResourceManager::main->Add(L"RectMesh", mesh);
    return mesh;
}