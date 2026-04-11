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
                            db_.AddBusStop(stop);
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
                                        db_.AddStopDistance(stop1_name,key,value.AsDouble());
                                    }

                                }
                            }
                        }
                        if(iter->second.AsString() == "Bus"){
                            domain::Bus bus = ParseBus(dict);
                            db_.AddBusRoute(bus);
                        }
                    }
                }
            }
        }
    }

}

void JsonReader::MakeStatRequests(json::Node nodes){
      if(nodes.IsArray()){
          for(const json::Node& node : nodes.AsArray()){
              if(node.IsMap()){
                  const json::Dict dict = node.AsMap();
                  auto iter = dict.find("type");
                  if(!iter->second.IsString()) return;
                  std::string type = iter->second.AsString();
                  if(type == "Stop"){
                      MakeStopStatRequest(dict);
                  }
                  if(type == "Bus"){
                      MakeBusStatRequest(dict);
                  }
              }
          }
         PrintStatRequests();

      }
}

void JsonReader::MakeStopStatRequest(const json::Dict &dict){
    std::string name="";
    int id = 0;
    for(const auto &[key,value]: dict){
        if(key == "name"){
            name = value.AsString();
        }
        if(key == "id"){
            id = value.AsInt();
        }
    }

    std::optional<std::set<std::string_view>> stop_info = rh_.GetStopInfo(name);
    if(stop_info.has_value()){
        Request new_request{id,stop_info.value()};
        stat_.push_back(new_request);
    }else{
        stat_.emplace_back(Request{id,nullptr});
    }
}

void JsonReader::MakeBusStatRequest(const json::Dict &dict){
    std::string name="";
    int id = 0;
    for(const auto &[key,value]: dict){
        if(key == "name"){
            name = value.AsString();
        }
        if(key == "id"){
            id = value.AsInt();
        }
    }
    std::optional<BusStat> bus_stat = rh_.GetBusStat(name);
    if(bus_stat.has_value()){
        Request new_request{id,bus_stat.value()};
        stat_.push_back(new_request);
    }else{
        stat_.push_back(Request{id,nullptr});
    }
}


struct VariantPrinter {
    std::ostream& out;
    int key;
    void operator()(std::nullptr_t) const {
         out << "\"request_id\": " <<  key  << ","<< std::endl;
        out << "\"error_message\": \"not found\"" << std::endl;
    }

    void operator()(BusStat bus_stat) const {
        out << "\"curvature\": " << bus_stat.curvature << ","<< std::endl;
        out << "\"request_id\": " <<  key  << ","<< std::endl;
        out << "\"route_length\": " << bus_stat.route_length <<"," << std::endl;
        out << "\"stop_count\": " << bus_stat.stops_on_route << ","<< std::endl;
        out << "\"unique_stop_count\": " << bus_stat.uniq_stops_on_route << std::endl;
    }
    void operator()(std::set<std::string_view> stop_info) const {
        if(stop_info.size() >0){
            out << "\"buses\": ";
            out << "[" ;
            int i=0;
            for(std::string_view sv: stop_info){
                out << "\"" <<sv <<"\"";
                ++i;
                if(i < stop_info.size()){
                    out <<",";
                }
            }
            out << "], " << std::endl;
            out << "\"request_id\": " <<  key  << std::endl;
        }else{

            out << "\"buses\": []," << std::endl;
            out << "\"request_id\": " <<  key  << std::endl;
        }


    }
};

void JsonReader::PrintStatRequests(){
    out_ << "["<< std::endl;
    if(stat_.size() >0){
        int i=0;
        for(auto &request : stat_){
            out_ << "{" << std::endl;
            std::visit(VariantPrinter{out_ ,request.id}, request.info);

            ++i;
            if(i < stat_.size()){
                out_ <<"}," << std::endl;
            }else{
                out_ << "}" << std::endl;
            }

        }
    }
    out_ <<"]"<< std::endl;


}



domain::Stop JsonReader::ParseStop(const json::Dict& dict){
    std::string name="";
    double lat=0,lon=0;
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
    std::vector<std::string_view>  stops;
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
                      //  std::cout << stop_name.AsString() << std::endl;
                        stops.push_back(db_.FindBusStopByName( stop_name.AsString())->name);

                    }
                }
            }
        }


    }


    return {name,stops,is_roundtrip};
}


} //end namespace transport_catalogue
