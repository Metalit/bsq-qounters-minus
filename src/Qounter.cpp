#include "Qounter.hpp"

DEFINE_TYPE(QountersMinus, Qounter);

bool QountersMinus::Qounter::Enabled = true;
bool QountersMinus::Qounter::HideCombo = false;
bool QountersMinus::Qounter::HideMultiplier = false;
float QountersMinus::Qounter::ComboOffset = 0.2f;
float QountersMinus::Qounter::MultiplierOffset = 0.4f;
bool QountersMinus::Qounter::ItalicText = false;
bool QountersMinus::Qounter::UprightInMultiplayer = true;
bool QountersMinus::Qounter::DisableIn90Degree = false;
bool QountersMinus::Qounter::FixedHUDPosition = true;
float QountersMinus::Qounter::DistanceStep = 1.0f;

const float distanceUnit = 40.0f;
const float distanceUnitOffsetMult = 0.15f;

struct QounterPositionData {
    std::string parentName;
    UnityEngine::Vector2 anchoredPosition;
    bool distanceIsDown;
};

std::unordered_map<QountersMinus::QounterPosition, QounterPositionData> QounterPositions = {
    {QountersMinus::QounterPosition::BelowCombo, {"ComboPanel", UnityEngine::Vector2(-4.0f, 12.0f), true}},
    {QountersMinus::QounterPosition::AboveCombo, {"ComboPanel", UnityEngine::Vector2(-4.0f, 40.0f), false}},
    {QountersMinus::QounterPosition::BelowMultiplier, {"MultiplierCanvas", UnityEngine::Vector2(0.0f, 12.0f), true}},
    {QountersMinus::QounterPosition::AboveMultiplier, {"MultiplierCanvas", UnityEngine::Vector2(0.0f, 40.0f), false}},
    {QountersMinus::QounterPosition::BelowEnergy, {"ComboPanel", UnityEngine::Vector2(0.0f, -200.0f), true}},
    {QountersMinus::QounterPosition::AboveHighway, {"ComboPanel", UnityEngine::Vector2(0.0f, 110.0f), false}}
};

void DeactivateChildren(UnityEngine::GameObject* gameObject) {
    auto parent = gameObject->get_transform();
    for (int i = 0; i < parent->get_childCount(); i++) {
        parent->GetChild(i)->get_gameObject()->SetActive(false);
    }
}

void DeactivateChildren(std::string gameObjectName) {
    DeactivateChildren(UnityEngine::GameObject::Find(gameObjectName));
}

// super slow way to find inactive game objects
// sometimes, the position parent objects are not active immediately after CoreGameHUD.Start
UnityEngine::GameObject* GetGameObject(std::string name) {
    auto allObjects = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();
    for (auto& object : allObjects) {
        if (object->get_name() == name) return object;
    }
    return nullptr;
}

UnityEngine::GameObject* GetParent(QountersMinus::QounterPosition position) {
    auto containerName = "QountersMinus_Container" + std::to_string((int) position);
    auto containerGO = UnityEngine::GameObject::Find(containerName);
    if (!containerGO) {
        // TODO: this is slow
        auto coreGameHUDController = UnityEngine::Object::FindObjectOfType<GlobalNamespace::CoreGameHUDController*>();
        auto parentGO = coreGameHUDController->get_gameObject()->Find(QounterPositions[position].parentName);
        // parentGO = GetGameObject(QounterPositions[position].parentName);
        if (!parentGO->get_activeSelf()) {
            DeactivateChildren(parentGO);
            parentGO->SetActive(true);
        }
        containerGO = UnityEngine::GameObject::New_ctor(containerName);
        auto rect = containerGO->AddComponent<UnityEngine::RectTransform*>();
        containerGO->get_transform()->SetParent(parentGO->get_transform(), false);
        auto anchoredPosition = QounterPositions[position].anchoredPosition;
        if (position == QountersMinus::QounterPosition::BelowCombo || position == QountersMinus::QounterPosition::AboveCombo) {
            anchoredPosition.y *= 1.0f + (QountersMinus::Qounter::ComboOffset * distanceUnit * distanceUnitOffsetMult * (QounterPositions[position].distanceIsDown ? -1.0f : 1.0f));
        } else if (position == QountersMinus::QounterPosition::BelowMultiplier || position == QountersMinus::QounterPosition::AboveMultiplier) {
            anchoredPosition.y *= 1.0f + (QountersMinus::Qounter::MultiplierOffset * distanceUnit * distanceUnitOffsetMult * (QounterPositions[position].distanceIsDown ? -1.0f : 1.0f));
        // } else {
        //     auto parentPosition = parentGO->get_transform()->get_position();
        //     anchoredPosition.x += parentPosition.x * -100.0f;
        //     LOG_DEBUG("Ending anchored x %d %.2f (from %.2f)", position, anchoredPosition.x, parentPosition.x);
        }
        rect->set_anchoredPosition(anchoredPosition);
        if (position == QountersMinus::QounterPosition::BelowEnergy || position == QountersMinus::QounterPosition::AboveHighway) {
            auto position = containerGO->get_transform()->get_position();
            containerGO->get_transform()->set_position(UnityEngine::Vector3(0.0f, position.y, position.z));
        }
    }
    return containerGO;
}

void SetPosition(UnityEngine::Transform* transform, QountersMinus::QounterPosition position, float distance) {
    const auto mult = QounterPositions[position].distanceIsDown ? -1.0f : 1.0f;
    const auto pivot = UnityEngine::Vector2(0.5f, QounterPositions[position].distanceIsDown ? 1.0f : 0.0f);
    const auto anchoredPosition = UnityEngine::Vector2(0.0f, distance * distanceUnit * mult);
    reinterpret_cast<UnityEngine::RectTransform*>(transform)->set_pivot(pivot);
    reinterpret_cast<UnityEngine::RectTransform*>(transform)->set_anchoredPosition(anchoredPosition);
}

QountersMinus::Qounter* QountersMinus::Qounter::Initialize(System::Type* type, QountersMinus::QounterPosition position, float distance) {
    auto parent = GetParent(position);
    auto instance = reinterpret_cast<Qounter*>(parent->AddComponent(type));
    SetPosition(instance->gameObject->get_transform(), position, distance);
    return instance;
}

void QountersMinus::Qounter::Awake() {
    static ConstString gameObjectName("QountersMinus_Qounter");
    gameObject = UnityEngine::GameObject::New_ctor(gameObjectName);
    gameObject->get_transform()->SetParent(get_transform(), false);
    gameObject->AddComponent<UnityEngine::RectTransform*>();
    refs = gameObject->AddComponent<QountersMinus::InjectedComponents*>();
}

void QountersMinus::Qounter::OnDestroy() {
    UnityEngine::GameObject::Destroy(gameObject);
}

TMPro::TextMeshProUGUI* QountersMinus::Qounter::CreateBasicTitle(std::string text) {
    auto titleText = QuestUI::BeatSaberUI::CreateText(gameObject->get_transform(), text, false);
    titleText->set_alignment(TMPro::TextAlignmentOptions::Center);
    titleText->set_fontSize(27.0f);
    titleText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0.0f, 0.0f));
    if(Qounter::ItalicText)
        titleText->set_fontStyle(TMPro::FontStyles::Italic);
    return titleText;
}

TMPro::TextMeshProUGUI* QountersMinus::Qounter::CreateBasicText(std::string text) {
    basicText = QuestUI::BeatSaberUI::CreateText(gameObject->get_transform(), text, false);
    basicText->set_alignment(TMPro::TextAlignmentOptions::Center);
    basicText->set_fontSize(35.0f);
    basicText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0.0f, -30.0f));
    if(Qounter::ItalicText)
        basicText->set_fontStyle(TMPro::FontStyles::Italic);
    return basicText;
}
