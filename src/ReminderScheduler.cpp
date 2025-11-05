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
    user.changeFromScratchUserData("Olia", userClasses, 2);
    
    // int a = 5;
    // std::cout << "a: " << a << std::endl;
    // std::cout << "&a: " << &a << std::endl;
    // auto p = &a;
    // std::cout << "*p: " << *p << std::endl;
    // std::cout << std::endl;

    // *p = *p + 10;
    // std::cout << "p: " << p << std::endl;
    // std::cout << "*p: " << *p << std::endl;
    // std::cout << "a: " << a << std::endl;
    // std::cout << "&a: " << &a << std::endl;
    // std::cout << std::endl;

    
    // // другая ячейка памяти
    // p = p + 10;
    // std::cout << "p: " << p << std::endl;
    // std::cout << "*p: " << *p << std::endl;
    // std::cout << "a: " << a << std::endl;
    // std::cout << "&a: " << &a << std::endl;
    // std::cout << std::endl;

    return 0;
}
