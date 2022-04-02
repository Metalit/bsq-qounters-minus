#include "Qounters/SpeedQounter.hpp"

DEFINE_TYPE(QountersMinus::Qounters,SpeedQounter);

bool QountersMinus::Qounters::SpeedQounter::Enabled = false;
int QountersMinus::Qounters::SpeedQounter::Position = static_cast<int>(QountersMinus::QounterPosition::BelowMultiplier);
float QountersMinus::Qounters::SpeedQounter::Distance = 2.0f;
int QountersMinus::Qounters::SpeedQounter::Mode = static_cast<int>(QountersMinus::SpeedQounterMode::Average);
bool QountersMinus::Qounters::SpeedQounter::SeparateCutValues = true;
int QountersMinus::Qounters::SpeedQounter::DecimalPrecision = 2;

void QountersMinus::Qounters::SpeedQounter::Register() {
    QounterRegistry::Register<SpeedQounter>("Speed", "Speed Qounter", "SpeedConfig", true);
    QounterRegistry::RegisterConfig<SpeedQounter>({
        .ptr = &DecimalPrecision,
        .field = "DecimalPrecision",
        .displayName = "Percentage Precision",
        .helpText = "How precise should the percentage be?",
        .type = QounterRegistry::ConfigType::Int,
        .intMin = 0,
        .intMax = 5,
    });
    QounterRegistry::RegisterConfig<SpeedQounter>({
        .ptr = &Mode,
        .field = "Mode",
        .helpText = "How should this Qounter display data?",
        .type = QounterRegistry::ConfigType::Enum,
        .enumNumElements = SpeedQounterModeCount,
        .enumDisplayNames = SpeedQounterModeNames,
        .enumSerializedNames = SpeedQounterModeLookup,
    });
}

float Max(System::Collections::Generic::List_1<float>* list) {
    float max = 0.0f;
    for (int i = 0; i < list->get_Count(); i++) {
        auto val = list->get_Item(i);
        if (val > max) max = val;
    }
    return max;
}
float Average(System::Collections::Generic::List_1<float>* list) {
    float sum = 0.0f;
    int count = list->get_Count();
    for (int i = 0; i < count; i++) {
        sum += list->get_Item(i);
    }
    return sum / count;
}

void QountersMinus::Qounters::SpeedQounter::Start() {
    if (Mode != static_cast<int>(QountersMinus::SpeedQounterMode::Top5Sec)) {
        CreateBasicTitle("Average Speed");

        averageText = QuestUI::BeatSaberUI::CreateText(gameObject->get_transform(), "0", false);
        averageText->set_alignment(TMPro::TextAlignmentOptions::Center);
        averageText->set_fontSize(35.0f);
        averageText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0.0f, -30.0f));
    }
    if (Mode != static_cast<int>(QountersMinus::SpeedQounterMode::Average) && Mode != static_cast<int>(QountersMinus::SpeedQounterMode::SplitAverage)) {
        auto titleText = CreateBasicTitle("Recent Top Speed");
        auto titleYOffset = 0.0f;
        auto textYOffset = -30.0f;
        if (Mode == static_cast<int>(QountersMinus::SpeedQounterMode::Both) || Mode == static_cast<int>(QountersMinus::SpeedQounterMode::SplitBoth)) {
            titleYOffset = -60.0f;
            textYOffset = -90.0f;
        }
        titleText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0.0f, titleYOffset));

        fastestText = QuestUI::BeatSaberUI::CreateText(gameObject->get_transform(), "0", false);
        fastestText->set_alignment(TMPro::TextAlignmentOptions::Center);
        fastestText->set_fontSize(35.0f);
        fastestText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0.0f, textYOffset));
    }

    leftSpeeds = il2cpp_utils::New<System::Collections::Generic::List_1<float>*>().value();
    rightSpeeds = il2cpp_utils::New<System::Collections::Generic::List_1<float>*>().value();
    fastestSpeeds = il2cpp_utils::New<System::Collections::Generic::List_1<float>*>().value();
}

void QountersMinus::Qounters::SpeedQounter::Update() {
    auto dt = UnityEngine::Time::get_deltaTime();
    lastUpdated += dt;
    if (lastUpdated > 0.25f) {
        lastUpdated = 0.0f;
        switch (static_cast<QountersMinus::SpeedQounterMode>(Mode)) {
            case QountersMinus::SpeedQounterMode::Average:
                rightSpeeds->Add((refs->saberManager->rightSaber->get_bladeSpeed() + refs->saberManager->leftSaber->get_bladeSpeed()) / 2.0f);
                averageText->set_text(FormatNumber(Average(rightSpeeds), DecimalPrecision));
                break;
            case QountersMinus::SpeedQounterMode::Top5Sec:
                fastestSpeeds->Add((refs->saberManager->rightSaber->get_bladeSpeed() + refs->saberManager->leftSaber->get_bladeSpeed()) / 2.0f);
                break;
            case QountersMinus::SpeedQounterMode::Both:
                fastestSpeeds->Add((refs->saberManager->rightSaber->get_bladeSpeed() + refs->saberManager->leftSaber->get_bladeSpeed()) / 2.0f);
                rightSpeeds->Add((refs->saberManager->rightSaber->get_bladeSpeed() + refs->saberManager->leftSaber->get_bladeSpeed()) / 2.0f);
                averageText->set_text(FormatNumber(Average(rightSpeeds), DecimalPrecision));
                break;
            case QountersMinus::SpeedQounterMode::SplitAverage:
                rightSpeeds->Add(refs->saberManager->rightSaber->get_bladeSpeed());
                leftSpeeds->Add(refs->saberManager->leftSaber->get_bladeSpeed());
                averageText->set_text(FormatNumber(Average(leftSpeeds), DecimalPrecision) + "   " + FormatNumber(Average(rightSpeeds), DecimalPrecision));
                break;
            case QountersMinus::SpeedQounterMode::SplitBoth:
                fastestSpeeds->Add((refs->saberManager->rightSaber->get_bladeSpeed() + refs->saberManager->leftSaber->get_bladeSpeed()) / 2.0f);
                rightSpeeds->Add(refs->saberManager->rightSaber->get_bladeSpeed());
                leftSpeeds->Add(refs->saberManager->leftSaber->get_bladeSpeed());
                averageText->set_text(FormatNumber(Average(leftSpeeds), DecimalPrecision) + "   " + FormatNumber(Average(rightSpeeds), DecimalPrecision));
                break;
        }
    }

    lastUpdatedFastest += dt;
    if (lastUpdatedFastest >= 5.0f) {
        lastUpdatedFastest = 0.0f;
        if (fastestText) {
            auto top = Max(fastestSpeeds);
            fastestSpeeds->Clear();
            fastestText->set_text(FormatNumber(top, DecimalPrecision));
        }
    }
}
