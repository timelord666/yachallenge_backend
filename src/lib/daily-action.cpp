#pragma once
#include <userver/components/component_list.hpp>
#include <userver/engine/run_standalone.hpp>
#include <userver/utils/periodic_task.hpp>

#include <chrono>
#include <functional>
#include <iostream>

namespace ya_challenge {

class DailyActionComponent final
    : public userver::components::LoggableComponentBase {
 public:
  static constexpr auto kName = "daily-action-component";

  DailyActionComponent(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context,
                       std::function<void()> daily_action)
      : LoggableComponentBase(config, context),
        daily_action_(std::move(daily_action)) {
    std::chrono::seconds initial_delay = std::chrono::seconds(0);

    std::chrono::hours interval(24);

    periodic_task_.Start("daily-task", initial_delay, interval,
                         [this]() { PerformDailyAction(); });
  }

  ~DailyActionComponent() override { periodic_task_.Stop(); }

 private:
  void PerformDailyAction() {
    if (daily_action_) {
      daily_action_();
    } else {
      std::cout << "No daily action provided." << std::endl;
    }
  }

  userver::utils::PeriodicTask periodic_task_;
  std::function<void()> daily_action_;
};

class DailyActionComponentFactory {
 public:
  static void Append(userver::components::ComponentList& component_list,
                     std::function<void()> daily_action) {
    component_list.Append<my_service::DailyActionComponent>(
        [daily_action](const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context) {
          return std::make_unique<my_service::DailyActionComponent>(
              config, context, daily_action);
        });
  }
};

}  // namespace ya_challenge
