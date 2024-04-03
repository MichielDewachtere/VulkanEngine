#include "Mesh.h"

//void Mesh::AddTriangle(TrianglePosCol triangle, bool updateBuffer)
//{
//	AddVertex(triangle.vertex1);
//	AddVertex(triangle.vertex2);
//	AddVertex(triangle.vertex3);
//
//	//m_VecTriangles.push_back(triangle);
//	//if (updateBuffer && m_IsInitialized)
//	//	UpdateVertexBuffer();
//}
//
//void Mesh::AddTriangle(PosCol2D vertex1, PosCol2D vertex2, PosCol2D vertex3, bool updateBuffer)
//{
//	AddTriangle(TrianglePosCol(vertex1, vertex2, vertex3), updateBuffer);
//}
//
//void Mesh::AddQuad(QuadPosCol quad, bool updateBuffer)
//{
//	AddTriangle(TrianglePosCol(quad.vertex1, quad.vertex2, quad.vertex3), false);
//	AddTriangle(TrianglePosCol(quad.vertex3, quad.vertex4, quad.vertex1), updateBuffer);
//}
//
//void Mesh::AddQuad(PosCol2D vertex1, PosCol2D vertex2, PosCol2D vertex3, PosCol2D vertex4, bool updateBuffer)
//{
//	AddTriangle(TrianglePosCol(vertex1, vertex2, vertex3), false);
//	AddTriangle(TrianglePosCol(vertex3, vertex4, vertex1), updateBuffer);
//}