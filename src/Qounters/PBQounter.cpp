#include "Qounters/PBQounter.hpp"

DEFINE_CLASS(QountersMinus::Qounters::PBQounter);

void QountersMinus::Qounters::PBQounter::Configure(QountersMinus::PBQounterConfig config) {
    mode = (int)config.mode;
    decimalPrecision = config.decimalPrecision;
    underscore = config.underscore;
    hideFirstScore = config.hideFirstScore;
    betterColor = config.betterColor;
    defaultColor = config.defaultColor;

    int noteCount = GetNoteCount();
    int maxRawScore = GlobalNamespace::ScoreModel::MaxRawScoreForNumberOfNotes(noteCount);

    auto gameplayCoreInstaller = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameplayCoreInstaller*>()->values[0];
    auto playerDataModel = UnityEngine::Object::FindObjectOfType<GlobalNamespace::PlayerDataModel*>();
    auto scoreController = UnityEngine::Object::FindObjectOfType<GlobalNamespace::ScoreController*>();
    relativeScoreAndImmediateRankCounter = UnityEngine::Object::FindObjectOfType<GlobalNamespace::RelativeScoreAndImmediateRankCounter*>();

    maxPossibleScore = GlobalNamespace::ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(
        maxRawScore,
        scoreController->gameplayModifiersModel->GetTotalMultiplier(scoreController->gameplayModifiers)
    );
    auto playerLevelStats = playerDataModel->playerData->GetPlayerLevelStatsData(gameplayCoreInstaller->sceneSetupData->difficultyBeatmap);
    highScore = playerLevelStats->highScore;

    // bool scoreConfigEnabled = underscore; // hmmmm how to access this
    // if (scoreConfigEnabled && underscore) {
    //     HUDCanvas scoreCanvas = CanvasUtility.GetCanvasSettingsFromID(scoreConfig.CanvasID);
    //     counter = CanvasUtility.CreateTextFromSettings(scoreConfig, SCORE_COUNTER_OFFSET * (3f / scoreCanvas.PositionScale));
    // }
    // else
    // {
    //     counter = CanvasUtility.CreateTextFromSettings(Settings);
    // }
    pbText = QuestUI::BeatSaberUI::CreateText(gameObject->get_transform(), "", false);
    pbText->set_alignment(TMPro::TextAlignmentOptions::Top);
    pbText->set_fontSize(config.textSize * 10.0f);
    pbText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0.0f, 0.0f));

    SetPersonalBest((float)highScore / maxPossibleScore);
    OnScoreUpdated(0);
}

void QountersMinus::Qounters::PBQounter::SetPersonalBest(float ratioOfMaxScore) {
    if (hideFirstScore && highScore == 0) {
        pbText->set_text(il2cpp_utils::createcsstr("PB: --"));
    } else {
        pbText->set_text(il2cpp_utils::createcsstr("PB: " + FormatNumber(ratioOfMaxScore * 100.0f, decimalPrecision) + "%"));
    }
}

void QountersMinus::Qounters::PBQounter::OnScoreUpdated(int modifiedScore) {
    if (maxPossibleScore != 0) {
        if (modifiedScore > highScore) {
            SetPersonalBest(modifiedScore / (float)maxPossibleScore);
        }
    }

    if (mode == (int)PBQounterMode::Relative) {
        if (relativeScoreAndImmediateRankCounter->relativeScore > ((float)highScore / maxPossibleScore)) {
            pbText->set_color(betterColor);
        } else {
            pbText->set_color(defaultColor);
        }
    } else {
        if (modifiedScore > highScore) {
            if (!(hideFirstScore && highScore == 0)) {
                pbText->set_color(betterColor);
            }
        } else {
            pbText->set_color(UnityEngine::Color::Lerp(
                UnityEngine::Color::get_white(),
                defaultColor,
                modifiedScore / (float)highScore
            ));
        }
    }
}
