//  Copyright 2022 Peter Aisher
//
//  Map.h
//  NetGen
//

#ifndef NETGEN_MAP_H_
#define NETGEN_MAP_H_

#include <array>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <utility>
#include "routing/indexed_delaunay.h"
#include "Graph.h"
#include "data/cargo_type.h"
#include "data/wagon_type.h"
#include "data/industry.h"
#include "data/town.h"
#include "data/cargo_information.h"
#include "data/supply_chain_information.h"

typedef IntGraph<std::array<float, WagonTypeCount>, float, float> CargoGraph;


/// Represents a game map
///
/// Contains all information about towns and industries,
/// as well as routing connections.
class Map {
  /* types for indexed cargo routing */
  typedef std::pair<int, CargoType> NodeAndNeed;
  struct NodeAndNeed_hash {
      std::size_t operator() (const NodeAndNeed &p) const {
          return std::hash<int>()(p.first) ^ std::hash<CargoType>()(p.second);
      }
  };
  struct ConnectionInformation;

  /* storage of user information */
  std::vector<Industry> industries;
  std::vector<Town> towns;
  std::vector<Line2D> impassableLines;

  /* storage of derived network data */
  IndexedDelaunay triangulation;
  CargoGraph network;

  /* information for supply chain routing */
  CargoInformation cargoInfo;
  SupplyChainInformation supplyChainInfo;

  /* variables for route finding */
  std::unordered_map<NodeAndNeed, float, NodeAndNeed_hash> connectionsToMake;
  std::vector<ConnectionInformation> all_paths;

  /* methods for indexed cargo routing */

  /// The number of industries
  inline size_t industryCount() const {return industries.size();}

  /// Does index belong to an industry
  /// @param i index to check
  ///
  /// @note indices start from zero, and can apply to towns or industries
  inline bool indexIsIndustry(int i) const {return i < industryCount();}

  /// shortest route using existing empty capacity
  /// @param sourceIndex the index of the consumer
  /// @param need the cargo type needed
  /// @param quantity the quantity needed
  std::pair<float, std::vector<int>>
  shortestRouteUsingCapacity(int sourceIndex, CargoType need, float quantity);

  /// the lowest-cost connection still to be made
  ConnectionInformation findCheapestOutstandingConnection();

  /// remove connection from list of connections to be made
  /// @param info the information for the connection to be removed
  void removeConnectionFromOutstanding(const ConnectionInformation& info);

  /// register flows in network
  /// @param info the information for the connection to be registered
  ///
  /// Registers the quantity of the relevant wagon type for each edge travelled.
  /// This means empty return journey capacity can be used by subsewuent routes
  void registerFlowsInNetwork(const ConnectionInformation &info);

  /// Add direct upstream industry chain info to list of outstanding connections
  /// @param info the information of the connection just made
  ///
  /// Adds the direct requirements (if any) of the supplying industry
  /// to the list of connections to make
  void addUpstreamIndustryChainToOutstanding(const ConnectionInformation &info);

  /// calculates the length of a path
  /// @param path the path whose length should be calculated
  float calculatePathLength(const std::vector<int>& path);

  /// Add a path or increase the quantity of identical registered path
  /// @param info the information of the path to add or increase
  void addPathOrInreaseCapacity(const ConnectionInformation & info);

 public:
  inline Map() {}

  /* methods for constructing the map */

  /// Add industry
  /// @param industry add this industry to the map
  inline void addIndustry(Industry industry) {industries.push_back(industry);}

  /// Add town
  /// @param town add this town to the map
  inline void addTown(Town town) {towns.push_back(town);}

  /// Add impassable line
  /// @param line add this impassable line to the map
  ///
  /// @note connections which cross impassable lines will not be present
  /// in the final generated network
  inline void addImpassableLine(Line2D line) {impassableLines.push_back(line);}

  /* methods for making network connections*/

  /// make a delaunay triangulation of all towns and industries
  void triangulateAllLocations();
  /// build the network graph from the triangulation
  void buildNetworkGraph();
  /// set uniform cargo requirements for all needs of each town
  /// @param town_cargo_need the amount of each cargo type needed by each town
  void setUniformTownCargoRequirement(float town_cargo_need);
  /// make all supply connections to supply towns with required cargo
  /// and all supply chains needed.
  void makeAllConnections();

  /* methods for reporting network structure */
  void printAllEdges(std::ostream& out = std::cout);
  void printAllPaths(std::ostream& out = std::cout);
  void printIndustryInfo(std::ostream& out = std::cout);
  void printTownInfo(std::ostream& out = std::cout);
  void printEfficiencyStats(std::ostream& out = std::cout);
};

struct Map::ConnectionInformation {
  float cost;
  float quantity;
  std::vector<int> path;
  CargoType cargoType;
  inline int supplier() const {return path.front();}
  inline int consumer() const {return path.back();}
};

#endif  // NETGEN_MAP_H_
