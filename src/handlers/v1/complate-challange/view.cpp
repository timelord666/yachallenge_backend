#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <nlohmann/json.hpp>

using namespace std;

namespace ya_challenge{
    namespace {
        class CompleteChallengeHandler : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr string_view kName = "handler-complete-challenge";

            using HttpHandlerBase::HttpHandlerBase;

            string HandleRequestThrow(
                const userver::server::http::HttpRequest& request,
                userver::server::request::RequestContext&) const override {
                
                const auto& path_params = request.GetPathArgs();
                const string& user_id = path_params.at("id");
                const string& challenge_id = path_params.at("challenge_id");

                auto db = GetComponent<userver::storages::postgres::Component>().GetCluster();

                auto user_exists = db->Execute(
                    "SELECT COUNT(*) FROM yaChallenge.users WHERE id = $1", user_id);
                if (user_exists.AsSingleRow<int>() == 0) {
                    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
                    return nlohmann::json{{"error", "User not found"}}.dump();
                }

                auto challenge_exists = db->Execute(
                    "SELECT COUNT(*) FROM yaChallenge.challenges WHERE id = $1", challenge_id);
                if (challenge_exists.AsSingleRow<int>() == 0) {
                    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
                    return nlohmann::json{{"error", "Challenge not found"}}.dump();
                }

                try {
                    db->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                        "INSERT INTO yaChallenge.completedChallenges (userId, challengeId) "
                        "VALUES ($1, $2) "
                        "ON CONFLICT (userId, challengeId) DO UPDATE "
                        "SET completedAt = CURRENT_TIMESTAMP",
                        user_id, challenge_id);

                    return nlohmann::json{{"status", "success"}}.dump();
                } catch (const exception& e) {
                    request.SetResponseStatus(userver::server::http::HttpStatus::kInternalServerError);
                    return nlohmann::json{{"error", e.what()}}.dump();
                }
            }
        };
    } // namespace

    void AppendCompletedChallange(userver::components::ComponentList& component_list) {
        component_list.Append<CompleteChallengeHandler>();
    }
} // namespace ya_challange

