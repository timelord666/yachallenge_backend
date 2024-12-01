#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "../../../models/challenge.hpp"
#include "./view.hpp"

namespace ya_challenge {

namespace {
class GetChallenges final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-get-challenges";

  GetChallenges(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& category = request.GetArg("category");

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        R"(
          SELECT id::text, category::text, *
          FROM yaChallenge.challenges
          WHERE ($1 IS NULL OR category = $1)
      )",
        category.empty() ? std::nullopt : std::optional<std::string>(category));

    userver::formats::json::ValueBuilder response;

    response["challenges"].Resize(0);

    for (auto challenge :
         result.AsSetOf<Challenge>(userver::storages::postgres::kRowTag)) {
      response["challenges"].PushBack(challenge);
    }

    return userver::formats::json::ToString(response.ExtractValue());
  };

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetChallenges(userver::components::ComponentList& component_list) {
  component_list.Append<GetChallenges>();
};
};  // namespace ya_challenge