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
class AddCategory final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-admin-add-category";

    AddCategory(const userver::components::ComponentConfig& config,
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

        if (!title) {
		auto& response = request.GetHttpResponse();
		response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
		return {};
    	}

        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
	    "INSERT INTO yaChallenge.categories (title, imageUrl) "
	    "VALUES($1, $2) "
	    "ON CONFLICT (title) DO NOTHING "
	    "RETURNING id", title, imageUrl
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

void AppendAddCategory(userver::components::ComponentList& component_list) {
    component_list.Append<AddCategory>();
}

}
