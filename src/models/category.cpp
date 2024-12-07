#include "./category.hpp"

namespace ya_challenge {
userver::formats::json::Value Serialize(
    const Category& data,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["id"] = data.id;
  item["title"] = data.title;

  if (data.imageUrl) {
    item["imageUrl"] = data.imageUrl.value();
  }

  return item.ExtractValue();
};

}  // namespace ya_challenge
