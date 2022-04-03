#include "Qounters/CutQounter.hpp"

DEFINE_TYPE(QountersMinus::Qounters, CutQounter);

using namespace QountersMinus;
using namespace GlobalNamespace;

bool Qounters::CutQounter::Enabled = false;
int Qounters::CutQounter::Position = static_cast<int>(QounterPosition::AboveHighway);
float Qounters::CutQounter::Distance = 1.0f;
bool Qounters::CutQounter::SeparateSaberCounts = false;
bool Qounters::CutQounter::SeparateCutValues = false;
int Qounters::CutQounter::AveragePrecision = 1;

void Qounters::CutQounter::Register() {
    QounterRegistry::Register<CutQounter>("Cut", "Cut Qounter", "CutConfig", true);
    QounterRegistry::RegisterConfig<CutQounter>({
        .ptr = &SeparateSaberCounts,
        .field = "SeparateSaberCounts",
        .displayName = "Separate Saber Cuts",
        .helpText = "Shows the average cut for the left and right sabers separately.",
        .type = QounterRegistry::ConfigType::Bool,
    });
    QounterRegistry::RegisterConfig<CutQounter>({
        .ptr = &SeparateCutValues,
        .field = "SeparateCutValues",
        .displayName = "Separate Cut Values",
        .helpText = "Show separate averages for angle before cut (0-70), angle after cut (0-30) and distance to center (0-15).",
        .type = QounterRegistry::ConfigType::Bool,
    });
    QounterRegistry::RegisterConfig<CutQounter>({
        .ptr = &AveragePrecision,
        .field = "AveragePrecision",
        .displayName = "Average Cut Precision",
        .helpText = "How many decimals should be shown on the average cuts?",
        .type = QounterRegistry::ConfigType::Int,
        .intMin = 0,
        .intMax = 4,
    });
}

void Qounters::CutQounter::Start() {
    CreateBasicTitle("Average Cut");

    auto defaultText = FormatNumber(0.0f, AveragePrecision);
    if (SeparateCutValues) {
        defaultText = defaultText + "\n" + defaultText + "\n" + defaultText;
    }
    auto fontSize = SeparateCutValues ? 28.0f : 35.0f;
    auto xOffset = SeparateSaberCounts ? 20.0f + (12.0f * AveragePrecision) : 0.0f;
    auto yOffset = SeparateCutValues ? -60.0f : -30.0f;

    leftCutText = CreateBasicText(defaultText);
    leftCutText->set_fontSize(fontSize);
    leftCutText->set_lineSpacing(-40.0f);
    leftCutText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(xOffset * -1.0f, yOffset));
    if (SeparateSaberCounts) {
        rightCutText = CreateBasicText(defaultText);
        rightCutText->set_fontSize(fontSize);
        rightCutText->set_lineSpacing(-40.0f);
        rightCutText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(xOffset, yOffset));
    }

    cutScores = il2cpp_utils::New<System::Collections::Generic::List_1<int>*>().value();
}

void Qounters::CutQounter::UpdateCutScores() {
    int leftBeforeSwingSum = 0, leftAfterSwingSum = 0, leftCutDistanceSum = 0, leftCount = 0,
        rightBeforeSwingSum = 0, rightAfterSwingSum = 0, rightCutDistanceSum = 0, rightCount = 0;
    auto cutScoresArr = cutScores->items;
    for (int i = 0, j = cutScores->get_Count(); i < j; i += 4) {
        if (cutScoresArr[i] == 0) {
            leftBeforeSwingSum += cutScoresArr[i + 1];
            leftAfterSwingSum += cutScoresArr[i + 2];
            leftCutDistanceSum += cutScoresArr[i + 3];
            leftCount += 1;
        } else {
            rightBeforeSwingSum += cutScoresArr[i + 1];
            rightAfterSwingSum += cutScoresArr[i + 2];
            rightCutDistanceSum += cutScoresArr[i + 3];
            rightCount += 1;
        }
    }
    if (leftCount == 0) leftCount++;
    if (rightCount == 0) rightCount++;

    if (SeparateCutValues) {
        if (SeparateSaberCounts) {
            leftCutText->set_text(
                FormatNumber((float)leftBeforeSwingSum / leftCount, AveragePrecision) + "\n" +
                FormatNumber((float)leftAfterSwingSum / leftCount, AveragePrecision) + "\n" +
                FormatNumber((float)leftCutDistanceSum / leftCount, AveragePrecision)
            );
            rightCutText->set_text(
                FormatNumber((float)rightBeforeSwingSum / rightCount, AveragePrecision) + "\n" +
                FormatNumber((float)rightAfterSwingSum / rightCount, AveragePrecision) + "\n" +
                FormatNumber((float)rightCutDistanceSum / rightCount, AveragePrecision)
            );
        } else {
            leftCutText->set_text(
                FormatNumber((float)(leftBeforeSwingSum + rightBeforeSwingSum) / (leftCount + rightCount), AveragePrecision) + "\n" +
                FormatNumber((float)(leftAfterSwingSum + rightAfterSwingSum) / (leftCount + rightCount), AveragePrecision) + "\n" +
                FormatNumber((float)(leftCutDistanceSum + rightCutDistanceSum) / (leftCount + rightCount), AveragePrecision)
            );
        }
    } else {
        if (SeparateSaberCounts) {
            leftCutText->set_text(
                FormatNumber((float)(leftBeforeSwingSum + leftAfterSwingSum + leftCutDistanceSum) / leftCount, AveragePrecision)
            );
            rightCutText->set_text(
                FormatNumber((float)(rightBeforeSwingSum + rightAfterSwingSum + rightCutDistanceSum) / rightCount, AveragePrecision)
            );
        } else {
            leftCutText->set_text(
                FormatNumber((float)(leftBeforeSwingSum + leftAfterSwingSum + leftCutDistanceSum + rightBeforeSwingSum + rightAfterSwingSum + rightCutDistanceSum) / (leftCount + rightCount), AveragePrecision)
            );
        }
    }
}

void Qounters::CutQounter::OnSwingRatingFinished(CutScoreBuffer* scoreBuffer) {
    cutScores->Add((int) scoreBuffer->noteCutInfo.saberType);
    cutScores->Add(scoreBuffer->get_beforeCutScore());
    cutScores->Add(scoreBuffer->get_afterCutScore());
    cutScores->Add(scoreBuffer->get_centerDistanceCutScore());
    UpdateCutScores();
}
