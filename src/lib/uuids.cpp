#include "uuids.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <iostream>


namespace ya_challenge {



std::vector<boost::uuids::uuid> ConvertStringsToUuids(
    const std::vector<std::string>& uuid_strings) {
  std::vector<boost::uuids::uuid> uuids;
  boost::uuids::string_generator gen;

  for (const auto& uuid_str : uuid_strings) {
    
    boost::uuids::uuid uuid = gen(uuid_str);
    uuids.push_back(uuid);
    
  }

  return uuids;
}



}  // namespace ya_challenge