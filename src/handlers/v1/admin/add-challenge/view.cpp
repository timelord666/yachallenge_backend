#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/formats/serialize/common_containers.hpp>


namespace ya_challenge {

namespace {
class AddChallenge final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-admin-add-challenge";

    AddChallenge(const userver::components::ComponentConfig& config,
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
	auto request_body = userver::formats::json::FromString(request.RequestBody());
	auto title = request_body["title"].As<std::optional<std::string>>();
	auto imageUrl = request_body["imageUrl"].As<std::optional<std::string>>();
	auto description = request_body["description"].As<std::optional<std::string>>();
	auto category = request_body["category"].As<std::optional<std::string>>();
    	auto score = request_body["score"].As<std::optional<int>>();

        if (!title || !description || !score || !category) {
		auto& response = request.GetHttpResponse();
		response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
		return {};
    	}
        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "WITH category_id AS ( "
    		"SELECT id "
    	        "FROM yaChallenge.categories "
    	        "WHERE title = $4 "
	    ") "
	    "INSERT INTO yaChallenge.challenges (title, imageUrl, description, category, score) "
	        "SELECT "
    	        "$1, "
                "$2, "
                "$3, "
                "category_id.id, "
                "$5 "
	    "FROM category_id "
	    "WHERE category_id.id IS NOT NULL "
	    "ON CONFLICT DO NOTHING "
	    "RETURNING id", title, imageUrl, description, category, score
        );
	if(result.IsEmpty()){
		auto& response = request.GetHttpResponse();
                response.SetStatus(userver::server::http::HttpStatus::kConflict);
                return {};
	}
	userver::formats::json::ValueBuilder response;
	response["id"] = result.AsSingleRow<std::string>();
        return userver::formats::json::ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}

void AppendAddChallenge(userver::components::ComponentList& component_list) {
    component_list.Append<AddChallenge>();
}

}
