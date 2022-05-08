//  Copyright 2022 Peter Aisher
//
//  wagon_type.h
//  NetGen
//

#ifndef wagon_type_h
#define wagon_type_h

/// Enumeration for the different wagon types for transporting cargo
enum WagonType : size_t {
  Gondola, TankCar, FlatcarWithSideStakes, Boxcar,
  WagonTypeCount
};

#endif /* wagon_type_h */
