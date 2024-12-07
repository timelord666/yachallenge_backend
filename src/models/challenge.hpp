#pragma once
#include <string>
#include <optional>

#include <userver/formats/json/value_builder.hpp>

using std::optional;
using std::string;

namespace ya_challenge {

struct Challenge {
  string id;
  string title;
  optional<string> imageUrl;
  string description;
  string category;
  long int score;
  string categoryTitle;
};

userver::formats::json::Value Serialize(
    const Challenge& data,
    userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace ya_challenge
