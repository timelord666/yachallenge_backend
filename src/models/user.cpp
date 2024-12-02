#include "user.hpp"

namespace ya_challenge {

userver::formats::json::Value Serialize(const User& user,
                                        userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["id"] = user.id;
    item["email"] = user.email;
    item["nickname"] = user.nickname;
    item["password"] = user.password;
    item["completedChallengeIds"] = user.completedChallengeIds;
    item["androidToken"] = user.androidToken;
    item["categories"] = user.categories;
    return item.ExtractValue();
}

} 

