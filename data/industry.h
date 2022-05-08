//  Copyright 2022 Peter Aisher
//
//  industry.h
//  NetGen
//

#ifndef industry_h
#define industry_h

#include "located_entity.h"
#include "vector2.h"
#include "cargo_type.h"
#include "industry_info.h"

/// Represents a producing industry
class Industry: public LocatedEntity {
  CargoType cargoProduced;
public:
  Industry(Point2D location, CargoType cargoProduced)
    : LocatedEntity(location), cargoProduced(cargoProduced) {}
  inline float maxOutput() {
    return IndustryInfo::maxProduction(cargoProduced);
  }
  inline CargoType outputType() const {return cargoProduced;}
};

#endif /* industry_h */
