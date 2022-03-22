#include "Qounters/NotesQounter.hpp"

DEFINE_TYPE(QountersMinus::Qounters,NotesQounter);

bool QountersMinus::Qounters::NotesQounter::Enabled = false;
int QountersMinus::Qounters::NotesQounter::Position = static_cast<int>(QountersMinus::QounterPosition::BelowCombo);
float QountersMinus::Qounters::NotesQounter::Distance = 1.0f;
bool QountersMinus::Qounters::NotesQounter::ShowPercentage = false;
int QountersMinus::Qounters::NotesQounter::DecimalPrecision = 2;

void QountersMinus::Qounters::NotesQounter::Register() {
    QounterRegistry::Register<NotesQounter>("Note", "Note Qounter", "NoteConfig", true);
    QounterRegistry::RegisterConfig<NotesQounter>({
        .ptr = &ShowPercentage,
        .field = "ShowPercentage",
        .displayName = "Show Percentage",
        .helpText = "Toggles the percentage of notes hit over total notes.",
        .type = QounterRegistry::ConfigType::Bool,
    });
    QounterRegistry::RegisterConfig<NotesQounter>({
        .ptr = &DecimalPrecision,
        .field = "DecimalPrecision",
        .displayName = "Percentage Precision",
        .helpText = "How precise should the percentage be?",
        .type = QounterRegistry::ConfigType::Int,
        .intMin = 0,
        .intMax = 5,
    });
}

void QountersMinus::Qounters::NotesQounter::Start() {
    CreateBasicTitle("Notes");

    std::string defaultText = "0";
    if (ShowPercentage) defaultText += " - " + FormatNumber(100.0f, DecimalPrecision) + "%";
    CreateBasicText(defaultText);
}

void QountersMinus::Qounters::NotesQounter::UpdateValue() {
    auto text = std::to_string(goodCuts) + "/" + std::to_string(allCuts);
    if (ShowPercentage) {
        text += " - " + FormatNumber((100.0f * goodCuts) / allCuts, DecimalPrecision) + "%";
    }
    basicText->set_text(text);
}

void QountersMinus::Qounters::NotesQounter::OnNoteCut(GlobalNamespace::NoteData* data, GlobalNamespace::NoteCutInfo* info) {
    allCuts++;
    if (info->get_allIsOK() && data->colorType != GlobalNamespace::ColorType::None) {
        goodCuts++;
    }
    UpdateValue();
}

void QountersMinus::Qounters::NotesQounter::OnNoteMiss(GlobalNamespace::NoteData* data) {
    if (data->colorType != GlobalNamespace::ColorType::None) {
        allCuts++;
        UpdateValue();
    }
}
