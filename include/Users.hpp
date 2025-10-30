/**
 * хранит джейсон со всеми пользователями, их предметами и расписаниями, преподавателями
 * обращается к файлу джейсона через FileManager
 * может добавлять новых пользователей, удалять их, изменять расписания
 */

#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <ctime>
#include <cstdio>

#include "FileManager.hpp"
#include "APIprocessor.hpp"

// structure of persons.json
// [
//   {
//     "name": "user1",
//     "classes": ["class1", "class2"],
//     "schedule": [{"days": ["Monday", "Wednesday"], "times": ["9:00-12.00", "9:00-12.00"], "rooms": ["D1-101", "D2-309"], "teachers": ["D Fre", "F Eda"], "class": "class1"},
//                  {"days": ["Tuesday"], "times": ["14:00-17:00"], "rooms": ["D1-101"], "teachers": ["J Ihdhsy"], "class": "class2"}]
//   },
//   {
//     "name": "user2",
//     "schedule": [{"days": ["Monday", "Wednesday"], "times": ["9:00-12.00", "9:00-12.00"], "rooms": ["D1-101", "D2-309"], "teachers": ["D Fre", "F Eda"], "class": "class1"}]
//   }
// ]

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
    std::vector<std::string> classes;
    std::vector<ScheduleItem> schedule;
};

class Users
{
public:
    Users()
    {
        personsJson = fileManager.loadJsonFromFile(filename);
    }

    void addNewUser(const std::string &userName, const std::vector<std::string> &userClasses, const int &termNum)
{
    if (!isTheUserExist(userName))
    {
        UserSchedule newUser;
        newUser.name = userName;
        newUser.classes = userClasses;
        fillClassesForUser(userClasses, termNum, newUser.schedule);

        nlohmann::json newUserJson;
        convertUserToJson(newUser, newUserJson);
        personsJson.push_back(newUserJson);
        
        fileManager.saveJsonToFile(personsJson, filename);
    }
}


    nlohmann::json getScheduleForUser(const std::string &userName)
    {
        if (isTheUserExist(userName))
        {
            for (const auto &person : personsJson)
            {
                // if (person["name"].get<std::string>().find(userName) != std::string::npos)
                if (person["name"].get<std::string>() == userName)
                {
                    return person["schedule"]; 
                }
            }
        }
        return nlohmann::json(); 
    }

private:
    bool isTheUserExist(const std::string &userName)
    {
        for (const auto &person : personsJson)
        {
            if (person["name"].get<std::string>() == userName)
            // if (person["name"].get<std::string>().find(userName) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    }

    std::string getWeekday(int year, int month, int day)
    {
        std::tm timeinfo = {};
        timeinfo.tm_year = year - 1900; // год с 1900
        timeinfo.tm_mon = month - 1;    // месяц 0-11
        timeinfo.tm_mday = day;

        std::mktime(&timeinfo); // нормализует структуру tm
        int weekday = timeinfo.tm_wday;
        switch (weekday)
        {
        case 0:
            return "Sunday";
        case 1:
            return "Monday";
        case 2:
            return "Tuesday";
        case 3:
            return "Wednesday";
        case 4:
            return "Thursday";
        case 5:
            return "Friday";
        case 6:
            return "Saturday";
        default:
            return "";
        }
    }

    void convertUserToJson(const UserSchedule &userSch, nlohmann::json &newUserJson)
    {
        newUserJson["name"] = userSch.name;
        newUserJson["classes"] = userSch.classes;
        newUserJson["schedule"] = nlohmann::json::array(); 

        for (const auto &course : userSch.schedule)
        {
            nlohmann::json courseJson;
            courseJson["class"] = course.class_name;
            courseJson["days"] = course.days;
            courseJson["times"] = course.times;
            courseJson["rooms"] = course.rooms;
            courseJson["teachers"] = course.teachers;
            newUserJson["schedule"].push_back(courseJson);
        }
    }

    void fillClassesForUser(const std::vector<std::string> &userClasses, const int &termNum, std::vector<ScheduleItem> &fullUserSchedule)
    {
        std::string termStart, termEnd, termID;

        {
            std::string allTerms = apiprocessor.getTermsList();

            try
            {

                auto jsonTerms = nlohmann::json::parse(allTerms);
                for (auto &term : jsonTerms)
                {

                    termID = term.value("id", "N/A");
                    if (termID.find("term" + std::to_string(termNum)) != std::string::npos)
                    {
                        termStart = term.value("start_date", "");
                        termEnd = term.value("end_date", "");
                        break; //to get the last study year for the term
                    }
                }
            }
            catch (std::exception &e)
            {
                return;
            }
        }



        for (const auto &cource : userClasses)
        {
            ScheduleItem newCourse;
            newCourse.class_name = cource;
            fullUserSchedule.push_back(newCourse);
        }

        int year, month, day;
        if (std::sscanf(termStart.c_str(), "%d-%d-%d", &year, &month, &day) != 3)
        {
            return; 
        }
        if ((day + 7) > 29)
        {

            day = 1;

            if (month == 12)
            {
                month = 1;
                year++;
            }
            else
            {
                month++;
            }
        }

        int weekDaysAmount = 7;

        // каждый день недели
        for (int t = 0; t < weekDaysAmount; t++)
        {
            if(day + t < 10) {
                termStart = std::to_string(year) + "-" + std::to_string(month) + "-0" + std::to_string(day + t);
            }
            else termStart = std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day + t);
            std::string theDay = apiprocessor.getDayList(termID, termStart);
            
            std::cout << "theDay" << theDay << std::endl;

            std::string weekday = getWeekday(year, month, day + t);

            try
            {

                auto jsonDay = nlohmann::json::parse(theDay);

                // перебор по всем предметам этого дня
                for (auto &disciplineStudied : jsonDay)
                {

                    std::string courceName = disciplineStudied.value("course_name", "N/A");
                    std::string instructors, start_time, end_time, room_id;

                    // поиск соотвествия предмета дня и выбранного предмета
                    for (const auto &cource : userClasses)
                    {
                        // курс из списка курсов терма и дисциплина студента соотвествуют друг другу
                        if (cource.find(courceName) != std::string::npos)
                        {
                            instructors = disciplineStudied.value("instructors", "N/A");
                            start_time = disciplineStudied.value("start_time", "N/A");
                            end_time = disciplineStudied.value("end_time", "N/A");
                            room_id = disciplineStudied.value("room_id", "N/A");
                            break;
                        }
                    }

                    for (auto &item : fullUserSchedule)
                    {
                        if (item.class_name.find(courceName) != std::string::npos)
                        {
                            item.days.push_back(weekday);
                            item.rooms.push_back(room_id);
                            item.teachers.push_back(instructors);
                            item.times.push_back(start_time + "-" + end_time);
                        }
                    }
                }
            }

            catch (std::exception &e)
            {
                return;
            }
        }
    }

    std::string filename = "persons.json";
    FileManager fileManager;
    nlohmann::json personsJson;
    APIprocessor apiprocessor;
};