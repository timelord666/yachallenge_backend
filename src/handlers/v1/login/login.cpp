#include <fmt/format.h>
#include "login.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/crypto/hash.hpp>

#include "../../../models/user.hpp"
using namespace std;

namespace ya_challenge {

namespace {
    class LoginUser final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-login-user";

        LoginUser(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& component_context)
            : HttpHandlerBase(config, component_context),
                pg_cluster_(
                    component_context
                        .FindComponent<userver::components::Postgres>("postgres-db-1")
                        .GetCluster()) {}

        std::string HandleRequestThrow(
            const userver::server::http::HttpRequest& request,
            userver::server::request::RequestContext&
        ) const override {

            auto JsonBody = userver::formats::json::FromString(request.RequestBody());
            
            if (!JsonBody.HasMember("email") || !JsonBody.HasMember("password")) {
                auto& response = request.GetHttpResponse();
                response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
                return {};
            }

            auto email = JsonBody["email"].As<string>("");
            auto password = JsonBody["password"].As<string>("");

            if (email.empty() || password.empty()) {
                auto& response = request.GetHttpResponse();
                response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
                return {};
            }
            password = userver::crypto::hash::Sha256(password);

            auto userResult = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                "SELECT * FROM yaChallenge.user WHERE email = $1",
                email
            );


            if (userResult.IsEmpty()) {
                auto& response = request.GetHttpResponse();
                response.SetStatus(userver::server::http::HttpStatus::kNotFound);
                return {};
            }

            struct User {
                std::string id;
                std::string password;
            };
            auto user = userResult.AsSingleRow<User>(userver::storages::postgres::kRowTag);

            if (password != user.password) {
                auto& response = request.GetHttpResponse();
                response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
                return {};
            }

            userver::formats::json::ValueBuilder response;
            response["id"] = user.id;

            return userver::formats::json::ToString(response.ExtractValue());
        }

    private:
        userver::storages::postgres::ClusterPtr pg_cluster_;
    };
    }
void AppendLoginUser(userver::components::ComponentList& component_list) {
    component_list.Append<LoginUser>();
}
}


