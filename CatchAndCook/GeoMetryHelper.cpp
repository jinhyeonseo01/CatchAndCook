#include "pch.h"
#include "GeoMetryHelper.h"
#include "Mesh.h"

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleBox(const float scale)
{

    // ���ҽ� Ű ����
    std::wstring key = L"box" + std::to_wstring(scale);

    // �̹� �ε�� Mesh�� �ִٸ� ��ȯ
    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(key);
    if (mesh)
    {
        return mesh;
    }

    // Vertex �� Index ������ ����
    vector<Vertex_Static> vertices;
    vertices.reserve(24); // 6�� x 4�� ����

    vector<uint32> indices;
    indices.reserve(36); // 6�� x 2���� �ﰢ�� x 3���� �ε���


    float w2 = scale;
    float h2 = scale;
    float d2 = scale;

    vector<Vertex_Static> vec(24);

    // �ո�
    vec[0] = Vertex_Static(vec3(-w2, -h2, -d2), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[1] = Vertex_Static(vec3(-w2, +h2, -d2), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[2] = Vertex_Static(vec3(+w2, +h2, -d2), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[3] = Vertex_Static(vec3(+w2, -h2, -d2), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));
    // �޸�
    vec[4] = Vertex_Static(vec3(-w2, -h2, +d2), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    vec[5] = Vertex_Static(vec3(+w2, -h2, +d2), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    vec[6] = Vertex_Static(vec3(+w2, +h2, +d2), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
    vec[7] = Vertex_Static(vec3(-w2, +h2, +d2), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
    // ����
    vec[8] = Vertex_Static(vec3(-w2, +h2, -d2), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
    vec[9] = Vertex_Static(vec3(-w2, +h2, +d2), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    vec[10] = Vertex_Static(vec3(+w2, +h2, +d2), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    vec[11] = Vertex_Static(vec3(+w2, +h2, -d2), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
    // �Ʒ���
    vec[12] = Vertex_Static(vec3(-w2, -h2, -d2), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f));
    vec[13] = Vertex_Static(vec3(+w2, -h2, -d2), vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f));
    vec[14] = Vertex_Static(vec3(+w2, -h2, +d2), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f));
    vec[15] = Vertex_Static(vec3(-w2, -h2, +d2), vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f));
    // ���ʸ�
    vec[16] = Vertex_Static(vec3(-w2, -h2, +d2), vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f));
    vec[17] = Vertex_Static(vec3(-w2, +h2, +d2), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f));
    vec[18] = Vertex_Static(vec3(-w2, +h2, -d2), vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f));
    vec[19] = Vertex_Static(vec3(-w2, -h2, -d2), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f));
    // �����ʸ�
    vec[20] = Vertex_Static(vec3(+w2, -h2, -d2), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f));
    vec[21] = Vertex_Static(vec3(+w2, +h2, -d2), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
    vec[22] = Vertex_Static(vec3(+w2, +h2, +d2), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
    vec[23] = Vertex_Static(vec3(+w2, -h2, +d2), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f));

    vector<uint32> idx(36);

    // �ո�
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;
    // �޸�
    idx[6] = 4; idx[7] = 5; idx[8] = 6;
    idx[9] = 4; idx[10] = 6; idx[11] = 7;
    // ����
    idx[12] = 8; idx[13] = 9; idx[14] = 10;
    idx[15] = 8; idx[16] = 10; idx[17] = 11;
    // �Ʒ���
    idx[18] = 12; idx[19] = 13; idx[20] = 14;
    idx[21] = 12; idx[22] = 14; idx[23] = 15;
    // ���ʸ�
    idx[24] = 16; idx[25] = 17; idx[26] = 18;
    idx[27] = 16; idx[28] = 18; idx[29] = 19;
    // �����ʸ�
    idx[30] = 20; idx[31] = 21; idx[32] = 22;
    idx[33] = 20; idx[34] = 22; idx[35] = 23;


    // Mesh �ʱ�ȭ �� ���ҽ� ���
    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);
    ResourceManager::main->Add(key, mesh);

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

    vector<Vertex_Static> vec(4);

    // �ո�
    vec[0] = Vertex_Static(vec3(-scale, -scale, 0), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[1] = Vertex_Static(vec3(-scale, +scale, 0), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[2] = Vertex_Static(vec3(+scale, +scale, 0), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[3] = Vertex_Static(vec3(+scale, -scale, 0), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));

    vector<uint32> idx(6);

    // �ո�
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;

    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    ResourceManager::main->Add(L"RECTANGLE" + temp, mesh);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadGripMesh(float width, float height, int division_x, int division_z)
{

    shared_ptr<Mesh> mesh = make_shared<Mesh>();

    const float dx = width / division_x;
    const float dz = height / division_z;

    vector<Vertex_Static> vertices;

    vertices.reserve((division_x + 1) * (division_z + 1));


    //leftBottom
    vec3 leftBottom = vec3(-0.5f * width, 0, -0.5f * height);

    for (int j = 0; j <= division_z; ++j)
    {
        vec3 startPoint = leftBottom + vec3(0, 0, dz * j);

        //�Ʒ���
        for (int i = 0; i <= division_x; ++i)
        {
            Vertex_Static v;
            //xz ��鿡�� x ���������̵�.
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
            //����
            indices.push_back(offset + i);
            indices.push_back(offset + i + division_x + 1);
            indices.push_back(offset + (i + 1) + division_x + 1);

            //�Ʒ���
            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + division_x + 1);
            indices.push_back(offset + i + 1);
        }
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

    vector<Vertex_Sprite> vec(4);

    // �ո�
    vec[0] = Vertex_Sprite(vec3(0.0f, 1.0f, 0), vec2(0.0f, 1.0f));
    vec[1] = Vertex_Sprite(vec3(0.0f, 0.0f, 0), vec2(0.0f, 0.0f));
    vec[2] = Vertex_Sprite(vec3(1.0f, 0.0f, 0), vec2(1.0f, 0.0f));
    vec[3] = Vertex_Sprite(vec3(1.0f, 1.0f, 0), vec2(1.0f, 1.0f));

    vector<uint32> idx = { 0,1,2,0,2,3 };

    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    ResourceManager::main->Add(L"sprtieMesh", mesh);

    return mesh;

}
