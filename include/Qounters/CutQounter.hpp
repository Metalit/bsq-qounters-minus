#pragma once

#include "util/logger.hpp"
#include "util/format.hpp"
#include "Qounter.hpp"
#include "QounterRegistry.hpp"

#include "custom-types/shared/macros.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "System/Collections/Generic/List_1.hpp"

DECLARE_CLASS_CUSTOM(QountersMinus::Qounters, CutQounter, QountersMinus::Qounter,
    static bool Enabled;
    static int Position;
    static float Distance;
    static bool SeparateSaberCounts;
    static bool SeparateCutValues;
    static int AveragePrecision;

    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, leftCutText);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, rightCutText);
    DECLARE_INSTANCE_FIELD(System::Collections::Generic::List_1<int>*, cutScores);

    DECLARE_STATIC_METHOD(void, Register);
    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, OnSwingRatingFinished, GlobalNamespace::CutScoreBuffer* scoreBuffer);
    DECLARE_INSTANCE_METHOD(void, UpdateCutScores);
)
