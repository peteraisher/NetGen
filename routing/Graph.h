//  Copyright 2022 Peter Aisher
//
//  Graph.h
//  NetGen
//

#ifndef Graph_h
#define Graph_h

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include "indexed_primitives.h"
#include "indexed_delaunay.h"

/// Represents a graph with edge, weight and vertex information
template <class E, class W, class V>
class IntGraph {
  /*
   implemented as dictionary of dictionaries
   vertex may be any hashable type,
   edge may be any type
   */

  typedef std::pair<W, E> WeightedEdge;

  std::unordered_map<int, std::unordered_map<int, WeightedEdge>> storage;
  std::unordered_map<int, V> nodes;

public:

  friend class Map;

  /// Construct an empty graph with no edges and no vertices;
  inline IntGraph() {}

  /// Add an edge to the graph
  ///
  /// @param edge the edge to add or update
  /// @param weight the weight to assign (optional - default constructed otherwise)
  /// @param bidirectional whether to also add the reversed edge
  ///
  /// @note if the edge already exists, this resets the weight of the edge
  /// to the given value (if present) or to a default constructed value
  void addEdge(IndexedEdge edge, W weight = W(), bool bidirectional = true) {
    storage[edge.a][edge.b].first = weight;
    if (bidirectional) {
      storage[edge.b][edge.a].first = weight;
    }
  }

  /// Remove an edge from the graph
  ///
  /// @param edge the edge to remove
  /// @param bidirectional whether to also remove the reversed edge
  ///
  /// @note if the edge is not present, this method has no effect
  void removeEdge(IndexedEdge edge, bool bidirectional = true) {
    storage[edge.a].erase(edge.b);
    if (bidirectional) {
      storage[edge.b].erase(edge.a);
    }
  }

  /// Get the neighbors of a node
  ///
  /// @param node the index of the node
  ///
  /// @returns a map from neighboring nodes to the weighted edges connecting them
  std::unordered_map<int, WeightedEdge>& getNeighbors(int node) {
    return storage[node];
  }

  /// Get all edges
  ///
  /// @returns a set containing all edges
  std::unordered_set<IndexedEdge> allEdges() {
    std::unordered_set<IndexedEdge> result {};
    for (const auto& a : storage) {
      for (const auto& b : a.second) {
        result.emplace(a.first, b.first);
      }
    }
    return result;
  }

  /// Construct a graph by adding all edges from a Delaunay triangulation
  ///
  /// @param dt the triangulation to use.
  ///
  /// @note sets edge weights to the distance between nodes
  /// @note only considers unmasked triangles from dt
  IntGraph(const IndexedDelaunay& dt) {
    for (int i = 0; i < dt.triangleCount(); ++i) {
      if (dt.isTriangleMasked(i)) {
        continue;
      }
      for (auto & edge : dt.triangles[i].edges()) {
        const auto& vb = dt.vertices[edge.b];
        const auto& va = dt.vertices[edge.a];
        const auto l = vb - va;
        addEdge(edge, l.length());
      }
    }
  }

};

#endif /* Graph_h */
