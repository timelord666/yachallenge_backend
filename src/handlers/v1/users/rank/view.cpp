#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace ya_challenge {

namespace {
class GetRank final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-v1-users-rank";

  GetRank(const userver::components::ComponentConfig& config,
              const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    auto& user_id = request.GetPathArg("id");
    // TODO: refator this to pagination
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT "
        "u.id, u.nickname, "
        "COALESCE(SUM(c.score), 0) AS score "
        "FROM "
        "yaChallenge.users u "
        "LEFT JOIN "
        "yaChallenge.completedChallenges cc ON u.id = cc.userId "
        "LEFT JOIN "
        "yaChallenge.challenges c ON cc.challengeId = c.id "
        "GROUP BY "
        "u.id "
        "ORDER BY "
        "score DESC ");

    userver::formats::json::ValueBuilder response;

    if (result.IsEmpty()) {
      return userver::formats::json::ToString(response.ExtractValue());
    }
    long long rank = 0;
    for (const auto& row : result) {
      rank++;
      if (row["id"].As<std::string>() == user_id) {
        
        response["nickname"] = row["nickname"].As<std::string>();
        response["score"] = row["score"].As<int>();
        response["rank"] = rank;
        return userver::formats::json::ToString(response.ExtractValue());
      }
    }
    return userver::formats::json::ToString(response.ExtractValue());
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetRank(userver::components::ComponentList& component_list) {
  component_list.Append<GetRank>();
}

}  // namespace ya_challenge
