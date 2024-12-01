#include <sstream>
#include <unordered_map>
#include <iostream>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "../../../models/category.hpp"
#include "./view.hpp"

namespace ya_challenge {

namespace {
class GetCategories final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-get-bookmarks";

  GetCategories(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "select * from yaChallenge.category");

    userver::formats::json::ValueBuilder response;

    response["categories"].Resize(0);

    for (auto row :
         result.AsSetOf<Category>(userver::storages::postgres::kRowTag)) {
      response["categories"].PushBack(row);
    }

    return userver::formats::json::ToString(response.ExtractValue());
  };

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetCategories(userver::components::ComponentList& component_list) {
  component_list.Append<GetCategories>();
};
};  // namespace ya_challenge