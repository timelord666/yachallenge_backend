#pragma once
#include <string>

#include <userver/formats/json/value_builder.hpp>

using std::string;

namespace ya_challenge {

struct Challenge {
  string id;
  string title;
  std::optional<string> imageUrl;
  string description;
  string category;
  long int score;
};

userver::formats::json::Value Serialize(
    const Challenge& data,
    userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace ya_challenge
