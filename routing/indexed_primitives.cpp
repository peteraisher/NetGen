//  Copyright 2022 Peter Aisher
//
//  indexed_primitives.cpp
//  NetGen
//

#include "indexed_primitives.h"

bool IndexedTriangle::containsEdge(IndexedEdge edge) const {
  if (edge.a == a) {
    return edge.b == b || edge.b == c;
  } else if (edge.a == b) {
    return edge.b == a || edge.b == c;
  } else if (edge.a == c) {
    return edge.b == a || edge.b == b;
  }
  return false;
}

bool IndexedTriangle::sharesEdgeWith(const IndexedTriangle & other) const {
  for (const auto & edge : edges()) {
    if (other.containsEdge(edge)) {
      return true;
    }
  }
  return false;
}
