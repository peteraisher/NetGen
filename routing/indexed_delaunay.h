//  Copyright 2022 Peter Aisher
//
//  indexed_delaunay.h
//  NetGen
//

#ifndef indexed_delaunay_h
#define indexed_delaunay_h

#include <vector>
#include <array>
#include <algorithm>
#include <deque>
#include <iterator>
#include "vector2.h"
#include "indexed_primitives.h"
#include "bbox.h"

//typedef dt::Vector2<float> Point;


/// Indexed Delaunay Triangulation
///
/// Constructs a delaunay triangulation using the Bowyer–Watson algorithm
/// see pseudocode at
/// https://en.wikipedia.org/wiki/Bowyer–Watson_algorithm#Pseudocode
class IndexedDelaunay {
  /* variables for data storage */
  std::vector<Point2D> vertices {};
  std::vector<IndexedTriangle> triangles;
  std::vector<bool> mask {};

  /// Flag set when triangulation is being constructed
  bool underConstruction = true;

  /// Check if point is contained in circumcircle of triangle
  /// @param i index of point to check
  /// @param tri triangle to check
  /// @returns true, if point is within circumcircle
  bool pointIsInCircumcircle(int i, IndexedTriangle tri);

  /// Check if triangle has points in CCW order
  /// @param tri the triangle to check
  /// @returns true if the points are in CCW order
  inline bool isCCW(IndexedTriangle tri) {
    auto a = vertices[tri.a];
    auto b = vertices[tri.b];
    auto c = vertices[tri.c];
    return pointsAreCCW(a, b, c);
  }

  /// Make a triangle CCW if not already so
  /// @param tri the triangle to enforce directionality of
  inline void makeCCW(IndexedTriangle& tri) {
    if (!isCCW(tri)) {
      int t = tri.b;
      tri.b = tri.c;
      tri.c = t;
    }
  }

  /// Remove triangles which share vertices with the
  /// supertriangle used for construction
  /// @param supertriangleStartIndex index of first supertriangle vertex
  inline void removeTrianglesWithSupertriangleVertices(int supertriangleStartIndex) {
    auto res = std::remove_if(triangles.begin(), triangles.end(), [supertriangleStartIndex](auto const tri) { return tri.a >= supertriangleStartIndex || tri.b >= supertriangleStartIndex || tri.c >= supertriangleStartIndex;});
    triangles.erase(res, triangles.end());
  }

  /// permanently remove triangles
  /// @param toRemove triangles to remove
  inline void removeTriangles(std::vector<IndexedTriangle> &toRemove) {
    auto res = std::remove_if(triangles.begin(), triangles.end(), [&toRemove](auto const x) { return std::find(toRemove.begin(), toRemove.end(), x) != toRemove.end(); });
    triangles.erase(res, triangles.end());
  }

  /// Triangles whose circumcircle contains vertex i
  /// @param i index of vertex
  /// @returns vector of indexed trianges whose circumcircle contains vertex i
  std::vector<IndexedTriangle> trianglesWithCircumcircleContainingVertex(int i);

  /// Edges not shared by multiple triangles
  /// @param tris the triangles to check for non-shared edges
  std::vector<IndexedEdge> nonSharedEdges(const std::vector<IndexedTriangle> &tris);
  void insertPointAndFixTriangulation(int i);

  /// The number of triangles (including masked) in the triangluation
  inline int triangleCount() const {return static_cast<int>(triangles.size());}

  /// The triangles on the boundary of the triangulation
  std::vector<IndexedTriangle> boundaryTriangles();
  /// The indices of triangles on the boundary of the triangulation
  std::vector<int> boundaryTriangleIndices();

  /// Check if triangle is a sliver
  /// @param i the index of the triangle to check
  /// @param cos_epsi_squared (cos(epsi))^2, where epsi is
  /// the (radian) angle below which a corner is considered sharp
  /// @returns true, if any corner is sharper than epsi
  bool isSliver(int i, float cos_epsi_squared);

  /// Check if triangle is masked
  /// @param i index of triangle to check
  inline bool isTriangleMasked(int i) const {
    if (underConstruction) {return false;}
    return mask[i];
  }

  /// Mask triangle
  /// @param i index of triangle to mask
  inline void maskTriangleAtIndex(int i) {mask[i] = true;}

  /// Create a triangle entirely containg a given box
  /// @param boundingBox the counding box which should be entirely contained
  std::array<Point2D, 3> triangleContainingBox(const BBox &boundingBox);

public:

  /// Mask boundary sliver triangles
  /// @param epsi the (radian) angle below which a corner is considered sharp
  ///
  /// recursively mask sliver triangles which lie on the boundary until none remain
  void maskSliverTrianglesOnBoundary(float epsi);

  template <class E, class W, class V>
  friend class IntGraph;
  friend class Map;

  /// Construct an empty triangulation
  inline IndexedDelaunay() {};

  /// Construct a triangulation of the points
  IndexedDelaunay(std::vector<Point2D> points);
};

#endif /* indexed_delaunay_h */
