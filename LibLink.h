#include <iostream>
#include "SDL.h"
#include "png.h"


#define R2D 180/ M_PI
#define D2R M_PI/ 180  
struct Color
{
	uint8_t r, g, b, a;
};

struct ImageData
{
	size_t width;
	size_t height;
	size_t chanel;
	size_t sizeofRow;
	uint8_t* pixel;
};

struct Point2
{
	float x;
	float y;
};

struct Triangle
{
	Point2 A, B, C;
};

struct TriangleFactor
{
	float alpha, beta, gama;
};

Point2 Point2SubtractCovert(Point2 O, Point2 A)
{
	Point2 newP2;
	newP2.x = A.x - O.x;
	newP2.y = A.y - O.y;

	return newP2;
}


Triangle rotateUVPoint(float angle, Point2 pA, Point2 pB, Point2 pC);