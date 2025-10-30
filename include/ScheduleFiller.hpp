#pragma once

#include <chrono>

/**
 * find actaul years: 1-2 last availible years from https://schedule.skoltech.ru:8443/api/v1/terms
 * fill info from api for each person in persons.json
 */
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <nlohmann/json.hpp>


class ScheduleFiller {
public:

ScheduleFiller(){}
    void initNewPerson() {
        
    }
    void fillSchedules(const std::set<std::string>& personClassesList, nlohmann::json &personClassesJson){

    }

};