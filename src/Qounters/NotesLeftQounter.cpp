#include "Qounters/NotesLeftQounter.hpp"

DEFINE_TYPE(QountersMinus::Qounters,NotesLeftQounter);

bool QountersMinus::Qounters::NotesLeftQounter::Enabled = false;
int QountersMinus::Qounters::NotesLeftQounter::Position = static_cast<int>(QountersMinus::QounterPosition::AboveHighway);
float QountersMinus::Qounters::NotesLeftQounter::Distance = -1.0f;
bool QountersMinus::Qounters::NotesLeftQounter::LabelAboveCount = false;

void QountersMinus::Qounters::NotesLeftQounter::Register() {
    QounterRegistry::Register<NotesLeftQounter>("Notes Left", "Notes Left Qounter", "NotesLeftConfig", true);
    QounterRegistry::RegisterConfig<NotesLeftQounter>({
        .ptr = &LabelAboveCount,
        .field = "LabelAboveCount",
        .displayName = "Label Above Qounter",
        .helpText = "Put the label above the number, similar to a usual Qounters- qounter.",
        .type = QounterRegistry::ConfigType::Bool,
    });
}

void QountersMinus::Qounters::NotesLeftQounter::Start() {
    if (LabelAboveCount) CreateBasicTitle("Notes Remaining");
    CreateBasicText("");
    float yOffset = LabelAboveCount ? -30.0f : 0.0;
    float fontSize = LabelAboveCount ? 35.0f : 25.0f;
    basicText->set_fontSize(fontSize);
    basicText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0.0f, yOffset));

    notesLeft = GetNoteCount();
    UpdateValue();
}

void QountersMinus::Qounters::NotesLeftQounter::UpdateValue() {
    basicText->set_text((LabelAboveCount ? "" : "Notes Remaining: ") + std::to_string(notesLeft));
}

void QountersMinus::Qounters::NotesLeftQounter::OnNoteCut(GlobalNamespace::NoteData* data, GlobalNamespace::NoteCutInfo* info) {
    if (data->colorType != GlobalNamespace::ColorType::None) {
        notesLeft--;
        UpdateValue();
    }
}

void QountersMinus::Qounters::NotesLeftQounter::OnNoteMiss(GlobalNamespace::NoteData* data) {
    if (data->colorType != GlobalNamespace::ColorType::None) {
        notesLeft--;
        UpdateValue();
    }
}
