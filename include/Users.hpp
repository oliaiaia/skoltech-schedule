/**
 * хранит джейсон со всеми пользователями, их предметами и расписаниями, преподавателями
 * обращается к файлу джейсона через FileManager
 * может добавлять новых пользователей, удалять их, изменять расписания
 */

// structure of persons.json
// [
//   {
//     "name": "user1",
//      "term": 2,
//     "classes": ["class1", "class2"],
//     "schedule": [{"days": ["Monday", "Wednesday"], "times": ["9:00-12.00", "9:00-12.00"], "rooms": ["D1-101", "D2-309"], "teachers": ["D Fre", "F Eda"], "class": "class1"},
//                  {"days": ["Tuesday"], "times": ["14:00-17:00"], "rooms": ["D1-101"], "teachers": ["J Ihdhsy"], "class": "class2"}]
//   },
//   {
//     "name": "user2",
//     "schedule": [{"days": ["Monday", "Wednesday"], "times": ["9:00-12.00", "9:00-12.00"], "rooms": ["D1-101", "D2-309"], "teachers": ["D Fre", "F Eda"], "class": "class1"}]
//   }
// ]

#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#include <cstdio>

#include "FileManager.hpp"
#include "APIprocessor.hpp"

struct ScheduleItem
{
    std::string class_name;
    std::vector<std::string> days;
    std::vector<std::string> times;
    std::vector<std::string> rooms;
    std::vector<std::string> teachers;
};

struct UserSchedule
{
    std::string name;
    int term;
    std::vector<std::string> classes;
    std::vector<ScheduleItem> schedule;
};

class Users
{
public:
    Users();

    void addNewUser(const std::string &userName, const std::vector<std::string> &userClasses, int termNum);
    void addCourse(const std::string &userName, const std::vector<std::string> &userClasses);
    void changeFromScratchUserData(const std::string &userName, const std::vector<std::string> &userClasses, const int &termNum);
    void removeCourseData(const std::string &userName, const std::vector<std::string> &userClasses);

private:
    bool isTheUserExist(const std::string &userName);

    std::string getWeekdayByCode(int weekday);

    int getCodeByWeekday(std::string weekday);

    int getNumWeekday(int year, int month, int day);

    void convertUserToJson(const UserSchedule &userSch, nlohmann::json &newUserJson);
    
    void convertJsonToUser( UserSchedule &userSch, const nlohmann::json &newUserJson);

    void sortByDay(ScheduleItem &scheduleItem);

    void getWeekDataFromApi(int termNum, std::unordered_map<int, std::string> &week);

    void findNecessaryCourseFromDay(const std::unordered_map<int, std::string> & allDays, ScheduleItem &scheduleItem);

    void fillClassesForUser(const std::vector<std::string> &userClasses, int termNum, std::vector<ScheduleItem> &fullUserSchedule);

    std::string filename = "persons.json";
    FileManager fileManager;
    nlohmann::json personsJson;
    APIprocessor apiprocessor;
};