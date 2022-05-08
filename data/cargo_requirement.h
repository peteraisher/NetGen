//  Copyright 2022 Peter Aisher
//
//  cargo_requirement.h
//  NetGen
//

#ifndef cargo_requirement_h
#define cargo_requirement_h

#include <vector>


/// Cargo type and quantity consumed per unit of cargo produced
struct CargoRequirement {
  CargoType cargoType;
  float quantity;
};

typedef std::vector<CargoRequirement> CargoRequirements;

#endif /* cargo_requirement_h */
