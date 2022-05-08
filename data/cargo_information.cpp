//  Copyright 2022 Peter Aisher
//
//  cargo_information.cpp
//  NetGen
//

#include "cargo_information.h"

CargoInformation::CargoInformation() {
  for (size_t i = 0; i < CargoTypeCount; ++i) {
    CargoType ct = static_cast<CargoType>(i);
    WagonType wt = cargoWagonTypes[i];
    wagonCargoTypes[wt].push_back(ct);
  }
}
