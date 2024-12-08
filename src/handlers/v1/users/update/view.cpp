#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace ya_challenge {

namespace {
class UpdateUserById final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-update-user-by-id";

  UpdateUserById(const userver::components::ComponentConfig& config,
              const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& id = request.GetPathArg("id");
    
    const auto& body = request.RequestBody();
    auto& response = request.GetHttpResponse();

    if (body.empty()) {
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }
    
    auto request_body =
        userver::formats::json::FromString(body);

    auto email = request_body["email"].As<std::optional<std::string>>();
    auto nickname = request_body["nickname"].As<std::optional<std::string>>();
    auto token = request_body["androidToken"].As<std::optional<std::string>>();

    const std::string query = R"(
      update yaChallenge.users
      set 
        email = coalesce($1, email),
        nickname = coalesce($2, nickname),
        androidToken = coalesce($3, androidToken)
      where
        id = $4
    )";

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster, query, email,
        nickname, token, id);

    response.SetStatus(userver::server::http::HttpStatus::kNoContent);

    return {};
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendUpdateUserById(userver::components::ComponentList& component_list) {
  component_list.Append<UpdateUserById>();
}

}  // namespace ya_challenge
