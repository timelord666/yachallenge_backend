#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/daemon_run.hpp>

#include "handler/v1/login/login.hpp"

int main(int argc, char* argv[]) {
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::HttpClient>()
          .Append<userver::server::handlers::TestsControl>()

  ya_challenge::AppendLoginUser(component_list);

  return userver::utils::DaemonMain(argc, argv, component_list);
}