//  Copyright 2022 Peter Aisher
//
//  supply_chain_information.h
//  NetGen
//

#ifndef supply_chain_information_h
#define supply_chain_information_h

#include "cargo_type.h"
#include "cargo_requirement.h"
#include <array>


/// Supply chain information
///
/// Stores information regarding industry cargo requirements for production
class SupplyChainInformation {
  std::array<CargoRequirements, CargoTypeCount> industryRequirements {
    CargoRequirements(), CargoRequirements(), CargoRequirements(), CargoRequirements(), CargoRequirements(), CargoRequirements(),
    {{Stone, 1.0f}}, {{CrudeOil, 2.0f}}, {{IronOre, 2.0f}, {Coal, 2.0f}}, {{Logs, 2.0f}}, {{Grain, 2.0f}},
    {{Oil, 1.0f}}, {{Oil, 1.0f}}, {{Planks, 1.0f}},
    {{Steel, 1.0f}, {Plastic, 1.0f}}, {{Steel, 1.0f}, {Planks, 1.0f}}
  };
public:
  inline CargoRequirements requirementsForIndustryProducing(CargoType cargo) {return industryRequirements[cargo];}
};

#endif /* supply_chain_information_h */
