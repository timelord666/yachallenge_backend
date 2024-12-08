#pragma once
#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace ya_challenge {    

void AppendCompletedChallenge(userver::components::ComponentList& component_list);

}