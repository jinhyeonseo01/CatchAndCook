#include "pch.h"
#include "GraphData.h"

 std::unordered_map<int, vec3> GraphData::datas;

void GraphData::GenVertices(const vec3& center, const vec3& extents, float spacing)
{
    datas.clear();

    int index = 0;

    for (float x = center.x - extents.x; x <= center.x + extents.x; x += spacing)
    {
        for (float y = center.y - extents.y; y <= center.y + extents.y; y += spacing)
        {
            for (float z = center.z - extents.z; z <= center.z + extents.z; z += spacing)
            {
                datas[index++] = vec3(x, y, z);
            }
        }
    }

    cout << "Graph Verices Count: " <<  datas.size() << endl;
}