
#include "Users.hpp"
Users::Users()
{
    personsJson = fileManager.loadJsonFromFile(filename);
}

void Users::addNewUser(const std::string &userName, const std::vector<std::string> &userClasses, int termNum)
{
    std::vector<std::string> uniqueClasses = userClasses;
    std::unordered_set<std::string> userClassesSet(userClasses.begin(), userClasses.end());
    uniqueClasses.assign(userClassesSet.begin(), userClassesSet.end());

    if (!isTheUserExist(userName))
    {
        UserSchedule newUser;
        newUser.name = userName;
        newUser.term = termNum;
        newUser.classes = uniqueClasses;
        fillClassesForUser(uniqueClasses, termNum, newUser.schedule);

        nlohmann::json newUserJson;
        convertUserToJson(newUser, newUserJson);
        personsJson.push_back(newUserJson);

        fileManager.saveJsonToFile(personsJson, filename);
    }
}

void Users::addCourse(const std::string &userName, const std::vector<std::string> &userClasses) {

    std::unordered_set<std::string> userClassesSet(userClasses.begin(), userClasses.end());

    auto jsonLinkByName = std::find_if(personsJson.begin(), personsJson.end(),
                           [&userName](const nlohmann::json &person)
                           {
                               return person["name"].get<std::string>() == userName;
                           });


    if (jsonLinkByName != personsJson.end())
    {

        UserSchedule userSch;
        nlohmann::json &currentUserCourses = *jsonLinkByName;
        convertJsonToUser(userSch, currentUserCourses);

        std::unordered_map<int, std::string> week;
        getWeekDataFromApi(userSch.term, week);
        

        for(const auto &course: userSch.classes) {
            auto linkToReapitedVal = userClassesSet.find(course); 
            if(linkToReapitedVal != userClassesSet.end()) {
                userClassesSet.erase(linkToReapitedVal);
            }
        }

        std::vector<std::string> uniqueClasses = userClasses;
        uniqueClasses.assign(userClassesSet.begin(), userClassesSet.end());

        for(const auto & addedCourse : uniqueClasses) {
            ScheduleItem scheduleItem;
            scheduleItem.class_name = addedCourse;
            findNecessaryCourseFromDay(week, scheduleItem);
            
            userSch.schedule.push_back(scheduleItem);
            userSch.classes.push_back(addedCourse);
        }

        convertUserToJson(userSch, currentUserCourses);
        fileManager.saveJsonToFile(personsJson, filename);
    }
}

void Users::removeCourseData(const std::string &userName, const std::vector<std::string> &userClasses)
{
    auto it = std::find_if(personsJson.begin(), personsJson.end(),
                           [&userName](const nlohmann::json &person)
                           {
                               return person["name"].get<std::string>() == userName;
                           });

    if (it != personsJson.end())
    {
        nlohmann::json &currentUserCourses = *it;
        
        std::unordered_set<std::string> classesToRemove(userClasses.begin(), userClasses.end());
        
        nlohmann::json newClasses = nlohmann::json::array();
        for (const auto &course : currentUserCourses["classes"]) {
            std::string courseName = course.get<std::string>();
            if (classesToRemove.find(courseName) == classesToRemove.end()) {
                newClasses.push_back(course);
            }
        }
        currentUserCourses["classes"] = newClasses;
        
        nlohmann::json newSchedule = nlohmann::json::array();
        for (const auto &scheduleItem : currentUserCourses["schedule"]) {
            std::string className = scheduleItem["class"].get<std::string>();
            if (classesToRemove.find(className) == classesToRemove.end()) {
                newSchedule.push_back(scheduleItem);
            }
        }
        currentUserCourses["schedule"] = newSchedule;
        fileManager.saveJsonToFile(personsJson, filename);
    }
}

void Users::changeFromScratchUserData(const std::string &userName, const std::vector<std::string> &userClasses, const int &termNum)
{

    personsJson.erase(
        std::remove_if(personsJson.begin(), personsJson.end(),
                       [&userName](const nlohmann::json &person)
                       {
                           return person["name"].get<std::string>() == userName;
                       }),
        personsJson.end());

    UserSchedule updatedUser;
    updatedUser.name = userName;
    updatedUser.term = termNum;
    updatedUser.classes = userClasses;
    fillClassesForUser(userClasses, termNum, updatedUser.schedule);

    nlohmann::json updatedUserJson;
    convertUserToJson(updatedUser, updatedUserJson);
    personsJson.push_back(updatedUserJson);

    fileManager.saveJsonToFile(personsJson, filename);
}

bool Users::isTheUserExist(const std::string &userName)
{
    for (const auto &person : personsJson)
    {
        if (person["name"].get<std::string>() == userName)
        {
            return true;
        }
    }
    return false;
}

std::string Users::getWeekdayByCode(int weekday)
{
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


int Users::getCodeByWeekday(std::string weekday)
{
    if(weekday.find("Sunday") != std::string::npos) return 0;
    if(weekday.find("Monday") != std::string::npos) return 1;
    if(weekday.find("Tuesday") != std::string::npos) return 2;
    if(weekday.find("Wednesday") != std::string::npos) return 3;
    if(weekday.find("Thursday") != std::string::npos) return 4;
    if(weekday.find("Friday") != std::string::npos) return 5;
    if(weekday.find("Saturday") != std::string::npos) return 6;
}

int Users::getNumWeekday(int year, int month, int day)
{
    std::tm timeinfo = {};
    timeinfo.tm_year = year - 1900; // год с 1900
    timeinfo.tm_mon = month - 1;    // месяц 0-11
    timeinfo.tm_mday = day;

    std::mktime(&timeinfo);
    int weekday = timeinfo.tm_wday;
    return weekday;
}

void Users::convertUserToJson(const UserSchedule &userSch, nlohmann::json &newUserJson)
{
    newUserJson["name"] = userSch.name;
    newUserJson["term"] = userSch.term;
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

void Users::convertJsonToUser( UserSchedule &userSch, const nlohmann::json &userJson)
{
    userSch.name = userJson["name"];
    userSch.term = userJson["term"];
    userSch.classes = userJson["classes"];

    userSch.schedule.clear();
    
    for (auto &courseJson : userJson["schedule"])
    {
        ScheduleItem coursesInfo;
        coursesInfo.class_name = courseJson["class"];
        coursesInfo.days = courseJson["days"];
        coursesInfo.times = courseJson["times"];
        coursesInfo.rooms = courseJson["rooms"];
        coursesInfo.teachers = courseJson["teachers"];
        userSch.schedule.push_back(coursesInfo);
    }
}

void Users::getWeekDataFromApi(int termNum, std::unordered_map<int, std::string> &week)
{

    std::string termStart, termID;
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
                std::cout << "term start time " << termStart << std::endl;
                break; // to get the last study year for the term
            }
        }
    }
    catch (std::exception &e)
    {
        return;
    }

    int year, month, day;

    if (std::sscanf(termStart.c_str(), "%d-%d-%d", &year, &month, &day) != 3)
    {
        return;
    }

    if ((day + 2*7) > 30)
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

    for (int t = 0; t < weekDaysAmount; t++)
    {
        if (day + t < 10) termStart = std::to_string(year) + "-" + std::to_string(month) + "-0" + std::to_string(day + t);
        else termStart = std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day + t);

        std::string theDay = apiprocessor.getDayList(termID, termStart);
        //if the day is weekend \ holiday
        if(theDay.size() < 10) {

            int theDayInTheNextWeek = day + 7;

            if (theDayInTheNextWeek + t < 10) termStart = std::to_string(year) + "-" + std::to_string(month) + "-0" + std::to_string(theDayInTheNextWeek + t);
            else termStart = std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(theDayInTheNextWeek + t);

            theDay = apiprocessor.getDayList(termID, termStart);
        }

        week.insert({getNumWeekday(year, month, day + t), theDay});
    }

}

  void Users::sortByDay(ScheduleItem &scheduleItem) {
        // Создаем вектор индексов
        std::vector<size_t> indices(scheduleItem.days.size());
        for (size_t i = 0; i < scheduleItem.days.size(); ++i) {
            indices[i] = i;
        }
        
        // Сортируем индексы по дням
        std::sort(indices.begin(), indices.end(),
            [this, scheduleItem](size_t i, size_t j) {
                return getCodeByWeekday(scheduleItem.days[i]) < getCodeByWeekday(scheduleItem.days[j]);
            });
        
        // Создаем временные векторы с отсортированными данными
        auto sorted_days = scheduleItem.days;
        auto sorted_rooms = scheduleItem.rooms;
        auto sorted_teachers = scheduleItem.teachers;
        auto sorted_times = scheduleItem.times;
        
        for (size_t i = 0; i < indices.size(); ++i) {
            scheduleItem.days[i] = sorted_days[indices[i]];
            scheduleItem.rooms[i] = sorted_rooms[indices[i]];
            scheduleItem.teachers[i] = sorted_teachers[indices[i]];
            scheduleItem.times[i] = sorted_times[indices[i]];
        }
    }

void Users::findNecessaryCourseFromDay(const std::unordered_map<int, std::string> &allDays, ScheduleItem &scheduleItem)
{

    for (const auto &[dayNum, oneDaySchedule] : allDays)
    {
        try
        {
            auto jsonDaySchedule = nlohmann::json::parse(oneDaySchedule);

            // перебор по всем предметам этого дня
            for (auto &disciplineStudied : jsonDaySchedule)
            {

                std::string courceName = disciplineStudied.value("course_name", "N/A");
                std::string instructors, start_time, end_time, room_id;

                // курс из списка курсов терма и дисциплина студента соотвествуют друг другу
                if (scheduleItem.class_name.find(courceName) != std::string::npos)
                {
                    
                    scheduleItem.days.push_back(getWeekdayByCode(dayNum));
                    scheduleItem.rooms.push_back(disciplineStudied.value("room_id", "N/A"));
                    scheduleItem.teachers.push_back(disciplineStudied.value("instructors", "N/A"));

                    start_time = disciplineStudied.value("start_time", "N/A");
                    end_time = disciplineStudied.value("end_time", "N/A");
                    scheduleItem.times.push_back(start_time + "-" + end_time);

                    break;
                }
            }
        }

        catch (std::exception &e)
        {
            return;
        }
    }

    sortByDay(scheduleItem);
}

void Users::fillClassesForUser(const std::vector<std::string> &userClasses, int termNum, std::vector<ScheduleItem> &fullUserSchedule)
{
    //убрано копирование

    for (const auto &cource : userClasses)
    {
        ScheduleItem newCourse;
        newCourse.class_name = cource;
        fullUserSchedule.push_back(newCourse);
    }

    std::unordered_map<int, std::string> week;
    getWeekDataFromApi(termNum, week);

    std::vector<std::thread> threads;

    for (auto &item : fullUserSchedule)
    {
        // week как ссылка, чтобы не копировать всю мапу в каждый поток. потоки должны джойниться до выхода из функции
        threads.emplace_back([&week, &item, this]()
                             { findNecessaryCourseFromDay(week, item); });
    }

    for (auto &t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}
