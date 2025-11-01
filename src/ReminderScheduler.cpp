#include "ReminderScheduler.hpp"
#include <iostream>
#include <string>
#include "Users.hpp"
#include <vector>


int main() {
    
    // std::cout << "Enter study year (e.g., 2023-2024): ";
    // std::string year;
    // std::getline(std::cin, year);
    // year = year;
    
    // std::cout << "Enter term number (e.g., 2): ";
    // int termNumber;
    // std::cin >> termNumber;



    std::vector <std::string> userClasses = {"Planning Algorithms in Artificial Intelligence", "Leader in Startup", "Systems Engineering", "CDE Seminars"};

    Users user;
    // user.changeFromScratchUserData("Kate", userClasses, 2);

    user.changeFromScratchUserData("Olia", userClasses, 2);
    user.addCourse("Olia", {"Bioinformatics"});
    

    return 0;
}
