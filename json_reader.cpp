#include "json_reader.h"
#include "domain.h"
#include "map_renderer.h"
#include "geo.h"
#include <iomanip>

 namespace transport_catalogue {
using namespace std::literals;
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
                  if(type == "Map"){
                      MakeMapStatRequest(dict);
                  }
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

    std::optional<std::set<std::string_view>> stop_info = db_.GetStopInfo(name);
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
    std::optional<main::BusStat> bus_stat = db_.GetBusStat(name);
    if(bus_stat.has_value()){
        Request new_request{id,bus_stat.value()};
        stat_.push_back(new_request);
    }else{
        stat_.push_back(Request{id,nullptr});
    }
}

void JsonReader::MakeMapStatRequest(const json::Dict &dict){
    int id = 0;
    for(const auto &[key,value]: dict){
        if(key == "id"){
            id = value.AsInt();
        }
    }

    map_renderer_ptr_->SetRenderSettings(render_settings_);
    rh_.DrawBusRoute(std::move(map_renderer_ptr_));
    std::string input_str = oss_.str();
    oss_.clear();
    oss_.str("");
    json::PrintValue(input_str, oss_);
    Request new_request{id,oss_.str()};
    stat_.push_back(new_request);
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

    render_settings_ = render_settings;
}


struct VariantPrinter {
    json::Array* arr_ptr;
    int key;
    void operator()(std::nullptr_t)  {
       // json::Dict dict = json::Dict{ std::string("request_id"), json::Node(key)};
       // json::Dict{ "error_message","not found"};
         json::Node dict_node{json::Dict{{"request_id"s, key}, {"error_message"s, "not found"}}};
         arr_ptr->push_back(dict_node);
    }

    void operator()(main::BusStat bus_stat) {
        json::Node dict_node{json::Dict{{"curvature"s, bus_stat.curvature},
                                        {"request_id"s, key},
                                        {"route_length"s, bus_stat.route_length},
                                        {"stop_count"s, bus_stat.stops_on_route},
                                        {"unique_stop_count"s, bus_stat.uniq_stops_on_route}
                                        }};
        arr_ptr->push_back(dict_node);
        /*
        out << "\"curvature\": " << bus_stat.curvature << ","<< std::endl;
        out << "\"request_id\": " <<  key  << ","<< std::endl;
        out << "\"route_length\": " << bus_stat.route_length <<"," << std::endl;
        out << "\"stop_count\": " << bus_stat.stops_on_route << ","<< std::endl;
        out << "\"unique_stop_count\": " << bus_stat.uniq_stops_on_route << std::endl;
*/
    }
    void operator()(std::set<std::string_view> stop_info)  {
        json::Node dict_node{json::Dict{{"buses"s, key}, {"request_id"s, key}}};
        arr_ptr->push_back(dict_node);
        /*
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
*/
    }

    void operator()(std::string map_str)  {
        json::Node dict_node{json::Dict{{"map"s, map_str}, {"request_id"s, key}}};
        arr_ptr->push_back(dict_node);
       // out << "\"map\": " << map_str << ","<< std::endl;
       // out << "\"request_id\": " <<  key  << std::endl;

    }
};

void JsonReader::PrintStatRequests(){
    std::ostringstream oss;
    std::istringstream input;
    json::Node root;
    json::Array arr;
    json::Array* arr_ptr= &arr;



    if(stat_.size() >0){

        for(auto &request : stat_){

            std::visit(VariantPrinter{arr_ptr, request.id}, request.info);

    }
   /// json::PrintValue(oss.str(),out_);

    }
    json::Document document = json::Document(json::Node(arr));
    json::Print(document,out_);
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
