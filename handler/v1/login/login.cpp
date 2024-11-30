#include <userver/crypto/hash.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace ya_challenge
{
    class RegisterUser final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-register-user";

        RegisterUser(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context)
            : HttpHandlerBase(config, context),
            pg_cluster_(
                context
                    .FindComponent<userver::components::Postgres>("postgres-db")
                    .GetCluster()) {}

        std::string HandleRequestThrow(
            const userver::server::http::HttpRequest& request,
            userver::server::request::RequestContext&) const override {
            auto email = request.GetFormDataArg("email").value_or("");
            auto password = request.GetFormDataArg("password").value_or("");
            auto nickname = request.GetFormDataArg("nickname").value_or("");

            if (email.empty() || password.empty()) {
                throw userver::server::handlers::ClientError("Missing required fields");
            }

            auto hashed_password = userver::crypto::hash::Sha256(password);
            auto uuid = GenerateUuid();

            auto user_result = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                "SELECT COUNT(*) FROM User WHERE Email = $1", email);
            if (user_result.AsSingleRow<int>() > 0) {
                throw userver::server::handlers::ClientError("Email already registered");
            }

            pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                "INSERT INTO User (UUID, Email, Password, Nickname) VALUES ($1, $2, $3, $4)",
                uuid, email, hashed_password, nickname);

            userver::formats::json::ValueBuilder response;
            response["message"] = "Registration successful";
            response["user_uuid"] = uuid;

            return userver::formats::json::ToString(response.ExtractValue());
        }

    private:
        userver::storages::postgres::ClusterPtr pg_cluster_;

        static std::string GenerateUuid() {
            return fmt::format("{}-{}", std::time(nullptr), rand());
        }
    };
}

