#include <chrono>
#include <string>
#include <string_view>

#include <userver/components/component.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/periodic_task.hpp>

#include <userver/engine/async.hpp>
#include <userver/engine/task/cancel.hpp>

#include <userver/clients/http/client.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace my_service {

class MyPeriodicTaskComponent final
    : public userver::components::LoggableComponentBase {
 public:
  static constexpr std::string_view kName = "periodic-send-notifications";

  MyPeriodicTaskComponent(const userver::components::ComponentConfig& config,
                          const userver::components::ComponentContext& context)
      : LoggableComponentBase(config, context),
        http_client_{context.FindComponent<userver::components::HttpClient>()
                         .GetHttpClient()},
        pg_cluster_(
            context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {

    periodic_task_.Start(
        "periodic-send-notifications", std::chrono::seconds(1), [this]() {
          userver::engine::Async([this]()
                                     -> userver::engine::TaskWithResult<void> {
            try {
              auto res = co_await pg_cluster_->Execute(
                  "SELECT androidToken FROM yaChallenge.users;");

              LOG_INFO() << "Fetched " << res.Size()
                         << " rows from the database.";

              for (const auto& row : res) {
                std::string name = row.As<std::string>(0);

                auto response = co_await http_client_->CreateRequest()
                                    .post("localhost:9000/ping")
                                    .retry(2)
                                    .timeout(std::chrono::seconds(5))
                                    .perform();

                if (response->IsOk()) {
                  LOG_INFO()
                      << "HTTP request for row with id=" << id
                      << " succeeded with status: " << response->GetStatus();
                } else {
                  LOG_ERROR()
                      << "HTTP request for row with id=" << id
                      << " failed with status: " << response->GetStatus();
                }
              }
            } catch (const std::exception& ex) {
              LOG_ERROR() << "An error occurred in periodic task: "
                          << ex.what();
            }

            co_return;
          }).Detach();  // Detach to run asynchronously
        });
  }

  ~MyPeriodicTaskComponent() override {
    // Stop the task gracefully on destruction
    periodic_task_.Stop();
  }


 private:
  userver::storages::postgres::Cluster* pg_cluster_{nullptr};
  userver::clients::http::Client& http_client_;
  userver::utils::PeriodicTask periodic_task_;
};

}  // namespace my_service

// Register the component so it can be added to the configuration
USERVER_COMPONENT_REGISTRY_ADD_TRANSIENT(
    my_service::MyPeriodicTaskComponent,
    my_service::MyPeriodicTaskComponent::kName)
