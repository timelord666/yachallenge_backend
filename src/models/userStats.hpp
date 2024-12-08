#pragma once

#include <string>
#include <vector>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
namespace ya_challenge {

struct UserStats {
  std::string nickname;
  long int score;
};
userver::formats::json::Value Serialize(const UserStats& data, userver::formats::serialize::To<userver::formats::json::Value>);

}
