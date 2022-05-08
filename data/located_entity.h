//  Copyright 2022 Peter Aisher
//
//  located_entity.h
//  NetGen
//

#ifndef located_entity_h
#define located_entity_h

#include "../vector/vector2.h"

/// An entity which has a 2D location
class LocatedEntity {
  Point2D _location;
public:
  inline LocatedEntity(Point2D location) : _location(location) {}
  inline Point2D location() const {return _location;}
};

#endif /* located_entity_h */
