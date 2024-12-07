#pragma once

#include <string>
#include <vector>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
namespace ya_challenge {

struct User {
  std::string id;
  std::string email;
  std::string nickname;
  std::optional<std::string> androidToken;
};
userver::formats::json::Value Serialize(const User& data, userver::formats::serialize::To<userver::formats::json::Value>);

}
