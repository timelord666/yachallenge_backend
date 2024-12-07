#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/formats/serialize/common_containers.hpp>

#include "../../../../models/completedChallenges.hpp"

namespace ya_challenge {

namespace {
class GetCompleted final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-users-challenges";

    GetCompleted(const userver::components::ComponentConfig& config,
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

        const auto& id = request.GetPathArg("id");
        if(id.empty()){
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
            return {};
        }
        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "SELECT " 
    	    "COUNT(cc.challengeId) AS count, " 
    	    "ARRAY_AGG( "
        	"JSON_BUILD_OBJECT( "
            	    "'id', c.id, "
           	    "'title', c.title, "
            	    "'imageUrl', c.imageUrl, "
            	    "'description', c.description, "
            	    "'category', c.category, "
            	    "'score', c.score, "
            	    "'completedAt', cc.completedAt "
       	    	    ") "
    		") AS challenges "
    	    "FROM "
    		"yaChallenge.completedChallenges cc "
	    "JOIN "
    		"yaChallenge.challenges c "
	    "ON "
    		"cc.challengeId = c.id "
	    "WHERE "
    		"cc.userId = $1",
            id
        );

       
	auto completedChallenges = result.AsOptionalSingleRow<CompletedChallenges>(userver::storages::postgres::kRowTag);
       return userver::formats::json::ToString(userver::formats::json::ValueBuilder{completedChallenges}.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}

void AppendGetCompleted(userver::components::ComponentList& component_list) {
    component_list.Append<GetCompleted>();
}

}