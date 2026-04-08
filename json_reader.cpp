#include "json_reader.h"
#include "domain.h"
#include "geo.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
 namespace transport_catalogue {
void JsonReader::MakeBaseRequests(json::Node nodes){
    if(nodes.IsArray()){
        for(const json::Node& node : nodes.AsArray()){
            if(node.IsMap()){
                const json::Dict dict = node.AsMap();
                auto iter = dict.find("type");
                if( iter != dict.end()){
                    if(iter->second.IsString()){
                        if(iter->second.AsString() == "Stop"){
                            domain::Stop stop = ParseStop(dict);
                            tc_->AddBusStop(stop);
                        }
                    }
                }
            }
        }
    }

    if(nodes.IsArray()){
        for(const json::Node& node : nodes.AsArray()){
            if(node.IsMap()){
                const json::Dict dict = node.AsMap();
                auto iter = dict.find("type");
                if( iter != dict.end()){
                    if(iter->second.IsString()){
                        if(iter->second.AsString() == "Stop"){
                            auto iter_distance = dict.find("road_distances");
                            if( iter_distance != dict.end()){
                                if(iter_distance->second.IsMap()){
                                    auto iter_name = dict.find("name");
                                    std::string stop1_name = iter_name->second.AsString();
                                    const json::Dict distances = iter_distance->second.AsMap();
                                    for(const auto &[key,value]: distances){
                                        tc_->AddStopDistance(stop1_name,key,value.AsDouble());
                                    }

                                }
                            }
                        }
                        if(iter->second.AsString() == "Bus"){
                            domain::Bus bus = ParseBus(dict);
                            tc_->AddBusRoute(bus);
                        }
                    }
                }
            }
        }
    }

}

void JsonReader::MakeStatRequests(json::Node node)
{

}



domain::Stop JsonReader::ParseStop(const json::Dict& dict){
    std::string name="";
    double lat,lon;
    for(const auto &[key,value]: dict){
        if(key == "name"){
            if(value.IsString()){
                name = value.AsString();
            }
        }
        if(key == "latitude"){
            if(value.IsDouble()){
                lat = value.AsDouble();
            }
        }
        if(key == "longitude"){
            if(value.IsDouble()){
                lon = value.AsDouble();
            }
        }
    }
    return Stop{name,geo::Coordinates{lat,lon},{} };
}

Bus JsonReader::ParseBus(const json::Dict &dict){
    std::string name="";
    bool is_roundtrip=false;
    std::vector<std::string_view> stops ={};
    for(const auto &[key,value]: dict){
        if(key == "name"){
            if(value.IsString()){
                name = value.AsString();
            }
        }
        if(key == "is_roundtrip"){
            if(value.IsBool()){
                is_roundtrip = value.AsBool();
            }
        }

        if(key == "stops"){
            if(value.IsArray()){
                for(const auto &stop_name: value.AsArray() ){
                    if(stop_name.IsString()){
                        std::cout << stop_name.AsString() << std::endl;
                    }
                }
            }
        }


    }


    return {name,stops,is_roundtrip};
}


} //end namespace transport_catalogue
