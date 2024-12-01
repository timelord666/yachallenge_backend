#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace ya_challenge {
void AppendGetChallenges(userver::components::ComponentList& component_list);
}