#pragma once

#include <string>
#include <vector>

namespace ya_challenge {

struct User {
  std::string id;
  std::string email;
  std::string password;
  std::string nickname;
  std::vector<std::string> categories;
};
}
