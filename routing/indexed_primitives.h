//  Copyright 2022 Peter Aisher
//
//  indexed_primitives.h
//  NetGen
//

#ifndef indexed_primitives_h
#define indexed_primitives_h

#include <array>


/// An edge, represented by the indices of each end
struct IndexedEdge {
  int a, b;
  inline IndexedEdge(int a, int b) : a(a), b(b) {}
  inline bool operator==(const IndexedEdge& other) const noexcept {
    return a == other.a && b == other.b;
  }
};

template<>
struct std::hash<IndexedEdge> {
  std::size_t operator()(IndexedEdge const& e) const noexcept {
    std::size_t h1 = std::hash<int>{}(e.a);
    std::size_t h2 = std::hash<int>{}(e.b);
    return h1 ^ ((h2 << 17) | (h2 >> 47));
  }
};


/// A Triangle, represented by the indices of its corners
struct IndexedTriangle {
  int a, b, c;
  inline IndexedTriangle(int a, int b, int c) : a(a), b(b), c(c) {}
  inline std::array<IndexedEdge, 3> edges() const {
    return {IndexedEdge{a, b}, {b, c}, {c, a}};
  }

  /// Does the triangle contain this edge
  /// @param edge the edge to find
  /// @returns true if the edge matches an edge in the triangle
  bool containsEdge(IndexedEdge edge) const;

  /// Does the triangle share an edge with another triangle
  /// @param other the triangle to find shared edges with
  /// @returns true if a matching edge is found
  bool sharesEdgeWith(const IndexedTriangle & other) const;
  inline bool operator==(const IndexedTriangle & other) const {
    return a == other.a && b == other.b && c == other.c;
  }
};

#endif /* indexed_primitives_h */
