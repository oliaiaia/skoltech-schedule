#pragma once

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using str_const = const std::string&;

class APIprocessor
{

public:

    std::string getDayList(str_const yearId, str_const date)
    {
        //"https://schedule.skoltech.ru:8443/api/v1/terms/2025-2026-term2/classes?date=2025-10-31"
        std::string dayURL;
        dayURL = mainUrlBody + "/" + yearId + "/" + "classes?date=" + date;
        return getDataByLink(dayURL);
    }

    std::string getTermsList()
    {
       return getDataByLink(mainUrlBody);
    }

    std::string getDataByLink(str_const url)
    {

        std::string response;

        // Создаёт handle (дескриптор) HTTP-сессии.
        CURL *curl = curl_easy_init();
        // проверка успешности инициализации
        if (curl)
        {

            // Устанавливаем адрес, на который будет отправлен запрос.
            //  эквивалентно GET https://schedule.skoltech.ru/api/courses?term=2025-2026-term2
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            // Разрешаем следовать за HTTP-перенаправлениями (redirect 301/302).
            // Если сервер ответит, что нужные данные по другому URL, libcurl автоматически перейдёт туда.
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            // CURLOPT_WRITEFUNCTION указывает функцию-обработчик (callback),
            // которая будет вызываться каждый раз, когда приходят кусочки данных от сервера.
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

            // CURLOPT_WRITEDATA передаёт указатель на объект, в который callback будет писать данные.
            // В данном случае &response — это std::string
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Это команда выполнит запрос.
            // Здесь libcurl:
            // устанавливает соединение с сервером,
            // отправляет HTTP GET,
            // получает ответ,
            // вызывает WriteCallback по мере поступления данных.
            CURLcode res = curl_easy_perform(curl);

            // Если res != CURLE_OK, значит произошла ошибка (например, CURLE_COULDNT_RESOLVE_HOST, CURLE_OPERATION_TIMEDOUT, CURLE_HTTP_RETURNED_ERROR и т.д.).

            if (res != CURLE_OK)
            {

                // curl_easy_strerror(res) возвращает строковое описание ошибки.
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                return response;
            }

            // Освобождаем ресурсы, связанные с дескриптором CURL*.
            // Если не вызвать — будет утечка памяти.
            curl_easy_cleanup(curl);
        }
        return response;
    }

private:
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
    {
        size_t totalSize = size * nmemb;
        output->append((char *)contents, totalSize);
        return totalSize;
    }

    std::string mainUrlBody = "https://schedule.skoltech.ru:8443/api/v1/terms";
};
