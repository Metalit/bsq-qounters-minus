#include "config.hpp"

Configuration& getConfig() {
    static Configuration _config((ModInfo){ID, VERSION});
    _config.Load();
    return _config;
}

template<typename T>
std::string LookupEnumString(T value, std::unordered_map<std::string, T> lookupTable) {
    for (auto kv : lookupTable) {
        if (kv.second == value) return kv.first;
    }
    return "";
}

UnityEngine::Color ParseHexColor(std::string hexString) {
    int r, g, b;
    sscanf(hexString.substr(1).c_str(), "%02x%02x%02x", &r, &g, &b);
    return UnityEngine::Color((float)r/255, (float)g/255, (float)b/255, 1.0f);
}

bool QountersMinus::LoadConfig() {
    bool foundEverything = true;
    getConfig().Load();
    auto customQounterConfig = getConfig().config["CustomCounters"].GetObject();

    for (auto def : QountersMinus::QounterRegistry::registry) {
        auto _childConfig = (def.second.isBaseQounter ? getConfig().config[def.second.configKey] : customQounterConfig[def.second.configKey]).GetObject();
        auto&& jsonConfigObj = def.second.configKey == "" ? getConfig().config.GetObject() : _childConfig;
        if (def.second.configKey != "") {
            if (def.second.isBaseQounter && (!getConfig().config.HasMember(def.second.configKey) || !getConfig().config[def.second.configKey].IsObject())) {
                LOG_DEBUG("Config object not found: " + def.second.configKey);
                foundEverything = false;
                continue;
            } else if (!def.second.isBaseQounter && (!customQounterConfig.HasMember(def.second.configKey) || !customQounterConfig[def.second.configKey].IsObject())) {
                LOG_DEBUG("Custom Qounter config object not found: " + def.second.configKey);
                foundEverything = false;
                continue;
            }
        }
        // TODO: handle nesting of custom qounter configs
        for (auto fieldConfig : def.second.configMetadata) {
            auto jsonKey = fieldConfig->jsonKey == "" ? fieldConfig->field : fieldConfig->jsonKey;
            if (!jsonConfigObj.HasMember(jsonKey)) {
                LOG_DEBUG("Config key not found: " + def.second.configKey + "." + jsonKey);
                foundEverything = false;
                continue;
            }
            if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Bool) {
                if (jsonConfigObj[jsonKey].IsBool()) {
                    *(bool*)fieldConfig->ptr = jsonConfigObj[jsonKey].GetBool();
                } else {
                    LOG_DEBUG("Config key is wrong type (expected bool): " + def.second.configKey + "." + jsonKey);
                    foundEverything = false;
                }
            } else if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Float) {
                if (jsonConfigObj[jsonKey].IsFloat()) {
                    *(float*)fieldConfig->ptr = jsonConfigObj[jsonKey].GetFloat();
                } else {
                    LOG_DEBUG("Config key is wrong type (expected float): " + def.second.configKey + "." + jsonKey);
                    foundEverything = false;
                }
            } else if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Int) {
                if (jsonConfigObj[jsonKey].IsInt()) {
                    *(int*)fieldConfig->ptr = jsonConfigObj[jsonKey].GetInt();
                } else {
                    LOG_DEBUG("Config key is wrong type (expected int): " + def.second.configKey + "." + jsonKey);
                    foundEverything = false;
                }
            } else if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Enum) {
                if (jsonConfigObj[jsonKey].IsString()) {
                    auto val = jsonConfigObj[jsonKey].GetString();
                    if (fieldConfig->enumSerializedNames.contains(val)) {
                        *(int*)fieldConfig->ptr = fieldConfig->enumSerializedNames[val];
                    } else {
                        LOG_DEBUG("Config key is unknown for enum: \"" + val + "\", " + def.second.configKey + "." + jsonKey);
                        foundEverything = false;
                    }
                } else {
                    LOG_DEBUG("Config key is wrong type (expected string): " + def.second.configKey + "." + jsonKey);
                    foundEverything = false;
                }
            } else if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Color) {
                if (jsonConfigObj[jsonKey].IsString()) {
                    *(UnityEngine::Color*)fieldConfig->ptr = ParseHexColor(jsonConfigObj[jsonKey].GetString());
                } else {
                    LOG_DEBUG("Config key is wrong type (expected string): " + def.second.configKey + "." + jsonKey);
                    foundEverything = false;
                }
            } else {
                // TODO (minor)
                // LOG_DEBUG("JSON field " + def.second.configKey + "." + jsonKey + " has unknown type \"" + (int)fieldConfig->type + "\"");
                foundEverything = false;
            }
        }
    }
    
    LOG_DEBUG("Config found all: %d", foundEverything);
    return foundEverything;
}

void QountersMinus::SaveConfig() {
    LOG_CALLER;

    auto prevCustomConfig = getConfig().config["CustomCounters"].GetObject();
    getConfig().config.RemoveAllMembers();
    getConfig().config.SetObject();
    rapidjson::Document::AllocatorType& allocator = getConfig().config.GetAllocator();
    rapidjson::Value customConfig(rapidjson::kObjectType);

    for (auto def : QountersMinus::QounterRegistry::registry) {
        rapidjson::Value _childConfig(rapidjson::kObjectType);
        auto&& childConfig = def.second.configKey == "" ? getConfig().config : _childConfig;
        for (auto fieldConfig : def.second.configMetadata) {
            auto _jsonKey = fieldConfig->jsonKey == "" ? fieldConfig->field : fieldConfig->jsonKey;
            rapidjson::Value jsonKey(_jsonKey, allocator);
            if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Bool) {
                childConfig.AddMember(jsonKey, *(bool*)fieldConfig->ptr, allocator);
            } else if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Float) {
                childConfig.AddMember(jsonKey, *(float*)fieldConfig->ptr, allocator);
            } else if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Int) {
                childConfig.AddMember(jsonKey, *(int*)fieldConfig->ptr, allocator);
            } else if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Enum) {
                std::string serializedEnumVal;
                for (auto pair : fieldConfig->enumSerializedNames) {
                    if (pair.second == *(int*)fieldConfig->ptr) {
                        serializedEnumVal = pair.first;
                        break;
                    }
                }
                if (serializedEnumVal == "") {
                    LOG_DEBUG("Config could not find string for enum val %d (" + def.second.configKey + "." + _jsonKey + ")", *(int*)fieldConfig->ptr);
                } else {
                    childConfig.AddMember(jsonKey, serializedEnumVal, allocator);
                }
            } else if (fieldConfig->type == QountersMinus::QounterRegistry::ConfigType::Color) {
                std::string hex = FormatColorToHex(*(UnityEngine::Color*)fieldConfig->ptr);
                childConfig.AddMember(jsonKey, hex, allocator);
            } else {
                // TODO (minor)
                // LOG_DEBUG("JSON field " + def.second.configKey + "." + _jsonKey + " has unknown type \"" + (int)fieldConfig->type + "\"");
            }
        }
        if (def.second.configKey != "") {
            rapidjson::Value configKey(def.second.configKey, allocator);
            if (def.second.isBaseQounter) {
                getConfig().config.AddMember(configKey, childConfig, allocator);
            } else {
                customConfig.AddMember(configKey, childConfig, allocator);
            }
        }
    }

    for (auto itr = prevCustomConfig.MemberBegin(); itr != prevCustomConfig.MemberEnd(); ++itr) {
        if (!customConfig.HasMember(itr->name) || !customConfig[itr->name].IsObject()) {
            LOG_DEBUG("Preserving " + itr->name.GetString() + " in Custom Qounter config");
            customConfig.AddMember(itr->name, itr->value, allocator);
        }
    }

    getConfig().config.AddMember("CustomCounters", customConfig, allocator);
    getConfig().Write();
}
