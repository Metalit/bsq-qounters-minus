#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"

Logger& getLogger();

// yes this is terrible, yes i am ashamed of myself, no i cba to make this correct
#define __LOG_DEBUG(m, ...) getLogger().debug(std::string( "%s (%s:" + std::to_string(__LINE__) + ")" + m).c_str(), __func__, __FILE__, __VA_ARGS__)
// #define __LOG_DEBUG(m, ...) ;
#define _LOG_DEBUG(m, ...) __LOG_DEBUG(": " + m, __VA_ARGS__)
#define LOG_DEBUG(...) _LOG_DEBUG(__VA_ARGS__, "")
#define LOG_CALLER __LOG_DEBUG("", "")
