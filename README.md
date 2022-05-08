# NetGen
A tool to generate efficient cargo networks (in Transport Fever 2)

The `Map` class represents a map, to which `Town` and `Industry` instances can be added.
The class creates a delaunay triangulation network linking all towns and industries,
optionally removing edges which cross impassable lines.
A network which minimises cost by using empty return journey capacity is generated.

## Features

- Uses a custom graph data structure to store directional weight and cargo flow information per edge
- Generates tab separated text output suitable for import into data visualisation softare or
  futher processing
- Well documented source code

## Documentation

The source code contains documentation of the main functions and types.

`CargoType` and `WagonType` enumerations represent cargo and wagon types.

`Town` and `Industry` classes represent towns and industries.

### Initialization

`Map` is initialized empty and information is then added using the following methods:
- `addTown(_)` to add a town
- `addIndustry(_)` to add an industry
- `addImpassableLine(_)` to add an impassable line

### Network Generation

A network can be generated using te following methods:
- `triangulateAllLocations()` to create the initial triangulation between all towns and industries
- `buildNetworkGraph()` to build the network graph in which edges crossing impassable lines are removed
  from the triangulation
- `setUniformTownCargoRequirement(_)` to set a uniform consumption demand for all towns
- `makeAllConnections()` to make all connections for cargo required

### Reporting network structure
The network structure can be inspected using the following methods:
- `printIndustryInfo()` to print node id, type and location of all industries
- `printTownInfo()` to print node id name and location of all towns
- `printAllPaths()` to print informatino about each path
- `printEfficiencyStats()` to print statistics about the efficiency of the network
