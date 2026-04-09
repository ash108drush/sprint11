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
#include "input_reader.h"
#include "stat_reader.h"
#include <fstream>
#include "json.h"
#include "json_reader.h"
#include <sstream>
#include <memory>
#include "domain.h"
using namespace transport_catalogue::main;
using namespace transport_catalogue::reader::detail;
using namespace transport_catalogue::output::detail;

int main(int argc, char** argv) {

    TransportCatalogue  catalogue;
    std::ifstream in;
    std::ofstream out;
    in.open("input.json");

    std::stringstream strm;

   // std::cout << strm.str();
    json::Document  doc = json::Load(in);
    transport_catalogue::main::TransportCatalogue tc;
    auto tc_ptr = std::make_unique<transport_catalogue::main::TransportCatalogue>(tc);
    transport_catalogue::JsonReader json_reader(doc,std::move(tc_ptr)); // =  new JsonReader(&doc);
    //json::Print(doc, strm);
    // std::cout << strm.str();
    //const auto doc = json::Load(strm);

    // const auto doc = json::Load("[\"array\":[1, 2, 3],\"bool\":true,\"double\":42.1,\"int\":42,\"map\":{\"key\":\"value\"},\"null\":null,\"string\":\"hello\"}]");
   // json::Print(doc, strm);
   // std::cout << strm.str();

    /*
    if(argc == 1){
        ReadInput(catalogue, std::cin);
        StatReader(catalogue, std::cin, std::cout);
    }

    if(argc > 1) {
        std::ifstream in;
        std::ofstream out;
        std::string outfile = "out.txt";
        in.open(argv[1]);
        if(!in.is_open()){
            std::cerr << "failed open read file "  << argv[1] << std::endl;
            std::abort();
        }
        if(argc == 3){
            outfile = argv[2];
        }
        out.open(outfile);
        if(!out.is_open()){
            std::cerr << "failed open write file "  << outfile << std::endl;
            std::abort();
        }
        ReadInput(catalogue, in);
        StatReader(catalogue, in, out);
        out.close();
        in.close();
    }

*/
}
