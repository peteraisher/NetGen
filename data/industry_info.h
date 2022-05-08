//  Copyright 2022 Peter Aisher
//
//  industry_info.h
//  NetGen
//

#ifndef industry_info_h
#define industry_info_h

#include <array>
#include <string>
#include "cargo_type.h"


/// Struct containing information about different industries
struct IndustryInfo {
  static const std::array<std::string, CargoTypeCount> industryNames;
  static inline float maxProduction(CargoType t) {
    if (t == Grain) {
      return 200.f;
    } else {
      return 400.f;
    }
  }
  static inline std::string nameOfIndustryProducing(CargoType t) {
    return industryNames[t];
  }
};

#endif /* industry_info_h */
