#include "weather.hpp"
#include <cstdlib>
#include <ctime>

Weather::Weather() {
    std::srand(std::time(nullptr));
    updateWeather();
}

std::string Weather::getCurrentWeather() {
    return m_currentWeather;
}

void Weather::updateWeather() {
    const std::string weatherConditions[] = {
        "Sunny", "Cloudy", "Rainy", "Snowy", "Windy"
    };
    int temperature = std::rand() % 41 - 10; // -10 to 30 degrees Celsius
    int conditionIndex = std::rand() % (sizeof(weatherConditions) / sizeof(weatherConditions[0]));
    
    m_currentWeather = weatherConditions[conditionIndex] + ", " + std::to_string(temperature) + "°C";
}