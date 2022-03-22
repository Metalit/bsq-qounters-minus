#include "Qounters/MissedQounter.hpp"

DEFINE_TYPE(QountersMinus::Qounters,MissedQounter);

bool QountersMinus::Qounters::MissedQounter::Enabled = true;
int QountersMinus::Qounters::MissedQounter::Position = static_cast<int>(QountersMinus::QounterPosition::BelowCombo);
float QountersMinus::Qounters::MissedQounter::Distance = 0.0f;
bool QountersMinus::Qounters::MissedQounter::CountBadCuts = true;

void QountersMinus::Qounters::MissedQounter::Register() {
    QounterRegistry::Register<MissedQounter>("Missed", "Missed Qounter", "MissedConfig", true);
    QounterRegistry::RegisterConfig<MissedQounter>({
        .ptr = &CountBadCuts,
        .field = "CountBadCuts",
        .displayName = "Include Bad Cuts",
        .helpText = "Bad cuts count towards the Missed counter.",
        .type = QounterRegistry::ConfigType::Bool,
    });
}

void QountersMinus::Qounters::MissedQounter::Start() {
    CreateBasicTitle("Misses");
    CreateBasicText("0");
}

void QountersMinus::Qounters::MissedQounter::UpdateValue() {
    basicText->set_text(std::to_string(misses));
}

void QountersMinus::Qounters::MissedQounter::OnNoteCut(GlobalNamespace::NoteData* data, GlobalNamespace::NoteCutInfo* info) {
    if (CountBadCuts && !info->get_allIsOK() && data->colorType != GlobalNamespace::ColorType::None) {
        misses++;
        UpdateValue();
    }
}

void QountersMinus::Qounters::MissedQounter::OnNoteMiss(GlobalNamespace::NoteData* data) {
    if (data->colorType != GlobalNamespace::ColorType::None) {
        misses++;
        UpdateValue();
    }
}
