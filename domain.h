#pragma once
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <string_view>
#include <vector>
#include <string>
#include "geo.h"
/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например, Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
namespace transport_catalogue {
namespace domain {
struct Bus {
    std::string name;
    std::vector<std::string_view> stops ={};
    bool is_roundtrip;
};


struct Stop{
    std::string name;
    geo::Coordinates coordinates;
    std::unordered_map<std::string_view,double> road_distances = {};
};

struct BusRoute {
    std::vector<const Stop *> stops ={};
    bool is_roundtrip;
    const Stop * last_stop =nullptr;
};




class Domain{
public:
    void AddBus(Bus bus){
        buses_.emplace_back(bus);
    }
    void AddStop(Stop stop){
        stops_.emplace_back(stop);
    }
private:
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
};

} //end namespace domain
} //end namespace transport_catalogue
