//  Copyright 2022 Peter Aisher
//
//  vector2.cpp
//  NetGen
//

#include "vector2.h"

Vector2D Point2D::operator-(const Point2D& p) const {return Vector2D(x - p.x, y - p.y);}
Point2D Point2D::operator-(const Vector2D& v) const {return Point2D(x - v.x, y - v.y);}
Point2D Point2D::operator+(const Vector2D& v) const {return Point2D(x + v.x, y + v.y);}

bool pointsAreCCW(Point2D a, Point2D b, Point2D c) {
  return (b-a).cross(c-a) > 0;
}
