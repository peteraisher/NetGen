//  Copyright 2022 Peter Aisher
//
//  cargo_information.h
//  NetGen
//

#ifndef cargo_information_h
#define cargo_information_h

#include <array>
#include <vector>
#include "cargo_type.h"
#include "wagon_type.h"


/// Class which stores information about cargo and wagon types
class CargoInformation {
  std::array<WagonType, CargoTypeCount> cargoWagonTypes = {
    Gondola, TankCar, Gondola, Gondola, FlatcarWithSideStakes, Gondola,
    FlatcarWithSideStakes, TankCar, FlatcarWithSideStakes, FlatcarWithSideStakes, Boxcar,
    TankCar, Boxcar, Boxcar,
    Boxcar, Boxcar
  };
  std::array<std::vector<CargoType>, WagonTypeCount> wagonCargoTypes;
public:
  CargoInformation();
  inline WagonType wagonTypeForCargo(CargoType c) const { return cargoWagonTypes[c];}
};

#endif /* cargo_information_h */
