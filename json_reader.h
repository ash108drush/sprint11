#pragma once
#include "json.h"
#include <iostream>
#include <string>
#include "input_reader.h"
#include "stat_reader.h"
#include <fstream>
#include "json.h"
#include "json_reader.h"
#include <sstream>

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
namespace transport_catalogue {
class JsonReader{
public:
    JsonReader(json::Document& document,std::unique_ptr<transport_catalogue::domain::Domain> domain):
        document_(document),domain_(std::move(domain)){
       auto root_node = document_.GetRoot();
        if(root_node.IsMap()){
           for (const auto&[ str, node] : root_node.AsMap()){
                //std::cout << str << std::endl;
               if(str == "base_requests"){
                    MakeBaseRequests(node);
               }
               if(str == "stat_requests"){
                    MakeStatRequests(node);
               }
           }
        }
         std::stringstream strm;
       // json::Print(doc, strm);
       // std::cout << strm.str();
    }
private:
    void MakeBaseRequests(json::Node node);
    void MakeStatRequests(json::Node node);
    json::Document document_;
    std::unique_ptr<transport_catalogue::domain::Domain> domain_;
};
}
