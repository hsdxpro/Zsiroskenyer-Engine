#include "Point.h"

Point::Point(int32 X, int32 Y)
:x(X), y(Y) {
}

Point Point::operator - (const Point& p) const {
	return Point(x - p.x, y - p.y);
}

Vec2 Point::operator - (const Vec2& p) const {
	return Vec2(x - p.x, y - p.y);
}

Point Point::operator + (const Point& p) const {
	return Point(x + p.x, y + p.y);
}

Point Point::operator / (const Point& p) const {
	return Point(x / p.x, y / p.y);
}

Point Point::operator * (const Point& p) const {
	return Point(x * p.x, y * p.y);
}