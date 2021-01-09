#include "QounterSettingsViewController.hpp"

extern QountersMinus::ModConfig config;

DEFINE_CLASS(QountersMinus::QounterSettingsViewController);

#include "Polyglot/LocalizedTextMeshProUGUI.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "HMUI/DropdownWithTableView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "GlobalNamespace/NoteJumpStartBeatOffsetDropdown.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"
#include "System/Action_2.hpp"

// HMUI::SimpleTextDropdown* CreateTextDropdown(
//     UnityEngine::Transform* parent,
//     std::string text,
//     std::vector<std::string> values,
//     int selectedIndex,
//     UnityEngine::Vector2 anchoredPosition,
//     System::Action_2<HMUI::DropdownWithTableView*, int>* onChange
// ) {
//     auto template = QuestUI::ArrayUtil::First(
//         QuestUI::ArrayUtil::Select<UnityEngine::GameObject*>(
//             UnityEngine::Resources::FindObjectsOfTypeAll<HMUI::SimpleTextDropdown*>(),
//             [](HMUI::SimpleTextDropdown* x) { return x->get_transform()->get_parent()->get_gameObject(); }
//         ),
//         [](UnityEngine::GameObject* x) { return to_utf8(csstrtostr(x->get_name())) == "NJBSO"; }
//     );
//     auto gameObject = UnityEngine::Object::Instantiate(template, parent, false);
//     static auto name = il2cpp_utils::createcsstr("QountersMinusTextDropdown", il2cpp_utils::StringType::Manual);
//     gameObject->set_name(name);
//     gameObject->SetActive(false);

//     auto labelText = gameObject->get_transform()->Find(il2cpp_utils::createcsstr("Label"))->get_gameObject();
//     UnityEngine::Object::Destroy(labelText->GetComponent<Polyglot::LocalizedTextMeshProUGUI*>());
//     TMPro::TextMeshProUGUI* textMesh = labelText->GetComponent<TMPro::TextMeshProUGUI*>();
//     textMesh->SetText(il2cpp_utils::createcsstr(text));
//     textMesh->set_richText(true);
    
//     UnityEngine::Object::Destroy(gameObject->GetComponentInChildren<GlobalNamespace::NoteJumpStartBeatOffsetDropdown*>());
//     auto dropdown = gameObject->GetComponentInChildren<HMUI::SimpleTextDropdown*>();
//     // toggle->set_interactable(true);
//     // toggle->set_isOn(currentValue);
//     // dropdown->

//     // toggle->onValueChanged = Toggle::ToggleEvent::New_ctor();
//     // if(onToggle)
//     //     toggle->onValueChanged->AddListener(onToggle);
    
//     auto rectTransform = gameObject->GetComponent<UnityEngine::RectTransform*>();
//     rectTransform->set_anchoredPosition(anchoredPosition);
//     // LayoutElement* layout = gameObject->GetComponent<LayoutElement*>();
//     // layout->set_preferredWidth(90.0f);
//     gameObject->SetActive(true);
//     return dropdown;
// }

#include "UnityEngine/RectOffset.hpp"
#include "logger.hpp"

void QountersMinus::QounterSettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (!firstActivation || !addedToHierarchy) return;

    // auto scrollContainer = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(get_transform());
    auto layout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_transform());
    layout->set_spacing(0);
    layout->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("round-rect-panel"));
    layout->set_padding(UnityEngine::RectOffset::New_ctor(2, 2, 2, 2));

    // bool hideCombo = false;
    // bool hideMultiplier = false;
    // float comboOffset = 0.2f; // ?
    // float multiplierOffset = 0.4f; // ?
    // bool italicText = false;

    // Qounter-specific configuration [ALL-QOUNTERS]
    auto cutQounterTitle = QuestUI::BeatSaberUI::CreateText(layout->get_transform(), "Cut Qounter");
    cutQounterTitle->set_alignment(TMPro::TextAlignmentOptions::Center);
    cutQounterTitle->set_fontSize(12.0f);
    auto cutQounterEnabled = QuestUI::BeatSaberUI::CreateToggle(layout->get_transform(), "Enabled", config.cutQounterConfig.enabled, il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
        classof(UnityEngine::Events::UnityAction_1<bool>*), this, +[](QountersMinus::QounterSettingsViewController* self, bool val) {
            LOG_DEBUG("SET config.cutQounterConfig.enabled = %d", val);
            config.cutQounterConfig.enabled = val;
            SaveConfig();
        }
    ));
    auto cutQounterSeparateSaberCounts = QuestUI::BeatSaberUI::CreateToggle(layout->get_transform(), "Separate Saber Counts", config.cutQounterConfig.separateSaberCounts, il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
        classof(UnityEngine::Events::UnityAction_1<bool>*), this, +[](QountersMinus::QounterSettingsViewController* self, bool val) {
            LOG_DEBUG("SET config.cutQounterConfig.separateSaberCounts = %d", val);
            config.cutQounterConfig.separateSaberCounts = val;
            SaveConfig();
        }
    ));
    auto cutQounterSeparateCutValues = QuestUI::BeatSaberUI::CreateToggle(layout->get_transform(), "Separate Cut Values", config.cutQounterConfig.separateCutValues, il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
        classof(UnityEngine::Events::UnityAction_1<bool>*), this, +[](QountersMinus::QounterSettingsViewController* self, bool val) {
            LOG_DEBUG("SET config.cutQounterConfig.separateCutValues = %d", val);
            config.cutQounterConfig.separateCutValues = val;
            SaveConfig();
        }
    ));
    auto cutQounterAveragePrecision = QuestUI::BeatSaberUI::CreateIncrementSetting(layout->get_transform(), "Average Precision", 0, 1.0f, config.cutQounterConfig.averagePrecision, il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(
        classof(UnityEngine::Events::UnityAction_1<float>*), this, +[](QountersMinus::QounterSettingsViewController* self, float val) {
            LOG_DEBUG("SET config.cutQounterConfig.averagePrecision = %d", (int)val);
            config.cutQounterConfig.averagePrecision = (int)val;
            SaveConfig();
        }
    ));
}

void QountersMinus::QounterSettingsViewController::Register() {
    custom_types::Register::RegisterType<QountersMinus::QounterSettingsViewController>();
    QuestUI::Register::RegisterModSettingsViewController<QountersMinus::QounterSettingsViewController*>(
        (ModInfo){"Qounters-", VERSION}, "Qounters-"
    );
}
