#pragma once

#include "vector2d.hpp"

class Wheel;

class Line
{
public:
	Line(Vector2d start, Vector2d end);
	void HandleCollision(Wheel&) const;
	Vector2d start_;
	Vector2d end_;
};
