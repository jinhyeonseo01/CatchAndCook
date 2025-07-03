#include "pch.h"
#include "GraphData.h"

std::unordered_map<LeftRight, std::unordered_map<int, vec3>> GraphData::datas;


void GraphData::GenVertices(const vec3& center, const vec3& extents, float spacing)
{
    datas.clear();

    int leftIndex = 0;
    int rightIndex = 0;

    for (float x = center.x - extents.x; x <= center.x + extents.x; x += spacing)
    {
        for (float y = center.y - extents.y; y <= center.y + extents.y; y += spacing)
        {
            for (float z = center.z - extents.z; z <= center.z + extents.z; z += spacing)
            {
                vec3 pos(x, y, z);

                if (x < center.x)
                    datas[LeftRight::Left][leftIndex++] = pos;
                else
                    datas[LeftRight::Right][rightIndex++] = pos;
            }
        }
    }

    std::cout << "Graph Vertices Count: "
        << "Left = " << datas[LeftRight::Left].size()
        << ", Right = " << datas[LeftRight::Right].size()
        << ", Total = " << datas[LeftRight::Left].size() + datas[LeftRight::Right].size()
        << std::endl;
}