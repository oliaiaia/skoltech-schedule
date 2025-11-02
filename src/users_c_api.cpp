#include "Users.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>

extern "C" {

Users* Users_New() {
    return new Users();
}

void Users_Delete(Users* u) {
    delete u;
}

// addNewUser
void Users_AddNewUser(Users* u, const char* name, const char** classes, int n, int term) {
    std::vector<std::string> v;
    for (int i = 0; i < n; i++) v.emplace_back(classes[i]);
    u->addNewUser(std::string(name), v, term);
}

// addCourse
void Users_AddCourse(Users* u, const char* name, const char** classes, int n) {
    std::vector<std::string> v;
    for (int i = 0; i < n; i++) v.emplace_back(classes[i]);
    u->addCourse(std::string(name), v);
}

// removeCourseData
void Users_RemoveCourseData(Users* u, const char* name, const char** classes, int n) {
    std::vector<std::string> v;
    for (int i = 0; i < n; i++) v.emplace_back(classes[i]);
    u->removeCourseData(std::string(name), v);
}

// changeFromScratchUserData
void Users_ChangeFromScratch(Users* u, const char* name, const char** classes, int n, int term) {
    std::vector<std::string> v;
    for (int i = 0; i < n; i++) v.emplace_back(classes[i]);
    u->changeFromScratchUserData(std::string(name), v, term);
}


}
