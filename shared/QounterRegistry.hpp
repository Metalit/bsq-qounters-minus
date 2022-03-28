#pragma once

#include "util/logger.hpp"
#include "Qounter.hpp"
#include "InjectedComponents.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "UnityEngine/Animator.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/MultiplayerLocalActivePlayerFacade.hpp"
#include "TMPro/FontStyles.hpp"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

namespace QountersMinus {
    class QounterRegistry {
        public:
            enum Event {
                NoteCut,
                NoteMiss,
                ScoreUpdated,
                // MaxScoreUpdated,
                SwingRatingFinished,
            };
            struct EventHandlerSignature {
                Event event;
                const char* methodName;
                int numArgs;
            };
            static const std::vector<EventHandlerSignature> eventHandlerSignatures;
            enum ConfigType {
                Int,
                Float,
                Bool,
                Color,
                Enum,
            };
            struct ConfigMetadata {
                void* ptr;
                std::string field;
                std::string jsonKey;
                std::string displayName;
                std::string helpText;
                ConfigType type;
                // hahaha unions you say? nay, i want the memory all for myself
                float floatMin = -100.0f;
                float floatMax = 100.0f;
                float floatStep = 1.0f;
                int intMin = -1024;
                int intMax = 1024;
                int intStep = 1;
                int enumNumElements = 0;
                std::unordered_map<int, std::string> enumDisplayNames;
                std::unordered_map<std::string, int> enumSerializedNames;
                void* uiElementPtr; // yuck yuck ew yuck
            };
            struct RegistryEntry {
                QountersMinus::Qounter* instance = nullptr;
                std::unordered_map<Event, const MethodInfo*> eventHandlers;
                std::string shortName;
                std::string longName;
                std::string configKey;
                std::vector<std::shared_ptr<ConfigMetadata>> configMetadata;
                std::unordered_map<std::string, void*> staticFieldRefs;
                bool isBaseQounter = false;
            };
            static std::unordered_map<std::pair<std::string, std::string>, RegistryEntry, pair_hash> registry;
            static std::vector<std::pair<std::string, std::string>> registryInsertionOrder;
            template <typename T>
            static void Register(std::string shortName, std::string longName, std::string configKey, bool isBaseQounter) {
                auto klass = classof(T*);
                std::unordered_map<Event, const MethodInfo*> eventHandlers;
                for (auto sig : eventHandlerSignatures) {
                    eventHandlers[sig.event] = il2cpp_functions::class_get_method_from_name(klass, sig.methodName, sig.numArgs);
                }
                std::unordered_map<std::string, void*> staticFieldRefs;
                if constexpr (!std::is_same_v<T, QountersMinus::Qounter>) {
                    staticFieldRefs = {
                        {"Enabled", &T::Enabled},
                        {"Position", &T::Position},
                        {"Distance", &T::Distance}
                    };
                }
                registry[{klass->namespaze, klass->name}] = {
                    .eventHandlers = eventHandlers,
                    .shortName = shortName,
                    .longName = longName,
                    .configKey = configKey,
                    .staticFieldRefs = staticFieldRefs,
                    .isBaseQounter = isBaseQounter
                };
                if constexpr (!std::is_same_v<T, QountersMinus::Qounter>) {
                    RegisterConfig<T>({
                        .ptr = staticFieldRefs["Enabled"],
                        .field = "Enabled",
                        .type = ConfigType::Bool,
                    });
                    RegisterConfig<T>({
                        .ptr = staticFieldRefs["Position"],
                        .field = "Position",
                        .type = ConfigType::Enum,
                        .enumNumElements = QounterPositionCount,
                        .enumDisplayNames = QounterPositionNames,
                        .enumSerializedNames = QounterPositionLookup,
                    });
                    RegisterConfig<T>({
                        .ptr = staticFieldRefs["Distance"],
                        .field = "Distance",
                        .type = ConfigType::Float,
                    });
                }
                registryInsertionOrder.push_back({klass->namespaze, klass->name});
            }

            template <typename T>
            static void Register(std::string shortName, std::string longName, std::string configKey) {
                Register<T>(shortName, longName, configKey, false);
            }

            template <typename T>
            static void RegisterConfig(ConfigMetadata config) {
                auto klass = classof(T*);
                auto ptr = std::make_shared<ConfigMetadata>(config);
                registry[{klass->namespaze, klass->name}].configMetadata.emplace_back(ptr);
            }

            static void Initialize();

            template <typename... TArgs>
            static void BroadcastEvent(Event event, TArgs&&... args) {
                for (auto def : registry) {
                    if (def.second.instance && def.second.eventHandlers[event]) {
                        il2cpp_utils::RunMethodUnsafe(def.second.instance, def.second.eventHandlers[event], args...);
                    }
                }
            }
    };
};
