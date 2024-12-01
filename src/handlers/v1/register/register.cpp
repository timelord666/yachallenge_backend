#include <userver/crypto/hash.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/utils/assert.hpp>


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
    const auto& email = request.GetFormDataArg("email").value;
    const auto& password = request.GetFormDataArg("password").value;
    const auto& nickname = request.GetFormDataArg("nickname").value;

    if (email.empty() || password.empty()) {
      throw userver::server::handlers::ClientError("Missing required fields");
    }

    auto hashed_password = userver::crypto::hash::Sha256(password);

    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                         "INSERT INTO yaChallenge.user (email, password, nickname) "
                         "VALUES ($1, $2, $3) "
			 "ON CONFLICT DO NOTHING "
        		 "RETURNING user.id",
                         email, hashed_password, nickname);

    if (result.IsEmpty()) {
        // If no row is returned, the user already exists
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return "User already exists";
    }

    userver::formats::json::ValueBuilder response;
    response["id"] = result.AsSingleRow<std::string>();

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
