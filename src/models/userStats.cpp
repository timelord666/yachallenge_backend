#include "userStats.hpp"

namespace ya_challenge {

userver::formats::json::Value Serialize(const UserStats& user,
                                        userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["nickname"] = user.nickname;
    item["score"] = user.score;
    return item.ExtractValue();
}

} 

