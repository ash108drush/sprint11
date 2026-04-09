#include "request_handler.h"
#include <cassert>
/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view name) const{
    const Bus* bus = db_.FindRouteByName(name);
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
    stop2 = db_.FindBusStopByName(stop_name);
    if(flagfirst){
        flagfirst = false;
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
}
int uniq_stops = bus_set.size();
double curve = route_distance / geo_route_distance;
return BusStat{stops_on_route,uniq_stops,route_distance, curve};
}

std::optional<std::set<std::string_view> > RequestHandler::GetStopInfo(std::string_view name) const {
    const Stop* stop = db_.FindBusStopByName(name);
    if(stop == nullptr){
        return std::nullopt;
    }
    return db_.GetBusesByStopName(name);
}
