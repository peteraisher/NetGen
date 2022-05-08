//  Copyright 2022 Peter Aisher
//
//  indexed_delaunay.cpp
//  NetGen
//

#include <utility>
#include "indexed_delaunay.h"


bool IndexedDelaunay::pointIsInCircumcircle(int i, IndexedTriangle tri) {
  auto a = vertices[tri.a];
  auto b = vertices[tri.b];
  auto c = vertices[tri.c];
  auto d = vertices[i];

  float d11 = a.x - d.x;
  float d21 = b.x - d.x;
  float d31 = c.x - d.x;

  float d12 = a.y - d.y;
  float d22 = b.y - d.y;
  float d32 = c.y - d.y;

  float d13 = a.x*a.x - d.x*d.x + a.y*a.y - d.y*d.y;
  float d23 = b.x*b.x - d.x*d.x + b.y*b.y - d.y*d.y;
  float d33 = c.x*c.x - d.x*d.x + c.y*c.y - d.y*d.y;

  float det = (d11 * d22 * d33) + (d21 * d32 * d13) + (d31 * d12 * d23)
            - (d31 * d22 * d13) - (d21 * d12 * d33) - (d11 * d32 * d23);
  return det > 0;
}

std::vector<IndexedEdge> IndexedDelaunay::nonSharedEdges(const std::vector<IndexedTriangle> &badTriangles) {
  std::vector<IndexedEdge> polygon {};
  for (const auto & tri : badTriangles) {
    for (const auto & edge : tri.edges()) {
      bool notShared = true;
      for (const auto & other_tri : badTriangles) {
        if (other_tri == tri) {
          continue;
        }
        if (other_tri.containsEdge(edge)) {
          notShared = false;
          break;
        }
      }
      if (notShared) {
        polygon.push_back(edge);
      }
    }
  }
  return polygon;
}

std::vector<IndexedTriangle> IndexedDelaunay::boundaryTriangles() {
  // build neighbors index
  // triangles without 3 neighbors are boundary
  std::vector<int> neighborCount {};
  std::vector<IndexedTriangle> boundaryTriangles {};
  const int count = triangleCount();
  neighborCount.resize(count);
  for (int i = 0; i < count; ++i) {
    if (isTriangleMasked(i)) {
      continue;
    }
    for (int j = i+1; j < count; ++j) {
      if (isTriangleMasked(j)) {
        continue;
      }
      if (triangles[i].sharesEdgeWith(triangles[j])) {
        ++neighborCount[i];
        ++neighborCount[j];
      }
    }
    if (neighborCount[i] < 3) {
      boundaryTriangles.push_back(triangles[i]);
    } else {
      assert(neighborCount[i] == 3);
    }
  }
  return boundaryTriangles;
}

std::vector<int> IndexedDelaunay::boundaryTriangleIndices() {
  // build neighbors index
  // triangles without 3 neighbors are boundary
  std::vector<int> neighborCount {};
  std::vector<int> boundaryTriangleIndices {};
  const int count = triangleCount();
  neighborCount.resize(count);
  for (int i = 0; i < count; ++i) {
    if (isTriangleMasked(i)) {continue;}
    for (int j = i+1; j < count; ++j) {
      if (isTriangleMasked(j)) {continue;}
      if (triangles[i].sharesEdgeWith(triangles[j])) {
        ++neighborCount[i];
        ++neighborCount[j];
      }
    }
    if (neighborCount[i] < 3) {
      boundaryTriangleIndices.push_back(i);
    } else {
      //assert(neighborCount[i] == 3);
    }
  }
  return boundaryTriangleIndices;
}

bool IndexedDelaunay::isSliver(int k, float cos_epsi_squared)  {
  IndexedTriangle tri = triangles[k];
  const auto edges = tri.edges();
  std::array<int, 3> next = {1, 2, 0};
  for (int i = 0; i < 3; ++i) {
    const int ii = next[i];
    float dx1 = vertices[edges[i].b].x - vertices[edges[i].a].x;
    float dy1 = vertices[edges[i].b].y - vertices[edges[i].a].y;
    float dx2 = vertices[edges[ii].b].x - vertices[edges[ii].a].x;
    float dy2 = vertices[edges[ii].b].y - vertices[edges[ii].a].y;
    float l1_squared = dx1 * dx1 + dy1 * dy1;
    float l2_squared = dx2 * dx2 + dy2 * dy2;
    float dot_next = dx1 * dx2 + dy1 * dy2;
    if (dot_next * dot_next > cos_epsi_squared * l1_squared * l2_squared) {
      return true;
    }
  }
  return false;
}

void IndexedDelaunay::maskSliverTrianglesOnBoundary(float epsi) {
  float ce = cos(epsi);
  float cos_epsi_squared = ce * ce;
  std::deque<int> todo {};
  std::vector<int> boundaryIndices = boundaryTriangleIndices();
  std::move(boundaryIndices.begin(), boundaryIndices.end(), std::back_inserter(todo));
  while (!todo.empty()) {
    auto i = todo.front();
    if (isTriangleMasked(i)) {
      continue;
    }
    if (isSliver(i, cos_epsi_squared)) {
      // TODO: cache neighbors to avoid this search
      for (int j = 0; j < triangleCount(); ++j) {
        if (i == j) {
          continue;
        }
        if (isTriangleMasked(j)) {
          continue;
        }
        if (triangles[i].sharesEdgeWith(triangles[j])) {
          todo.push_back(j);
        }
      }
      maskTriangleAtIndex(i);
    }
    todo.pop_front();
  }
}

std::vector<IndexedTriangle> IndexedDelaunay::trianglesWithCircumcircleContainingVertex(int i) {
  std::vector<IndexedTriangle> badTriangles {};
  for (const auto & tri : triangles) {
    if (pointIsInCircumcircle(i, tri)) {
      badTriangles.push_back(tri);
    }
  }
  return badTriangles;
}

void IndexedDelaunay::insertPointAndFixTriangulation(int i) {
  std::vector<IndexedTriangle> badTriangles = trianglesWithCircumcircleContainingVertex(i);
  std::vector<IndexedEdge> polygon = nonSharedEdges(badTriangles);
  removeTriangles(badTriangles);
  for (const auto & edge : polygon) {
    IndexedTriangle newTri {edge.a, edge.b, i};
    makeCCW(newTri); // ought to be already ccw
    triangles.push_back(newTri);
  }
}

std::array<Point2D, 3> IndexedDelaunay::triangleContainingBox(const BBox &boundingBox) {
  float xmin = boundingBox.bl.x;
  float ymin = boundingBox.bl.y;
  float xmax = boundingBox.tr.x;
  float ymax = boundingBox.tr.y;
  std::array<Point2D, 3> superTriangle = {
    Point2D{xmin, ymin}, {xmax + xmax - xmin, ymin}, {xmin, ymax + ymax - ymin}
  };
  return superTriangle;
}

IndexedDelaunay::IndexedDelaunay(std::vector<Point2D> points)
: vertices(points) {
  BBox boundingBox(points);
  boundingBox.extendByAbsoluteDistance(10.f);

  std::array<Point2D, 3> superTriangle = triangleContainingBox(boundingBox);
  int supertriangleStartIndex = static_cast<int>(vertices.size());

  for (auto & p : superTriangle) {
    vertices.push_back(p);
  }
  IndexedTriangle superTri = {supertriangleStartIndex,
                              supertriangleStartIndex + 1,
                              supertriangleStartIndex + 2};
  triangles.push_back(superTri);
  for (int i = 0; i < supertriangleStartIndex; ++i) {
    insertPointAndFixTriangulation(i);
  }
  removeTrianglesWithSupertriangleVertices(supertriangleStartIndex);
  vertices.resize(supertriangleStartIndex);
  mask.resize(triangleCount());
  std::fill(mask.begin(), mask.end(), false);
  underConstruction = false;
}
