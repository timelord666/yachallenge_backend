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
  static constexpr std::string_view kName = "handler-v1-users-rankings";

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

    int pages = 0;
    bool user_not_found = true;
    while(user_not_found) {
      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "SELECT "
          "u.nickname, "
          "COALESCE(SUM(c.score), 0) AS score "
          "FROM "
          "yaChallenge.users u "
          "LEFT JOIN "
          "yaChallenge.completedChallenges cc ON u.id = cc.userId "
          "LEFT JOIN "
          "yaChallenge.challenges c ON cc.challengeId = c.id "
          "GROUP BY "
          "u.nickname "
          "ORDER BY "
          "score DESC "
          "LIMIT 15 "
          "OFFSET $1",
          pages * 15);

            
    }

    

    userver::formats::json::ValueBuilder response;
    response["users"].Resize(0);

    if (result.IsEmpty()) {
      return userver::formats::json::ToString(response.ExtractValue());
    }

    for (const auto& row : result) {
      userver::formats::json::ValueBuilder userElem;
      userElem["nickname"] = row["nickname"].As<std::string>();
      userElem["score"] = row["score"].As<int>();
      response["users"].PushBack(std::move(userElem));
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
