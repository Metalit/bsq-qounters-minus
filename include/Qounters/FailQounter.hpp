#pragma once

#include "util/logger.hpp"
#include "util/song_id.hpp"
#include "Qounter.hpp"
#include "QounterRegistry.hpp"

#include "custom-types/shared/macros.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "GlobalNamespace/PlayerAllOverallStatsData.hpp"
#include "GlobalNamespace/PlayerAllOverallStatsData_PlayerOverallStatsData.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Time.hpp"
#include "System/Action.hpp"

DECLARE_CLASS_CUSTOM(QountersMinus::Qounters, FailQounter, QountersMinus::Qounter,
    static bool Enabled;
    static int Position;
    static float Distance;
    static bool ShowRestartsInstead;

    static std::string prevBeatmapHash;
    static int restarts;
    DECLARE_INSTANCE_FIELD_DEFAULT(int, count, 0);
    DECLARE_INSTANCE_FIELD_DEFAULT(float, animationTimer, 10.0f);

    DECLARE_STATIC_METHOD(void, Register);
    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, Update);
)
