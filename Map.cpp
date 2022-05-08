//  Copyright 2022 Peter Aisher
//
//  Map.cpp
//  NetGen
//

#include "Map.h"

#include <queue>
#include <utility>
#include <limits>
#include <string>

std::pair<float, std::vector<int>>
Map::shortestRouteUsingCapacity(int sourceIndex, CargoType need,
                                float quantity) {
  std::priority_queue<std::pair<float, int>> frontier;
  frontier.push({0.0f, sourceIndex});
  std::vector<bool> visited;
  visited.resize(triangulation.vertices.size());
  std::vector<int> previous;
  previous.resize(triangulation.vertices.size());
  std::fill(previous.begin(), previous.end(), -1);

  std::vector<float> distance;
  distance.resize(triangulation.vertices.size());
  std::fill(distance.begin(), distance.end(),
            std::numeric_limits<float>::infinity());

  distance[sourceIndex] = 0.f;

  WagonType t  = cargoInfo.wagonTypeForCargo(need);

  while (!frontier.empty()) {
    auto u_pair = frontier.top();
    frontier.pop();
    float dist_u = u_pair.first;
    int u = u_pair.second;
    if (indexIsIndustry(u)) {
      if (industries[u].outputType() == need) {
        float remaining_capacity = network.nodes[u];
        if (remaining_capacity >= quantity) {
          std::vector<int> path {u};
          while (previous[u] >= 0) {
            u = previous[u];
            path.push_back(u);
          }
          return {dist_u, path};
        }   // if (remaning_capacity < capacity)
      }   // if (industries[u].outputType() == need)
    }   // if (indexIsIndustry(u))
    visited[u] = true;
    for (const auto& v_gr : network.storage[u]) {
      const int v = v_gr.first;
      if (visited[v]) {
        continue;
      }
      float outbound_flow = v_gr.second.second[t];
      float inbound_flow = network.storage[v][u].second[t];
      float available_capacity = inbound_flow - outbound_flow;

      float effective_quantity = quantity;
      if (available_capacity > 0) {
        if (available_capacity >= quantity) {
          effective_quantity = 0;
        } else {
          effective_quantity -= available_capacity;
        }
      }
      float cost = v_gr.second.first * effective_quantity;

      float alt = dist_u + cost;
      if (alt < distance[v]) {
        distance[v] = alt;
        previous[v] = u;
        frontier.emplace(alt, v);
      }
    }
  }   // while
  return {};  // zero cost and empty path
}


void Map::triangulateAllLocations() {
  std::vector<Point2D> allLocations {};
  for (auto & industry : industries) {
    allLocations.push_back(industry.location());
  }
  for (auto & town : towns) {
    allLocations.push_back(town.location());
  }
  triangulation = IndexedDelaunay(allLocations);
  triangulation.maskSliverTrianglesOnBoundary(0.15);
}

void Map::buildNetworkGraph() {
  network = CargoGraph(triangulation);
  for (const auto& edge : network.allEdges()) {
    LineSegment l = {triangulation.vertices[edge.a],
                     triangulation.vertices[edge.b]};
    for (const auto& line : impassableLines) {
      if (l.intersectsLine(line)) {
        network.removeEdge(edge);
      }
    }
  }
  for (int i = 0; i < industries.size(); ++i) {
    network.nodes[i] = industries[i].maxOutput();
  }
}

void Map::setUniformTownCargoRequirement(float town_cargo_need) {
  for (int i = 0; i < towns.size(); ++i) {
    int id = static_cast<int>(industryCount()) + i;
    for (auto req : towns[i].cargoRequired()) {
      connectionsToMake[{id, req}] = town_cargo_need;
    }
  }
}

Map::ConnectionInformation Map::findCheapestOutstandingConnection() {
  ConnectionInformation result {std::numeric_limits<float>::infinity()};
  for (auto &p : connectionsToMake) {
    int id = p.first.first;
    CargoType need = p.first.second;
    float quantity = p.second;
    if (quantity == 0.f) {
      continue;
    }
    float industry_max_production = 100.f;
    if (quantity > industry_max_production) {
      quantity = industry_max_production;
    }
    auto path = shortestRouteUsingCapacity(id, need, quantity);
    if (path.first < result.cost) {
      result.cost = path.first;
      result.path = path.second;
      result.cargoType = need;
      result.quantity = quantity;
    }
  }
  return result;
}

void Map::removeConnectionFromOutstanding(const ConnectionInformation& info) {
  int id = info.consumer();
  CargoType need = info.cargoType;

  connectionsToMake[{id, need}] -= info.quantity;

  if (connectionsToMake[{id, need}] == 0) {
    connectionsToMake.erase({id, need});
  }
}

void Map::addPathOrInreaseCapacity(const ConnectionInformation& info) {
  for (auto& p : all_paths) {
    if (p.cargoType == info.cargoType
        && p.consumer() == info.consumer()
        && p.supplier() == info.supplier()
        && p.path == info.path) {
      p.quantity += info.quantity;
      p.cost += info.cost;
      return;
    }
  }
  all_paths.push_back(info);
}

void Map::registerFlowsInNetwork(
        const ConnectionInformation &info) {
  CargoType _need = info.cargoType;
  WagonType t = cargoInfo.wagonTypeForCargo(_need);
  for (int i = 0, j = 1; j < info.path.size(); ++i, ++j) {
    int u = info.path[i];
    int v = info.path[j];
    // path is from supplier to consumer, so add flows in reverse direction
    network.storage[v][u].second[t] += info.quantity;
  }
  network.nodes[info.supplier()] -= info.quantity;
  addPathOrInreaseCapacity(info);
}

void Map::addUpstreamIndustryChainToOutstanding(
          const ConnectionInformation &path) {
  auto requirements = supplyChainInfo
    .requirementsForIndustryProducing(path.cargoType);
  for (auto& requirement : requirements) {
    float required_amount = path.quantity * requirement.quantity;
    connectionsToMake[{path.supplier(),
                       requirement.cargoType}] += required_amount;
  }
}

void Map::printAllEdges(std::ostream& out) {
  for (auto & edge : network.allEdges()) {
    out << "(" << edge.a << ", " << edge.b << ")" << std::endl;
  }
}

void Map::printAllPaths(std::ostream& out) {
  out << all_paths.size() << " connections\n"
    << "cargo_id\twagon_id\tquantity\tcost\tpath" << std::endl;
  for (auto& p : all_paths) {
    out << p.cargoType << "\t" <<
      cargoInfo.wagonTypeForCargo(p.cargoType) << "\t" << p.quantity <<
      "\t" << p.cost << "\t[ ";
    for (auto& node : p.path) {
      out << node << " ";
    }
    out << "]" << std::endl;
  }
}
void Map::printIndustryInfo(std::ostream & out) {
  out << industryCount() << " industries\n"
    << "node_id\tname\tx_coord\ty_coord" << std::endl;
  for (int i = 0; i < industryCount(); ++i) {
    Industry& industry = industries[i];
    std::string name =
      IndustryInfo::nameOfIndustryProducing(industry.outputType());
    out << i << "\t" << name << "\t" << industry.location().x << "\t" <<
      industry.location().y << std::endl;
  }
}

void Map::printTownInfo(std::ostream & out) {
  out << towns.size() << " towns\n"
    << "node_id\tname\tx_coord\ty_coord" << std::endl;
  for (int i = 0; i < towns.size(); ++i) {
    Town& town = towns[i];
    const std::string& name = town.name();
    out << (i + industryCount()) << "\t" << name << "\t" <<
      town.location().x << "\t" << town.location().y << std::endl;
  }
}

void Map::makeAllConnections() {
  while (!connectionsToMake.empty()) {
    ConnectionInformation costliestPath =  findCheapestOutstandingConnection();
    removeConnectionFromOutstanding(costliestPath);
    registerFlowsInNetwork(costliestPath);
    addUpstreamIndustryChainToOutstanding(costliestPath);
  }
}

float Map::calculatePathLength(const std::vector<int> &path) {
  float pathLength = 0.f;
  for (int i = 0, j = 1; j < path.size(); ++i, ++j) {
    pathLength += network.storage[path[i]][path[j]].first;
  }
  return pathLength;
}

void Map::printEfficiencyStats(std::ostream& out) {
  float totalNaiveCost = 0.f;
  float totalCost = 0.f;
  float totalQuantity = 0.f;
  for (const auto& c_info : all_paths) {
    float pathLength = calculatePathLength(c_info.path);
    totalNaiveCost += pathLength * c_info.quantity;
    totalCost += c_info.cost;
    totalQuantity += c_info.quantity;
  }
  float saving = (totalNaiveCost - totalCost) / totalNaiveCost;
  out << "Efficiency statistics:\n";
  out << "total cargo volume:  " << totalQuantity << "\n";
  out << "naive total cost:    " << totalNaiveCost << "\n";
  out << "actual total cost:   " << totalCost << "\n";
  out << "cost saving          " << (saving * 100) << "%\n";
}
