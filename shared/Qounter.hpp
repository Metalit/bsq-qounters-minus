#pragma once

#include <map>
#include "util/logger.hpp"
#include "InjectedComponents.hpp"
#include "custom-types/shared/macros.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Resources.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

// define version for external consumers
#define QOUNTERS_MINUS_VERSION "1.1.3-r1"

namespace QountersMinus {
    enum class QounterPosition {
        BelowCombo,
        AboveCombo,
        BelowMultiplier,
        AboveMultiplier,
        BelowEnergy,
        AboveHighway
    };
    static int QounterPositionCount = 6;
    static std::unordered_map<int, std::string> QounterPositionNames = {
        {static_cast<int>(QounterPosition::BelowCombo), "Below Combo"},
        {static_cast<int>(QounterPosition::AboveCombo), "Above Combo"},
        {static_cast<int>(QounterPosition::BelowMultiplier), "Below Multiplier"},
        {static_cast<int>(QounterPosition::AboveMultiplier), "Above Multiplier"},
        {static_cast<int>(QounterPosition::BelowEnergy), "Below Energy"},
        {static_cast<int>(QounterPosition::AboveHighway), "Over Highway"}
    };
    static std::unordered_map<std::string, int> QounterPositionLookup = {
        {"BelowCombo", static_cast<int>(QounterPosition::BelowCombo)},
        {"AboveCombo", static_cast<int>(QounterPosition::AboveCombo)},
        {"BelowMultiplier", static_cast<int>(QounterPosition::BelowMultiplier)},
        {"AboveMultiplier", static_cast<int>(QounterPosition::AboveMultiplier)},
        {"BelowEnergy", static_cast<int>(QounterPosition::BelowEnergy)},
        {"AboveHighway", static_cast<int>(QounterPosition::AboveHighway)}
    };
}

UnityEngine::GameObject* GetParent(QountersMinus::QounterPosition position);
void SetPosition(UnityEngine::Transform* transform, QountersMinus::QounterPosition position, float distance);

DECLARE_CLASS_CODEGEN(QountersMinus, Qounter, UnityEngine::MonoBehaviour,
    static bool Enabled;
    static bool HideCombo;
    static bool HideMultiplier;
    static float ComboOffset;
    static float MultiplierOffset;
    static bool ItalicText;
    static bool UprightInMultiplayer;
    static bool DisableIn90Degree;
    static bool FixedHUDPosition;
    static float DistanceStep;

    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, gameObject);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, basicText);
    DECLARE_INSTANCE_FIELD(QountersMinus::InjectedComponents*, refs);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);

    public:
        TMPro::TextMeshProUGUI* CreateBasicTitle(std::string text);
        TMPro::TextMeshProUGUI* CreateBasicText(std::string text);
        static Qounter* Initialize(System::Type* type, QountersMinus::QounterPosition position, float distance);
)
