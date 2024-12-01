#include "./challenge.hpp"
#include <optional>
#include <string>
namespace ya_challenge {
userver::formats::json::Value Serialize(
    const Challenge& data,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;

  item["id"] = data.id;
  item["title"] = data.title;

  if (data.imageUrl) {
    item["imageUrl"] = data.imageUrl.value();
  }

  item["description"] = data.description;
  item["category"] = data.category;
  item["score"] = data.score;

  return item.ExtractValue();
};
}  // namespace ya_challenge