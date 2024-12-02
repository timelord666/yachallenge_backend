#include "completedChallenges.hpp"

namespace ya_challenge {

userver::formats::json::Value Serialize(const CompletedChallenges& data,
                                        userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["count"] = data.count;
    item["challenges"] = data.challenges;
    return item.ExtractValue();
}

} 

