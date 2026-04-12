#pragma once
#include "json.h"
#include <iostream>
#include <string>
#include <fstream>
#include "json.h"
#include "json_reader.h"
#include "request_handler.h"
#include <sstream>
#include <cassert>
#include "map_renderer.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
namespace transport_catalogue {
class JsonReader{
public:
    JsonReader(const json::Document& document,main::TransportCatalogue& db,
               const RequestHandler& rh,MapRenderer& map_renderer, std::ostream& out):
        document_(document),db_(db),rh_(rh), map_renderer_(map_renderer),out_(out){
       auto root_node = document_.GetRoot();

        if(root_node.IsMap()){

           for (const auto&[ str, node] : root_node.AsMap()){
                //std::cout << str << std::endl;

               if(str == "base_requests"){
                    MakeBaseRequests(node);
               }
               if(str == "stat_requests"){
                   // MakeStatRequests(node);
               }
               if(str == "render_settings"){
                   SetRenderSettings(node);
               }


           }
        }else{
        assert(false && "root node not map");
        }
    }
    using stat_info = std::variant<std::nullptr_t,main::BusStat,std::set<std::string_view>>;

private:
    struct Request{
        int id;
        stat_info info;

    };
    std::string ArrayToString(json::Array node);

    void MakeBaseRequests(json::Node node);
    domain::Stop ParseStop(const json::Dict& dict);
    domain::Bus ParseBus(const json::Dict& dict);
    void MakeStatRequests(json::Node node);
    void MakeStopStatRequest(const json::Dict& dict);
    void MakeBusStatRequest(const json::Dict& dict);
    void SetRenderSettings(json::Node node);
    void PrintStatRequests();
    json::Document document_;
    main::TransportCatalogue& db_;
    const RequestHandler& rh_;
    MapRenderer& map_renderer_;

    std::vector<Request> stat_;

    std::ostream& out_;

};
}
