#include "Qounters/PBQounter.hpp"

DEFINE_TYPE(QountersMinus::Qounters,PBQounter);

bool QountersMinus::Qounters::PBQounter::Enabled = true;
int QountersMinus::Qounters::PBQounter::Position = static_cast<int>(QountersMinus::QounterPosition::BelowMultiplier);
float QountersMinus::Qounters::PBQounter::Distance = 1.0f;
int QountersMinus::Qounters::PBQounter::Mode = static_cast<int>(QountersMinus::PBQounterMode::Absolute);
UnityEngine::Color QountersMinus::Qounters::PBQounter::BetterColor = UnityEngine::Color(1.0f, 0.0f, 0.0f, 1.0f);
UnityEngine::Color QountersMinus::Qounters::PBQounter::DefaultColor = UnityEngine::Color(1.0f, 0.647f, 0.0f, 1.0f);
int QountersMinus::Qounters::PBQounter::DecimalPrecision = 2;
int QountersMinus::Qounters::PBQounter::TextSize = 2;
bool QountersMinus::Qounters::PBQounter::UnderScore = true;
bool QountersMinus::Qounters::PBQounter::HideFirstScore = false;

void QountersMinus::Qounters::PBQounter::Register() {
    QounterRegistry::Register<PBQounter>("Personal Best", "PB Qounter", "PBConfig", true);
    QounterRegistry::RegisterConfig<PBQounter>({
        .ptr = &Mode,
        .field = "Mode",
        .helpText = "Change color based on absolute (song maximum) or relative (current maximum) score.",
        .type = QounterRegistry::ConfigType::Enum,
        .enumNumElements = PBQounterModeCount,
        .enumDisplayNames = PBQounterModeNames,
        .enumSerializedNames = PBQounterModeLookup,
    });
    QounterRegistry::RegisterConfig<PBQounter>({
        .ptr = &BetterColor,
        .field = "BetterColor",
        .displayName = "Better Color",
        .type = QounterRegistry::ConfigType::Color,
    });
    QounterRegistry::RegisterConfig<PBQounter>({
        .ptr = &DefaultColor,
        .field = "DefaultColor",
        .displayName = "Default Color",
        .type = QounterRegistry::ConfigType::Color,
    });
    QounterRegistry::RegisterConfig<PBQounter>({
        .ptr = &DecimalPrecision,
        .field = "DecimalPrecision",
        .displayName = "Percentage Precision",
        .helpText = "How precise should the percentage be?",
        .type = QounterRegistry::ConfigType::Int,
        .intMin = 0,
        .intMax = 5,
    });
    QounterRegistry::RegisterConfig<PBQounter>({
        .ptr = &TextSize,
        .field = "TextSize",
        .displayName = "Text Size",
        .helpText = "How large should the text be?",
        .type = QounterRegistry::ConfigType::Int,
        .intMin = 1,
        .intMax = 10,
    });
    QounterRegistry::RegisterConfig<PBQounter>({
        .ptr = &UnderScore,
        .field = "UnderScore",
        .displayName = "Below Score Qounter",
        .helpText = "Will the Personal Best counter be positioned below the Score Qounter instead of based on the position settings?",
        .type = QounterRegistry::ConfigType::Bool,
    });
    QounterRegistry::RegisterConfig<PBQounter>({
        .ptr = &HideFirstScore,
        .field = "HideFirstScore",
        .displayName = "Hide First Score",
        .helpText = "Hides Personal Best if you play a map that doesnt yet have a personal best set.",
        .type = QounterRegistry::ConfigType::Bool,
    });
}

void QountersMinus::Qounters::PBQounter::Start() {
    int maxRawScore = GetMaxMultipliedScore();

    maxPossibleScore = GlobalNamespace::ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(
        maxRawScore,
        refs->scoreController->gameplayModifiersModel->GetTotalMultiplier(
            refs->scoreController->gameplayModifierParams,
            refs->scoreController->gameEnergyCounter->get_energy()
        )
    );
    auto playerLevelStats = refs->playerData->GetPlayerLevelStatsData(refs->difficultyBeatmap);
    highScore = playerLevelStats->highScore;

    pbText = CreateBasicTitle("");
    pbText->set_alignment(TMPro::TextAlignmentOptions::Top);
    pbText->set_fontSize(TextSize * 10.0f);

    if (UnderScore) {
        auto scoreUIController = UnityEngine::Object::FindObjectOfType<GlobalNamespace::ScoreUIController*>();
        gameObject->get_transform()->SetParent(scoreUIController->scoreText->get_transform());
        gameObject->get_transform()->set_localPosition(UnityEngine::Vector3(0, -30, 0));
    }

    SetPersonalBest((float) highScore / maxPossibleScore);
    OnScoreUpdated(0);
}

void QountersMinus::Qounters::PBQounter::SetPersonalBest(float ratioOfMaxScore) {
    if (HideFirstScore && highScore == 0) {
        pbText->set_text("PB: --");
    } else {
        pbText->set_text("PB: " + FormatNumber(ratioOfMaxScore * 100.0f, DecimalPrecision) + "%");
    }
}

void QountersMinus::Qounters::PBQounter::OnScoreUpdated(int modifiedScore) {
    if (maxPossibleScore != 0) {
        if (modifiedScore > highScore) {
            SetPersonalBest(modifiedScore / (float)maxPossibleScore);
        }
    }

    if (Mode == static_cast<int>(PBQounterMode::Relative)) {
        if (refs->relativeScoreAndImmediateRankCounter->relativeScore > ((float)highScore / maxPossibleScore)) {
            pbText->set_color(BetterColor);
        } else {
            pbText->set_color(DefaultColor);
        }
    } else {
        if (modifiedScore > highScore) {
            if (!(HideFirstScore && highScore == 0)) {
                pbText->set_color(BetterColor);
            }
        } else {
            pbText->set_color(UnityEngine::Color::Lerp(
                UnityEngine::Color::get_white(),
                DefaultColor,
                (float)modifiedScore / (highScore == 0 ? 1 : highScore)
            ));
        }
    }
}
