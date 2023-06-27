#include "wbback.h"
#include <corecrt.h>
#include "database.h"
WBBack::WBBack()
{

}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    ((std::string*)userdata)->append(ptr, size * nmemb);
    return size * nmemb;
}

void WBBack::save_data_to_file(const rapidjson::Document& data) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    data.Accept(writer);
    std::ofstream file("./wb_catalogs_data.json");
    if (file.is_open()) {
        file << buffer.GetString();
        file.close();
        std::cout << "Данные сохранены в wb_catalogs_data.json" << std::endl;
    } else {
        std::cout << "Ошибка при открытии файла" << std::endl;
    }
}

void WBBack::get_catalogs_wb() {
    std::string url = "https://static-basket-01.wb.ru/vol0/data/main-menu-ru-ru-v2.json";
    std::string response_string;
    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , FALSE);
        //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , 1);
        //curl_easy_setopt(curl, CURLOPT_CAINFO, "C:\xampp\apache\bin\curl-ca-bundle.crt");
        CURLcode result = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (result == CURLE_OK) {
            rapidjson::Document data;
            data.Parse(response_string.c_str());
            if (!data.HasParseError()) {
                save_data_to_file(data);
            } else {
                std::cout << "Ошибка при парсинге данных" << std::endl;
            }
        } else {
            std::cout << "Ошибка при выполнении запроса" << std::endl;
        }
    } else {
        std::cout << "Ошибка при инициализации CURL" << std::endl;
    }
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::vector<std::map<std::string, std::string>> WBBack::get_catalogs_wb1() {
    db.CreateConnection();
    db.CreateTables();

    std::vector<std::map<std::string, std::string>> data_list;

    const char* url = "https://www.wildberries.ru/webapi/menu/main-menu-ru-ru.json";
    std::string response_data;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , FALSE);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cout << "Error: " << curl_easy_strerror(res) << std::endl;
            return data_list;
        }
    } else {
        std::cout << "Error: Failed to initialize CURL." << std::endl;
        return data_list;
    }

    rapidjson::Document document;
    document.Parse(response_data.c_str());

    if (document.IsArray()) {
        for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr) {
            const rapidjson::Value& d = *itr;

            if (d.HasMember("childs") && d["childs"].IsArray()) {
                const rapidjson::Value& childs = d["childs"];
                for (rapidjson::Value::ConstValueIterator childItr = childs.Begin(); childItr != childs.End(); ++childItr) {
                    const rapidjson::Value& child = *childItr;

                    try {
                        std::string category_name = child["name"].GetString();
                        std::string category_url = child["url"].GetString();
                        std::string shard = "0";
                        std::string query = "0";
                        if(category_url.substr(0,5)!="https"){
                            shard = child["shard"].GetString();
                            query = child["query"].GetString();
                        }
                        std::map<std::string, std::string> data;
                        data["category_name"] = category_name;
                        data["category_url"] = category_url;
                        data["shard"] = shard;
                        data["query"] = query;

                        data_list.push_back(data);
                    } catch (...) {
                        continue;
                    }

                    if (child.HasMember("childs") && child["childs"].IsArray()) {
                        const rapidjson::Value& sub_childs = child["childs"];
                        for (rapidjson::Value::ConstValueIterator subChildItr = sub_childs.Begin(); subChildItr != sub_childs.End(); ++subChildItr) {
                            const rapidjson::Value& sub_child = *subChildItr;

                            try {
                                std::string category_name = sub_child["name"].GetString();
                                std::string category_url = sub_child["url"].GetString();
                                std::string shard = "0";
                                std::string query = "0";

                                if(category_url.substr(0,5)!="https"){
                                shard = sub_child["shard"].GetString();
                                query = sub_child["query"].GetString();
                                }

                                std::map<std::string, std::string> data;
                                data["category_name"] = category_name;
                                data["category_url"] = category_url;
                                data["shard"] = shard;
                                data["query"] = query;


                                data_list.push_back(data);
                            } catch (...) {
                                continue;
                            }
                        }
                    }
                }
            }
        }
    }

    return data_list;
}

std::tuple<std::string, std::string, std::string> WBBack::search_category_in_catalog(const std::string& url, const std::vector<std::map<std::string, std::string>>& catalog_list) {
    try {
        for (const auto& catalog : catalog_list) {
            if (catalog.at("category_url") == url.substr(url.find("https://www.wildberries.ru")+ strlen("https://www.wildberries.ru"))) {
                std::string name_category = catalog.at("category_name");
                std::string shard = catalog.at("shard");
                std::string query = catalog.at("query");
                std::cout << "найдено совпадение: " << name_category << std::endl;
                return std::make_tuple(name_category, shard, query);
            }
        }
        // Если нет совпадения
        // std::cout << "нет совпадения" << std::endl;
    } catch (...) {
        std::cout << "Данный раздел не найден!" << std::endl;
    }

    return std::make_tuple("", "", "");
}



std::vector<std::map<std::string, std::string>> WBBack::get_data_from_json(const rapidjson::Value& json_file) {
    std::vector<std::map<std::string, std::string>> data_list;

    const rapidjson::Value& products = json_file["data"]["products"];
    for (const auto& data : products.GetArray()) {
        try {
            int price = data["priceU"].GetInt() / 100;
            int salePrice = data["salePriceU"].GetInt() / 100;

            std::map<std::string, std::string> item;
            item["Наименование"] = data["name"].GetString();
            item["id"] = std::to_string(data["id"].GetInt());
            item["Скидка"] = data["sale"].GetInt();
            item["Цена"] = std::to_string(price);
            item["Цена со скидкой"] = std::to_string(salePrice);
            item["Бренд"] = data["brand"].GetString();
            item["id бренда"] = std::to_string(data["brandId"].GetInt());
            item["feedbacks"] = std::to_string(data["feedbacks"].GetInt());
            item["rating"] = std::to_string(data["rating"].GetInt());
            item["Ссылка"] = "https://www.wildberries.ru/catalog/" + std::to_string(data["id"].GetInt()) + "/detail.aspx?targetUrl=BP";
            std::string id = std::to_string(data["id"].GetInt());
            std::string sale = std::to_string(data["sale"].GetInt());
            std::string SPrice = std::to_string(salePrice);
            std::string rating = std::to_string(data["rating"].GetInt());
            std::string link = "https://www.wildberries.ru/catalog/" + std::to_string(data["id"].GetInt()) + "/detail.aspx?targetUrl=BP";
            db.PushInfo(QString::fromStdString(id),QString::fromStdString(SPrice),QString::fromStdString(sale),QString::fromStdString(rating),QString::fromStdString(link));
            data_list.push_back(item);
        } catch (...) {
            continue;
        }
    }

    return data_list;
}



std::vector<std::map<std::string, std::string>> WBBack::get_content(const std::string& shard, const std::string& query, int low_price, int top_price) {
    std::vector<std::map<std::string, std::string>> data_list;

    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: */*");
        headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

        std::string response_data;
        std::string url;
        for (int page = 1; page <= 100; ++page) {
            std::cout << "Positions from page " << page << " / 100" << std::endl;
            url = "https://catalog.wb.ru/catalog/" + shard + "/catalog?appType=1&curr=rub&dest=-1075831,-77677,-398551,12358499" +
                  "&locale=en&page=" + std::to_string(page) + "&priceU=" + std::to_string(low_price * 100) + ";" + std::to_string(top_price * 100) +
                  "&regions=64,83,4,38,80,33,70,82,86,30,69,1,48,22,66,31,40&sort=popular&spp=0&" + query;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , FALSE);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Ошибка при выполнении запроса: " << curl_easy_strerror(res) << std::endl;
                break;
            }

            rapidjson::Document document;
            document.Parse(response_data.c_str());
            if (document.HasParseError()) {
                std::cerr << "Json ERROR" << std::endl;
                break;
            }

            std::vector<std::map<std::string, std::string>> page_data = get_data_from_json(document);
            std::cout << "Added positions: " << page_data.size() << std::endl;

            if (!page_data.empty()) {
                data_list.insert(data_list.end(), page_data.begin(), page_data.end());
            } else {
                std::cout << "Сбор данных завершен." << std::endl;
                break;
            }

            response_data.clear();
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Не удалось инициализировать CURL" << std::endl;
    }

    return data_list;
}

