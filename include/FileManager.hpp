#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

class FileManager
{
public:
    bool saveJsonToFile(const json &j, const std::string &filename)
    {
        try
        {
            std::ofstream file(filename);
            if (!file.is_open())
            {
                std::cerr << "Ошибка: не удалось создать/открыть файл " << filename << " для записи" << std::endl;
                return false;
            }

            file << j.dump(4);
            file.close();

            std::cout << "[FileManager] JSON успешно сохранен в файл: " << filename
                      << " (кол-во элементов: " << j.size() << ")" << std::endl;
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "[FileManager] Ошибка при сохранении JSON: " << e.what() << std::endl;
            return false;
        }
    }

    json loadJsonFromFile(const std::string &filename)
    {
        try
        {
            std::ifstream file(filename);
            if (!file.is_open())
            {
                std::cerr << "[FileManager] Файл " << filename << " не найден. Создаю новый пустой JSON-массив." << std::endl;
                return json::array();  
            }

            json j = json::parse(file);
            file.close();

            std::cout << "[FileManager] JSON успешно загружен из файла: " << filename
                      << " (кол-во элементов: " << j.size() << ")" << std::endl;
            return j;
        }
        catch (const std::exception &e)
        {
            std::cerr << "[FileManager] Ошибка при загрузке JSON: " << e.what()
                      << ". Возвращаю пустой массив." << std::endl;
            return json::array(); 
        }
    }
};
