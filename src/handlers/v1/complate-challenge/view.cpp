#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include "../../../models/challenge.hpp"
#include "../../../models/user.hpp"
#include "./view.hpp"

using namespace std;

namespace ya_challenge{
    namespace {
        class CompleteChallengeHandler final : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr string_view kName = "handler-complete-challenge";

            CompleteChallengeHandler(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& component_context)
                : HttpHandlerBase(config, component_context),
                    pg_cluster_(
                        component_context
                            .FindComponent<userver::components::Postgres>("postgres-db-1")
                            .GetCluster()) {}

            using HttpHandlerBase::HttpHandlerBase;

            string HandleRequestThrow(
                const userver::server::http::HttpRequest& request,
                userver::server::request::RequestContext&) const override {
                
                const std::string& user_id = request.GetPathArg("userId");
                const std::string& challenge_id = request.GetPathArg("challengeId");

                auto user_exists = pg_cluster_->Execute(
                    userver::storages::postgres::ClusterHostType::kMaster,
                    "SELECT COUNT(*) FROM yaChallenge.users WHERE id = $1", user_id);

                if (user_exists.IsEmpty() || user_exists.AsSingleRow<int64_t>() == 0) {
                    auto& response = request.GetHttpResponse();
                    // request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
                    // return userver::formats::json::ValueBuilder{{"error", "User not found"}}.ExtractValue().ToString();
                    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
                    return {};
                }

                auto challenge_exists = pg_cluster_->Execute(
                    userver::storages::postgres::ClusterHostType::kMaster,
                    "SELECT COUNT(*) FROM yaChallenge.challenges WHERE id = $1", challenge_id);

                if (user_exists.IsEmpty() || challenge_exists.AsSingleRow<int64_t>() == 0) {
                    auto& response = request.GetHttpResponse();
                    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
                    // response.SetBody(userver::formats::json::ValueBuilder{{"error", "Challenge not found"}}
                    //                     .ExtractValue()
                    //                     .ToString());
                    return {};
                }
                try {
                    pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                        "INSERT INTO yaChallenge.completedChallenges (userId, challengeId) "
                        "VALUES ($1, $2) "
                        "ON CONFLICT (userId, challengeId) DO UPDATE "
                        "SET completedAt = CURRENT_TIMESTAMP",
                        user_id, challenge_id);

                    userver::formats::json::ValueBuilder response;

                    response["success"] = true; 
                    return userver::formats::json::ToString(response.ExtractValue());
                    
                } catch (const std::exception& e) {
                    auto& response = request.GetHttpResponse();
                    response.SetStatus(userver::server::http::HttpStatus::kInternalServerError);
                    return {};
                }
            }

            userver::storages::postgres::ClusterPtr pg_cluster_;

        };
    } // namespace

    void AppendCompletedChallenge(userver::components::ComponentList& component_list) {
        component_list.Append<CompleteChallengeHandler>();
    };
}; // namespace ya_challange

