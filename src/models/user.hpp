#pragma once

#include <string>
#include <vector>
using namespace std;
namespace ya_challenge {

struct User {
    string id;
    string email;
    string password;
    string nickname;
    string androidToken;
    vector<std::string> categories; 
};


}