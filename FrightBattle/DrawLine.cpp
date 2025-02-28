#include "DrawLine.h"

using namespace DirectX;

DrawLine* DrawLine::Get()
{
	static DrawLine instance;
	return &instance;
}

void DrawLine::Update()
{

}

void DrawLine::Draw(const BoundingBox& bbox)
{


}

void DrawLine::GetBoundingBoxVertices(const BoundingBox& bbox)
{
    XMFLOAT3 min;
    min.x = bbox.Center.x - bbox.Extents.x;
    min.y = bbox.Center.y - bbox.Extents.y;
    min.z = bbox.Center.z - bbox.Extents.z;

    XMFLOAT3 max;
    max.x = bbox.Center.x + bbox.Extents.x;
    max.y = bbox.Center.y + bbox.Extents.y;
    max.z = bbox.Center.z + bbox.Extents.z;

    Vertices[0] = XMFLOAT3(min.x, min.y, min.z);
    Vertices[1] = XMFLOAT3(max.x, min.y, min.z);
    Vertices[2] = XMFLOAT3(min.x, max.y, min.z);
    Vertices[3] = XMFLOAT3(max.x, max.y, min.z);
    Vertices[4] = XMFLOAT3(min.x, min.y, max.z);
    Vertices[5] = XMFLOAT3(max.x, min.y, max.z);
    Vertices[6] = XMFLOAT3(min.x, max.y, max.z);
    Vertices[7] = XMFLOAT3(max.x, max.y, max.z);
}
