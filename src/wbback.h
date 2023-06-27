#ifndef WBBACK_H
#define WBBACK_H
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
#include "database.h"
class WBBack
{
private:
    Database db;
public:
    WBBack();
    //size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
    void save_data_to_file(const rapidjson::Document& data);
    void get_catalogs_wb();
    //size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    std::vector<std::map<std::string, std::string>> get_catalogs_wb1();
    std::tuple<std::string, std::string, std::string> search_category_in_catalog(const std::string& url, const std::vector<std::map<std::string, std::string>>& catalog_list);
    std::vector<std::map<std::string, std::string>> get_data_from_json(const std::map<std::string, std::map<std::string, std::vector<std::map<std::string, std::string>>>>& json_file);
    std::vector<std::map<std::string, std::string>> get_data_from_json(const rapidjson::Value& json_file);
    std::vector<std::map<std::string, std::string>> get_content(const std::string& shard, const std::string& query, int low_price = -1, int top_price = -1);

};

#endif // WBBACK_H
