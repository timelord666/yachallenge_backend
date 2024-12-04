#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "handlers/v1/get-categories/view.hpp"
#include "handlers/v1/users/create/view.hpp"
#include "handlers/v1/users/view.hpp"
#include "handlers/v1/users/challenges/view.hpp"
#include "handlers/v1/users/categories/view.hpp"
#include "hello.hpp"
#include "handlers/v1/login/login.hpp"


int main(int argc, char* argv[]) {
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::components::HttpClient>()
          .Append<userver::server::handlers::TestsControl>()
          .Append<userver::components::Postgres>("postgres-db-1")
          .Append<userver::clients::dns::Component>();


  ya_challenge::AppendHello(component_list);
  ya_challenge::AppendRegisterUser(component_list);
  ya_challenge::AppendGetUserCategories(component_list);
  ya_challenge::AppendGetProfile(component_list);
  ya_challenge::AppendLoginUser(component_list);
  ya_challenge::AppendGetCategories(component_list);
  ya_challenge::AppendGetCompleted(component_list);
  return userver::utils::DaemonMain(argc, argv, component_list);
}
