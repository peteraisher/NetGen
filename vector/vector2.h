//  Copyright 2022 Peter Aisher
//
//  vector2.h
//  NetGen
//

#ifndef vector2_h
#define vector2_h

#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <vector>


struct Pair2D {
  float x;
  float y;
  Pair2D() = default;
  Pair2D(const Pair2D& p) = default;
  Pair2D(Pair2D&&) = default;
  Pair2D(const float px, const float py) : x(px), y(py) {}

  Pair2D &operator=(const Pair2D& p) = default;
  Pair2D &operator=(Pair2D&&) = default;
  bool operator ==(const Pair2D& p) const {return x == p.x && y == p.y;}
};

struct Vector2D;

struct Point2D : Pair2D {
  Point2D() = default;
  Point2D(const Point2D& p) = default;
  Point2D(Point2D&&) = default;
  Point2D(const float px, const float py) : Pair2D(px, py) {}

  Point2D &operator=(const Point2D& p) = default;
  Point2D &operator=(Point2D&&) = default;
  inline bool operator ==(const Point2D& p) const {return x == p.x && y == p.y;}

  Vector2D operator-(const Point2D& p) const;
  Point2D operator-(const Vector2D& v) const;
  Point2D operator+(const Vector2D& v) const;
};

struct Vector2D: Pair2D {
  Vector2D() = default;
  Vector2D(const Vector2D& p) = default;
  Vector2D(Vector2D&&) = default;
  Vector2D(const float px, const float py) : Pair2D(px, py) {}

  Vector2D &operator=(const Vector2D& p) = default;
  Vector2D &operator=(Vector2D&&) = default;
  inline bool operator ==(const Vector2D& p) const {return x == p.x && y == p.y;}

  inline Vector2D operator+(const Vector2D& p) const {return Vector2D(x + p.x, y + p.y);}
  inline Vector2D operator-(const Vector2D& p) const {return Vector2D(x - p.x, y - p.y);}
  inline Point2D operator+(const Point2D& p) const {return Point2D(x + p.x, y + p.y);}

  inline float lengthSquared() const {return x*x + y*y;}
  inline float length() const {return sqrtf(lengthSquared());}
  inline float cross(const Vector2D& v) const {return x * v.y - y * v.x;}
};

typedef std::vector<Point2D> Line2D;

bool pointsAreCCW(Point2D a, Point2D b, Point2D c);

struct LineSegment {
  Point2D a;
  Point2D b;
  bool intersects(const LineSegment& s) const {
    return (pointsAreCCW(a, s.a, s.b) != pointsAreCCW(b, s.a, s.b))
      && (pointsAreCCW(a, b, s.a) != pointsAreCCW(a, b, s.b));
  }
  inline LineSegment(const Point2D& a, const Point2D& b) : a(a), b(b) {};
  inline bool intersectsLine(Line2D line) const {
    for (int i = 0, j = 1; j < line.size(); ++i, ++j) {
      LineSegment l(line[i], line[j]);
      if ((*this).intersects(l)) {
        return true;
      }
    }
    return false;
  }
};

#endif /* vector2_h */
