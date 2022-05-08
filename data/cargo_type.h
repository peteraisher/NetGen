//  Copyright 2022 Peter Aisher
//
//  cargo_type.h
//  NetGen
//

#ifndef cargo_type_h
#define cargo_type_h


/// Enumeration for different cargo types which may be produced, transported and consumed
enum CargoType : size_t {
  Stone, CrudeOil, IronOre, Coal, Logs, Grain,
  ConstructionMaterials, Oil, Steel, Planks, Food,
  Fuel, Plastic, Tools,
  Goods, Machines,
  CargoTypeCount
};

#endif /* cargo_type_h */
