#pragma once

#include <vector>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/json/value.hpp>
namespace ya_challenge {

struct CompletedChallenges {
  int count;
  std::vector<userver::formats::json::Value> challenges;
};
userver::formats::json::Value Serialize(const CompletedChallenges& data, userver::formats::serialize::To<userver::formats::json::Value>);

}
