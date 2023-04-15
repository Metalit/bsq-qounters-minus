#include "QounterRegistry.hpp"

using namespace QountersMinus;

const std::vector<QounterRegistry::EventHandlerSignature> QounterRegistry::eventHandlerSignatures = {
    {QounterRegistry::Event::NoteCut, "OnNoteCut", 2},
    {QounterRegistry::Event::NoteMiss, "OnNoteMiss", 1},
    {QounterRegistry::Event::ScoreUpdated, "OnScoreUpdated", 1},
    // {QounterRegistry::Event::MaxScoreUpdated, "OnMaxScoreUpdated", 1},
    {QounterRegistry::Event::SwingRatingFinished, "OnSwingRatingFinished", 1},
};
std::unordered_map<std::pair<std::string, std::string>, QounterRegistry::RegistryEntry, pair_hash> QounterRegistry::registry;
std::vector<std::pair<std::string, std::string>> QounterRegistry::registryInsertionOrder;

void _DeactivateChildren(UnityEngine::GameObject* gameObject) {
    auto parent = gameObject->get_transform();
    for (int i = 0; i < parent->get_childCount(); i++) {
        parent->GetChild(i)->get_gameObject()->SetActive(false);
    }
}

void _DeactivateChildren(std::string gameObjectName) {
    _DeactivateChildren(UnityEngine::GameObject::Find(gameObjectName));
}


void logScales(UnityEngine::GameObject* go, std::string path="") {
    auto name = path + "/" + static_cast<std::string>(go->get_name());
    auto scale = go->get_transform()->get_localScale();
    auto pos = go->get_transform()->get_localPosition();
    auto rot = go->get_transform()->get_localRotation().get_eulerAngles();
    LOG_DEBUG(name + " position: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
    LOG_DEBUG("  rotation: %.2f, %.2f, %.2f", rot.x, rot.y, rot.z);
    LOG_DEBUG("  scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);

    if (go->get_transform()->get_parent()) {
        logScales(go->get_transform()->get_parent()->get_gameObject(), name);
    }

    // for (int i = 0; i < go->get_transform()->get_childCount(); i++) {
    //     logScales(go->get_transform()->GetChild(i)->get_gameObject(), name);
    // }
}

void QountersMinus::QounterRegistry::Initialize() {
    for (auto&& def : registry) def.second.instance = nullptr;
    if (!Qounter::Enabled) return;
    auto playerData = UnityEngine::Object::FindObjectOfType<PlayerDataModel*>();
    if (playerData->playerData->playerSpecificSettings->noTextsAndHuds) return;
    if (playerData->playerData->gameplayModifiers->zenMode) return;

    auto comboPanel = UnityEngine::GameObject::Find("ComboPanel");
    auto scoreCanvas = UnityEngine::GameObject::Find("ScoreCanvas");
    auto multiplierCanvas = UnityEngine::GameObject::Find("MultiplierCanvas");
    auto progressCanvas = UnityEngine::GameObject::Find("SongProgressCanvas");

    // hacky way of getting BeatmapCharacteristic
    auto refs = comboPanel->AddComponent<QountersMinus::InjectedComponents*>();
    bool hasRotations = refs->beatmapCharacteristic->containsRotationEvents;

    // also kind of hacky imo
    bool inMultiplayer = UnityEngine::Object::FindObjectOfType<GlobalNamespace::MultiplayerLocalActivePlayerFacade*>() != nullptr;

    if (Qounter::DisableIn90Degree && hasRotations) {
        UnityEngine::Object::Destroy(refs);
        return;
    }

    if (Qounter::FixedHUDPosition && !hasRotations && !inMultiplayer) {
        auto energyBar = refs->coreGameHUDController->energyPanelGO->get_transform();
        auto energyPos = energyBar->get_position();
        auto leftPanel = comboPanel->get_transform()->GetParent();
        auto rightPanel = multiplierCanvas->get_transform()->GetParent();
        auto guiParent = leftPanel->GetParent();
        guiParent->SetPositionAndRotation({0, 0, 0}, UnityEngine::Quaternion::get_identity());
        guiParent->set_localScale({1, 1, 1});
        auto lossyScale = guiParent->get_lossyScale();
        guiParent->set_localScale({1/lossyScale.x, 1/lossyScale.y, 1/lossyScale.z});
        float xDist = guiParent->get_name() == "NarrowGameHUD" ? 2 : 3.2;
        leftPanel->set_localPosition({-xDist, 0.4, 7});
        rightPanel->set_localPosition({xDist, 0.4, 7});
        comboPanel->get_transform()->set_localPosition({0, 1.43, 0});
        scoreCanvas->get_transform()->set_localPosition({0, 0.4, 0});
        multiplierCanvas->get_transform()->set_localPosition({0, 1.3, 0});
        progressCanvas->get_transform()->set_localPosition({0, 0.4, 0});
        energyBar->set_position(energyPos);
    }

    if (Qounter::HideCombo)
        _DeactivateChildren(comboPanel);
    else if (Qounter::ItalicText) {
        for (auto& text : comboPanel->GetComponentsInChildren<TMPro::TextMeshProUGUI*>())
            text->set_fontStyle(TMPro::FontStyles::Italic);
    }
    if (Qounter::HideMultiplier) {
        multiplierCanvas->GetComponent<UnityEngine::Animator*>()->set_enabled(false);
        _DeactivateChildren(multiplierCanvas);
    }
    if (Qounter::UprightInMultiplayer && inMultiplayer) {
        UnityEngine::Transform* transforms[4] = {
            comboPanel->get_transform(),
            scoreCanvas->get_transform(),
            multiplierCanvas->get_transform(),
            progressCanvas->get_transform()
        };
        for (auto transform : transforms)
            transform->set_rotation(UnityEngine::Quaternion::get_identity());
        transforms[0]->set_localPosition({-2, 1.8, 7});
        transforms[1]->set_localPosition({-2, 0.9, 7});
        transforms[2]->set_localPosition({2, 1.8, 7});
        transforms[3]->set_localPosition({2, 0.9, 7});
        // enable texts
        auto scoreUIText = refs->coreGameHUDController->GetComponentInChildren<GlobalNamespace::ScoreUIController*>()->scoreText;
        auto relativeScoreText = refs->coreGameHUDController->relativeScoreGO->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
        auto rankText = refs->coreGameHUDController->immediateRankGO->GetComponent<TMPro::TextMeshProUGUI*>();
        scoreUIText->set_enabled(true);
        relativeScoreText->set_enabled(true);
        rankText->set_enabled(true);
    }

    for (auto key : registryInsertionOrder) {
        auto&& def = registry[key];
        if (key.first == "QountersMinus" && key.second == "Qounter") continue; // there's got to be a better way
        auto enabled = *(bool*)def.staticFieldRefs["Enabled"];
        auto position = *(int*)def.staticFieldRefs["Position"];
        auto distance = *(float*)def.staticFieldRefs["Distance"];
        if (!enabled) continue;
        LOG_DEBUG("Initialize " + key.first + "::" + key.second);
        auto systemType = il2cpp_utils::GetSystemType(key.first, key.second);
        def.instance = QountersMinus::Qounter::Initialize(systemType, static_cast<QountersMinus::QounterPosition>(position), distance);
    }
}
