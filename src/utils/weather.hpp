#pragma once

#include <string>

class Weather {
public:
    Weather();
    std::string getCurrentWeather();
    void updateWeather();

private:
    std::string m_currentWeather;
};