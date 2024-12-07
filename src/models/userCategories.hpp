#pragma once

#include <vector>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/json/value.hpp>
namespace ya_challenge {

struct UserCategories {
  int count;
  std::optional<std::vector<userver::formats::json::Value>> categories;
};
userver::formats::json::Value Serialize(const UserCategories& data, userver::formats::serialize::To<userver::formats::json::Value>);

}
