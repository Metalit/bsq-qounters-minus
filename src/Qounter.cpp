#include "Qounter.hpp"

DEFINE_CLASS(QountersMinus::Qounter);

void QountersMinus::Qounter::Awake() {
    LOG_CALLER;
    static auto gameObjectName = il2cpp_utils::createcsstr("QountersMinus_Qounter", il2cpp_utils::StringType::Manual);
    gameObject = UnityEngine::GameObject::New_ctor(gameObjectName);
    gameObject->get_transform()->SetParent(get_transform(), false);
}

void QountersMinus::Qounter::Destroy() {
    UnityEngine::GameObject::Destroy(gameObject);
}
