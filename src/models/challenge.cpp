#include "./challenge.hpp"

namespace ya_challenge {
userver::formats::json::Value Serialize(
    const Challenge& data,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;

  item["id"] = data.id;
  item["title"] = data.title;
  // item["imageUrl"] = data.imageUrl;
  item["description"] = data.description;
  item["category"] = data.category;
  item["score"] = data.score;

  return item.ExtractValue();
};
}  // namespace ya_challenge