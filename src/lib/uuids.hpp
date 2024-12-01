#pragma once
#include <vector>
#include <string>
#include <boost/uuid/uuid.hpp> 




namespace ya_challenge {

std::vector<boost::uuids::uuid> ConvertStringsToUuids(
    const std::vector<std::string>& uuid_strings);

}  // namespace ya_challenge