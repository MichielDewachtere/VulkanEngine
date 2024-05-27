#ifndef PIPELINEENUMS_H
#define PIPELINEENUMS_H

namespace real
{
	enum class EPrimitiveTopology
	{
		pointList = 0,
		lineList = 1,
		lineStrip = 2,
		triangleList = 3,
		triangleStrip = 4,
		triangleFan = 5,
		lineListAdjacency = 6,
		lineStripAdjacency = 7,
		triangleListAdjacency = 8,
		triangleStripAdjacency = 9,
		patchList = 10
	};

	enum class ERenderMode
	{
		filled = 0,
		lines = 1,
		points = 2
	};

	enum class ECullMode
	{
		back = 0,
		front = 1,
		both = 2,
		none = 3
	};
}

#endif // PIPELINEENUMS_H