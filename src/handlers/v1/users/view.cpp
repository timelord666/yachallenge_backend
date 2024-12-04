#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../models/user.hpp"

namespace ya_challenge {

namespace {
class GetProfile final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-users-user";

    GetProfile(const userver::components::ComponentConfig& config,
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
   	    "u.id, "
    	    "u.email, "
	    "u.androidToken, "
    	    "u.nickname "
	    "FROM yaChallenge.users u "
            "WHERE "
            "u.id = $1",
            id
        );

        if (result.IsEmpty()) {
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kNotFound);
            return {};
        }

       auto user = result.AsSingleRow<User>(userver::storages::postgres::kRowTag);
       userver::formats::json::ValueBuilder response;
       return userver::formats::json::ToString(userver::formats::json::ValueBuilder{user}.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  

void AppendGetProfile(userver::components::ComponentList& component_list) {
    component_list.Append<GetProfile>();
}

}  
