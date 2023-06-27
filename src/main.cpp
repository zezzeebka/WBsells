#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <iostream>
#include "wbback.h"
#include "database.h"
#include <QApplication>



int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    setlocale(LC_CTYPE, "Russian");
    std::shared_ptr<WBBack> back = std::make_shared<WBBack>();
    std::vector<std::map<std::string, std::string>> catalog_list = back->get_catalogs_wb1();

    //std::string url = "https://www.wildberries.ru/catalog/zhenshchinam/odezhda/bluzki-i-rubashki";

    //std::tuple<std::string, std::string, std::string> result = search_category_in_catalog(url, catalog_list);

    //if (!std::get<0>(result).empty()) {
    //    std::cout << "Название категории: " << std::get<0>(result) << std::endl;
    //    std::cout << "Shard: " << std::get<1>(result) << std::endl;
    //    std::cout << "Query: " << std::get<2>(result) << std::endl;
    //}
    for(const auto& catalog : catalog_list){
    back->get_content(catalog.at("shard"),catalog.at("query"), 10000, 100000);
    }
    //Database db;
    //db.CreateConnection();
    //db.CreateTables();
    //db.PushInfo("1", "1", "1", "1", "1");
    //db.DropTables();
    return 0;
}
