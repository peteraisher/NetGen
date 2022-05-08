//  Copyright 2022 Peter Aisher
//
//  town.h
//  NetGen
//

#ifndef town_h
#define town_h

#include <string>
#include "vector2.h"
#include "located_entity.h"

/// Represents a town, which consumes two types of cargo
class Town: public LocatedEntity {
  std::array<CargoType, 2> _cargoRequired;
  std::string _name;
public:
  inline Town(Point2D location, std::array<CargoType, 2> requirements, const char* name)
    : LocatedEntity(location), _cargoRequired(requirements), _name(name) {}
  inline std::array<CargoType, 2> cargoRequired() const {return _cargoRequired;}
  inline const std::string& name() const {return _name;}
};

#endif /* town_h */
