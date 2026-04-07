#include "json_reader.h"
#include "domain.h"
#include "geo.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
 namespace transport_catalogue {
void JsonReader::MakeBaseRequests(json::Node nodes)
{
    if(nodes.IsArray()){
        for(const json::Node& node : nodes.AsArray()){
            if(node.IsMap()){
                json::Dict dict = node.AsMap();
                 if(dict["type"] == "Stop"){
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
                    } //end for cycle
                    domain_->AddStop({name,geo::Coordinates{lat,lon} });

                    }
                }
            }// end first read

            for(const json::Node& node : nodes.AsArray()){
                if(node.IsMap()){
                    for(const auto &[key,value] : node.AsMap()){
                    if(key == "type"){
                        if(value.AsString()=="Bus"){
                            std::string name="";
                            bool is_roundtrip=false;
                            std::vector<std::string_view> stops ={};
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
                                    for(const auto & str :value.AsArray() ){
                                        std::cout << str.AsString() << std::endl;

                                }
                                }else{ std::cout << "smth wrong";}
                        } //end for cycle


                    }
                        domain_->AddBus({name,stops,is_roundtrip} );
                    }


        }// end second read
    }
}

void JsonReader::MakeStatRequests(json::Node node)
{

}
} //end namespace transport_catalogue
