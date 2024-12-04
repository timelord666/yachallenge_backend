#include "userCategories.hpp"

namespace ya_challenge {

userver::formats::json::Value Serialize(const UserCategories& data,
                                        userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["count"] = data.count;
    item["categories"] = data.categories;
    return item.ExtractValue();
}

} 

