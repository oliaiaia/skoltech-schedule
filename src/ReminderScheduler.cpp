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



    std::vector <std::string> userClasses = {"Advanced Optimization Methods", "Introduction to Optical Biosensors", "Computational Chemistry and Materials Modeling"};

    Users user;
    user.addNewUser("Olive", userClasses, 2);
    

    return 0;
}
