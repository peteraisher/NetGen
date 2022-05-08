//  Copyright 2022 Peter Aisher
//
//  bbox.h
//  NetGen
//

#ifndef bbox_h
#define bbox_h

#include "vector2.h"

struct BBox {
  Point2D bl;
  Point2D tr;
  inline void extendToInclude(Point2D point) {
    bl.x = fmin(bl.x, point.x);
    bl.y = fmin(bl.y, point.y);
    tr.x = fmax(bl.x, point.x);
    tr.y = fmax(bl.y, point.y);
  }
  BBox(LineSegment& l) {
    bl.x = fmin(l.a.x, l.b.x);
    bl.y = fmin(l.a.y, l.b.y);
    tr.x = fmax(l.a.x, l.b.x);
    tr.y = fmax(l.a.y, l.b.y);
  }
  BBox(const std::vector<Point2D>& points) {
    if (points.empty()) { return; }
    bl = points.front();
    tr = points.front();
    auto it = points.begin();
    ++it;
    while (it != points.end()) {
      extendToInclude(*it);
      ++it;
    }
  }
  bool intersects(BBox& other) const {
    return bl.x < other.tr.x && other.bl.x < tr.x
    && bl.y < other.tr.y && other.bl.y < tr.y;
  }
  void extendByAbsoluteDistance(float d) {
    bl.x -= d;
    bl.y -= d;
    tr.x += d;
    tr.y += d;
  }

  BBox operator|(const BBox& other) const {
    BBox result = *this;
    result.extendToInclude(other.bl);
    result.extendToInclude(other.tr);
    return result;
  }
};

#endif /* bbox_h */
