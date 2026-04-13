#include "json_reader.h"
#include "domain.h"
#include "map_renderer.h"
#include "geo.h"
#include <iomanip>
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
    std::optional<main::BusStat> bus_stat = rh_.GetBusStat(name);
    if(bus_stat.has_value()){
        Request new_request{id,bus_stat.value()};
        stat_.push_back(new_request);
    }else{
        stat_.push_back(Request{id,nullptr});
    }
}

std::string JsonReader::ArrayToString(json::Array node) {
    std::string color = "none";
    std::vector<std::string> colors;
    for(const json::Node & node_str : node){
        if(node_str.IsInt()){
            colors.push_back(std::to_string(node_str.AsInt()));
        }else if(node_str.IsDouble()){
            std::stringstream ss;
            ss << std::setprecision(6);
            ss << node_str.AsDouble();
            colors.push_back(ss.str());
        }
    }
    if(colors.size() == 3){
        color="rgb(";
    }else if(colors.size() == 4){
        color="rgba(";
    }
    for(auto iter  = colors.begin(); iter != colors.end(); ++iter){
        color += *iter;
        if(iter != colors.end()-1){
            color +=",";
        }
    }
    color += ")";
    return color;
}
void JsonReader::SetRenderSettings(json::Node nodes){
    RenderSettings render_settings = RenderSettings();
    if(nodes.IsMap()){
        for(const auto &[key,value]: nodes.AsMap()){
            if(key == "width" ){
                render_settings.width = value.AsDouble();
            }
            if(key == "height" ){
                render_settings.height = value.AsDouble();
            }
            if(key == "padding" ){
                render_settings.padding = value.AsDouble();
            }
            if(key == "padding" ){
                render_settings.padding = value.AsDouble();
            }
            if(key == "stop_radius" ){
                render_settings.stop_radius = value.AsDouble();
            }
            if(key == "line_width" ){
                render_settings.line_width = value.AsDouble();
            }
            if(key == "bus_label_font_size" ){
                render_settings.bus_label_font_size = value.AsInt();
            }
            if(key == "bus_label_offset" ){
                if(value.IsArray()){
                    render_settings.bus_label_offset = {value.AsArray()[0].AsDouble(),value.AsArray()[1].AsDouble()};
                }
            }
            if(key == "stop_label_font_size" ){
                render_settings.stop_label_font_size = value.AsInt();
            }
            if(key == "stop_label_offset" ){
                if(value.IsArray()){
                    render_settings.stop_label_offset = {value.AsArray()[0].AsDouble(),value.AsArray()[1].AsDouble()};
                }
            }
            if(key == "underlayer_color" ){
                if(value.IsArray()){
                    render_settings.underlayer_color = ArrayToString(value.AsArray());
                }
                if(value.IsString()){
                    render_settings.underlayer_color = value.AsString();
                }
            }
            if(key == "underlayer_width" ){
                render_settings.underlayer_width = value.AsDouble();
            }

            if(key == "color_palette" ){
                if(value.IsArray()){
                    std::vector<std::string> colors;
                    for(const json::Node & node_str : value.AsArray()){
                        if(node_str.IsString()){
                            colors.push_back(node_str.AsString());
                        }else{
                            colors.push_back(ArrayToString(node_str.AsArray()));
                        }
                    }
                    render_settings.color_palette = colors;
                }
            }
        }
    }

    map_renderer_.SetRenderSettings(render_settings);
    rh_.DrawBusRoute(map_renderer_);

}


struct VariantPrinter {
    std::ostream& out;
    int key;
    void operator()(std::nullptr_t) const {
         out << "\"request_id\": " <<  key  << ","<< std::endl;
        out << "\"error_message\": \"not found\"" << std::endl;
    }

    void operator()(main::BusStat bus_stat) const {
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
            size_t i=0;
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

    if(stat_.size() >0){
        out_ << "["<< std::endl;
        size_t i=0;
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
        out_ <<"]"<< std::endl;
    }

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
                        const Stop* stop = db_.FindBusStopByName( stop_name.AsString());
                        if(stop != nullptr){
                          stops.push_back(stop->name);
                        }

                    }
                }
            }
        }


    }


    return {name,stops,is_roundtrip};
}


} //end namespace transport_catalogue
