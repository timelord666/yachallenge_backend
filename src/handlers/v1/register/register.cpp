#include "register.hpp"

#include <fmt/format.h>
#include <userver/crypto/hash.hpp>
#include <userver/formats/json.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
namespace ya_challenge {
namespace {
class RegisterUser final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-register-user";

  RegisterUser(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& context)
      : HttpHandlerBase(config, context),
        pg_cluster_(
            context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    auto email = request_body["email"].As<std::optional<std::string>>();
    auto nickname = request_body["nickname"].As<std::optional<std::string>>();
    auto password = request_body["password"].As<std::optional<std::string>>();
    auto token = request_body["androidToken"].As<std::optional<std::string>>();
    auto categories = request_body["categories"].As<std::optional<std::vector<std::string>>>();

    if (!email|| !password || !nickname || !categories) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }

    auto hashed_password = userver::crypto::hash::Sha256(password.value());
    auto uuid = GenerateUuid();

    auto user_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT COUNT(*) FROM User WHERE Email = $1", email);
    if (user_result.AsSingleRow<int>() > 0) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }

    pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                         "INSERT INTO User (id, email, password, nickname, categories, androidToken) "
                         "VALUES ($1, $2, $3, $4, $5, $6)",
                         uuid, email.value(), hashed_password,
                         nickname, categories.value(),
                         token.value_or(nullptr));

    userver::formats::json::ValueBuilder response;
    response["id"] = uuid;

    return userver::formats::json::ToString(response.ExtractValue());
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;

  static std::string GenerateUuid() {
    return fmt::format("{}-{}", std::time(nullptr), rand());
  }
};

}  // namespace
void AppendRegisterUser(userver::components::ComponentList& component_list) {
  component_list.Append<RegisterUser>();
}

}  // namespace ya_challenge
