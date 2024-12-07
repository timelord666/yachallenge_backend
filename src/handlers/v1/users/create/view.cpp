#include "view.hpp"

#include <fmt/format.h>
#include <userver/crypto/hash.hpp>
#include <userver/formats/json.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "../../../../lib/uuids.hpp"
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
    namespace pg = userver::storages::postgres;
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

    auto txn = pg_cluster_->Begin(
        pg::TransactionOptions(pg::IsolationLevel::kReadCommitted));

    auto result =
        txn.Execute(
                     "INSERT INTO yaChallenge.users(email, password, "
                     "nickname, androidToken) "
                     "VALUES($1, $2, $3, $4)"
                     "ON CONFLICT DO NOTHING "
                     "RETURNING id",
                     email.value(), hashed_password, nickname, token);

    if (result.IsEmpty()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kConflict);
      return {};
    }

    userver::formats::json::ValueBuilder response;
    response["id"] = result.AsSingleRow<std::string>();

    auto category_result = txn.Execute(
        "INSERT INTO yaChallenge.userCategories (categoryId, userId) "
        "SELECT UNNEST($1::text[]), $2",
        categories.value(), result.AsSingleRow<std::string>());

    txn.Commit();    

    return userver::formats::json::ToString(response.ExtractValue());
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;

};

}  // namespace
void AppendRegisterUser(userver::components::ComponentList& component_list) {
  component_list.Append<RegisterUser>();
}

}  // namespace ya_challenge
