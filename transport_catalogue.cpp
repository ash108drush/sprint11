#include "transport_catalogue.h"

#include <iostream>
#include<algorithm>
#include <cassert>
#include "geo.h"
using namespace geo;
namespace transport_catalogue {
namespace main {

void TransportCatalogue::AddBusRoute(Bus bus){
    Bus& bus_ref = buses_.emplace_back(std::move(bus));
    busname_to_bus_.insert({bus_ref.name, &bus_ref});
    for(auto iter = bus_ref.stops.begin(); iter != bus_ref.stops.end(); ++iter){
        stopname_to_bus_list_[(*iter)].insert(bus_ref.name);
    }
}

void TransportCatalogue::AddBusStop(Stop stop){
    Stop& stop_ref = stops_.emplace_back(std::move(stop));
    stopname_to_stop_.insert({stop_ref.name, &stop_ref});
}

void TransportCatalogue::AddStopDistance(std::string_view stop_name1,std::string_view stop_name2, double distance){
    const Stop* stop1 = FindBusStopByName(stop_name1);
    const Stop* stop2 = FindBusStopByName(stop_name2);
    if(stop1 != nullptr && stop2 != nullptr){
        real_distance_.insert({{stop1,stop2},distance});
    }
}

const Bus* TransportCatalogue::FindRouteByName(std::string_view name) const{
    auto iter  = busname_to_bus_.find(name);
    if(iter != busname_to_bus_.end()){
        return iter->second;
    }
    return nullptr;
}

const Stop* TransportCatalogue::FindBusStopByName(std::string_view name) const {
    auto iter  = stopname_to_stop_.find(name);
    if(iter != stopname_to_stop_.end()){
        return iter->second;
    }
    return nullptr;
}

const std::set<std::string_view> TransportCatalogue::GetBusesByStopName(std::string_view name) const {
    std::set<std::string_view> stops={};
    auto iter = stopname_to_bus_list_.find(name);
    if(iter != stopname_to_bus_list_.end()){
        stops = (iter->second);
    }
    return stops;

}

const std::map<std::string_view, Bus> TransportCatalogue::GetAllBus() const {
    std::map<std::string_view, Bus> all_buses;
    for(const Bus & bus :buses_){
        all_buses.insert({bus.name,bus});
    }
    return all_buses;
}
/*
std::optional<BusStat> TransportCatalogue::GetBusStat(std::string_view name) const {
    const Bus* bus = FindRouteByName(name);
    if(bus == nullptr){
        return std::nullopt;
    }
    int stops_on_route = bus->stops.size();
    std::unordered_set<std::string_view> bus_set;
    const Stop * stop1 = nullptr;
    const Stop * stop2 = nullptr;
    bool flagfirst= true;
    double route_distance=0;
    double geo_route_distance=0;
    double stop_distance=0;
    for(std::string_view stop_name : bus->stops){
        bus_set.insert(stop_name);
        stop1 = stop2;
        stop2 = FindBusStopByName(stop_name);
        if(flagfirst){
            flagfirst = false;
            continue;
        }
        if(stop1 != nullptr && stop2 != nullptr){
            std::optional<double> real_dist = RealDistanceCalculator(stop1,stop2);
            if(real_dist.has_value()){
                stop_distance = real_dist.value();
            }else{
                stop_distance = geo::ComputeDistance(stop1->coordinates, stop2->coordinates);
            }

            geo_route_distance += geo::ComputeDistance(stop1->coordinates, stop2->coordinates);
            route_distance += stop_distance;
        }else{
            if(stop1 == nullptr){ assert("stop1 nullptr");}
            if(stop2 == nullptr){ assert("stop2 nullptr");}
        }
    }
    int uniq_stops = bus_set.size();
    double curve = route_distance / geo_route_distance;
    return BusStat{stops_on_route,uniq_stops,route_distance, curve};
}

*/
double TransportCatalogue::GetDistance(std::pair<Stop *, Stop *> point) {
    return geo::ComputeDistance(point.first->coordinates,point.second->coordinates);
}

std::optional<double> TransportCatalogue::RealDistanceCalculator(const Stop* stop1, const Stop* stop2) const {
    auto iter = real_distance_.find({stop1,stop2});
    if(iter == real_distance_.end()){
        iter = real_distance_.find({stop2,stop1});
    }
    if(iter == real_distance_.end()){
        return std::nullopt;
    }
    return iter->second;
}
/*
std::optional<std::set<std::string_view>> TransportCatalogue::GetStopInfo(std::string_view name) const {
    const Stop* stop = FindBusStopByName(name);
    if(stop == nullptr){
        return std::nullopt;
    }
    std::set<std::string_view> stops={};
    auto iter = stopname_to_bus_list_.find(name);
    if(iter != stopname_to_bus_list_.end()){
        stops = (iter->second);
    }
    return stops;
}
*/

}
} // end namespace
