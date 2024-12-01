#include <string>
using namespace std;
namespace ya_challenge {

struct User {
    string id;
    string email;
    string password;
    string nickname;
    vector<std::string> categories; 
};


}
