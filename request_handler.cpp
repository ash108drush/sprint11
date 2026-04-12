#include "request_handler.h"
#include <cassert>
#include <iostream>
/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
using namespace transport_catalogue;
std::optional<main::BusStat> RequestHandler::GetBusStat(const std::string_view name) const{
    const Bus* bus = db_.FindRouteByName(name);
if(bus == nullptr){
    return std::nullopt;
}

int stops_on_route = 0;
std::unordered_set<std::string_view> bus_set;
const Stop * stop1 = nullptr;
const Stop * stop2 = nullptr;
bool flagfirst= true;
double route_distance=0;
double geo_route_distance=0;
double stop_distance=0;
for(auto iter = bus->stops.begin(); iter != bus->stops.end(); ++iter){
    bus_set.insert(*iter);
    stop1 = stop2;
    stop2 = db_.FindBusStopByName(*iter);
    if(flagfirst){
        flagfirst = false;
        ++stops_on_route;
        continue;
    }
    if(stop1 != nullptr && stop2 != nullptr){
        std::optional<double> real_dist = db_.RealDistanceCalculator(stop1,stop2);
        if(real_dist.has_value()){
            stop_distance = real_dist.value();
        }else{
            stop_distance = geo::ComputeDistance(stop1->coordinates, stop2->coordinates);
        }

        geo_route_distance += geo::ComputeDistance(stop1->coordinates, stop2->coordinates);
        route_distance += stop_distance;
    }else{
        if(stop1 == nullptr){ assert(false && "stop1 nullptr");}
        if(stop2 == nullptr){ assert(false && "stop2 nullptr");}
    }
    ++stops_on_route;
}

if(!bus->is_roundtrip){
    for(auto iter = bus->stops.rbegin()+1; iter != bus->stops.rend(); ++iter){
        stop1 = stop2;
        stop2 = db_.FindBusStopByName(*iter);
        if(stop1 != nullptr && stop2 != nullptr){
            std::optional<double> real_dist = db_.RealDistanceCalculator(stop1,stop2);
            if(real_dist.has_value()){
                stop_distance = real_dist.value();
            }else{
                stop_distance = geo::ComputeDistance(stop1->coordinates, stop2->coordinates);
            }

            geo_route_distance += geo::ComputeDistance(stop1->coordinates, stop2->coordinates);
            route_distance += stop_distance;
        }
        ++stops_on_route;
    }
}
int uniq_stops = bus_set.size();
double curve = route_distance / geo_route_distance;
return main::BusStat{stops_on_route,uniq_stops,route_distance, curve};
}

std::optional<std::set<std::string_view> > RequestHandler::GetStopInfo(std::string_view name) const {
    const Stop* stop = db_.FindBusStopByName(name);

    if(stop == nullptr){
        return std::nullopt;
    }
    return db_.GetBusesByStopName(name);
}

const std::map<std::string_view, Bus> RequestHandler::GetAllBus() const
{
    return db_.GetAllBus();
}

void RequestHandler::DrawBusRoute(MapRenderer& map_renderer_) const {
    const std::map<std::string_view, Bus> all_buses = GetAllBus();
    const Stop * stop = nullptr;
    for(const auto &[name,bus] : all_buses){
        std::vector<const Stop *> round;
        for(auto iter = bus.stops.begin(); iter != bus.stops.end(); ++iter){
            stop = db_.FindBusStopByName(*iter);
            if(stop != nullptr){
               map_renderer_.AddRoute(name,stop);
                round.push_back(stop);
            }
        }
        if(!bus.is_roundtrip){
            for(auto iter = round.rbegin()+1; iter != round.rend();++iter){
                map_renderer_.AddRoute(name,*iter);
            }
        }
    }



    map_renderer_.RenderMap();

   // return main::BusStat{stops_on_route,uniq_stops,route_distance, curve};
}
