#include <iostream>

using namespace std;

//int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */
//}


#include <iostream>
#include <string>
#include <fstream>
#include "json.h"
#include "json_reader.h"
#include <sstream>
#include <memory>
#include "domain.h"
#include "request_handler.h"
#include "map_renderer.h"
using namespace transport_catalogue;
using namespace transport_catalogue::main;

int main() {

    TransportCatalogue  catalogue;
    std::ifstream in;
    std::ofstream out;
    std::ofstream outsvg;
    in.open("input.json");
    out.open("output.json");
    outsvg.open("out.svg");

    json::Document  doc = json::Load(in);
    transport_catalogue::main::TransportCatalogue tc;
    RequestHandler rh(tc);
    MapRenderer map_renderer(outsvg);
    JsonReader json_reader(doc, tc, rh,out, map_renderer);


    /*
    json::Document  doc = json::Load(std::cin);
    transport_catalogue::main::TransportCatalogue tc;
    RequestHandler rh(tc);
    JsonReader json_reader(doc, tc, rh,std::cout);
*/

}
//Задача прошла 14 из 26 проверок
//    Неуспех
