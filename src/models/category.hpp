#pragma once
#include <string>

#include <userver/formats/json/value_builder.hpp>

namespace ya_challenge {
struct Category {
  std::string id;
  std::optional<std::string> imageUrl;
  std::string title;
};

userver::formats::json::Value Serialize(
    const Category& data,
    userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace ya_challenge