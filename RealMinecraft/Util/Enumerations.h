#ifndef GAMEENUMERATIONS_H
#define GAMEENUMERATIONS_H

enum class EBlock : int
{
	air = 0,
	stone = 1,
	dirt = 2,
	grassBlock = 3,
	debug = 4,
	oakLog = 5
};

enum class EBlockType : char
{
	block = 0,
	cube = 1,
	cubeAll = 2,
	cubeBottomTop = 3,
	cubeColumn = 4,
};

enum class EDirection : char
{
	north = 0,
	east = 1,
	south = 2,
	west = 3,
	up = 4,
	down = 5,
	amountOfDirections = 6
};

#endif // GAMEENUMERATIONS_H