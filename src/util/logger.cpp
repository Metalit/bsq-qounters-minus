#include "util/logger.hpp"

Logger& getLogger() {
    static auto logger = new Logger((ModInfo){ID, VERSION}, {false, true});
    return *logger;
}
