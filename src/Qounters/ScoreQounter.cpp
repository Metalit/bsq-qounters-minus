#include "Qounters/ScoreQounter.hpp"

DEFINE_TYPE(QountersMinus::Qounters,ScoreQounter);

bool QountersMinus::Qounters::ScoreQounter::Enabled = true;
int QountersMinus::Qounters::ScoreQounter::Position = static_cast<int>(QountersMinus::QounterPosition::BelowMultiplier);
float QountersMinus::Qounters::ScoreQounter::Distance = 0.0f;
int QountersMinus::Qounters::ScoreQounter::Mode = static_cast<int>(QountersMinus::ScoreQounterMode::Original);
int QountersMinus::Qounters::ScoreQounter::DecimalPrecision = 2;
bool QountersMinus::Qounters::ScoreQounter::DisplayRank = true;
bool QountersMinus::Qounters::ScoreQounter::CustomRankColors = true;
UnityEngine::Color QountersMinus::Qounters::ScoreQounter::SSColor = UnityEngine::Color(0.0f, 1.0f, 1.0f, 1.0f);
UnityEngine::Color QountersMinus::Qounters::ScoreQounter::SColor = UnityEngine::Color(1.0f, 1.0f, 1.0f, 1.0f);
UnityEngine::Color QountersMinus::Qounters::ScoreQounter::AColor = UnityEngine::Color(0.0f, 1.0f, 0.0f, 1.0f);
UnityEngine::Color QountersMinus::Qounters::ScoreQounter::BColor = UnityEngine::Color(1.0f, 0.92f, 0.016f, 1.0f);
UnityEngine::Color QountersMinus::Qounters::ScoreQounter::CColor = UnityEngine::Color(1.0f, 0.5f, 0.0f, 1.0f);
UnityEngine::Color QountersMinus::Qounters::ScoreQounter::DColor = UnityEngine::Color(1.0f, 0.0f, 0.0f, 1.0f);
UnityEngine::Color QountersMinus::Qounters::ScoreQounter::EColor = UnityEngine::Color(1.0f, 0.0f, 0.0f, 1.0f);

void QountersMinus::Qounters::ScoreQounter::Register() {
    QounterRegistry::Register<ScoreQounter>("Score", "Score Qounter", "ScoreConfig", true);
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &Mode,
        .field = "Mode",
        .helpText = "How should this Qounter display data?",
        .type = QounterRegistry::ConfigType::Enum,
        .enumNumElements = ScoreQounterModeCount,
        .enumDisplayNames = ScoreQounterModeNames,
        .enumSerializedNames = ScoreQounterModeLookup,
    });
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &DecimalPrecision,
        .field = "DecimalPrecision",
        .displayName = "Percentage Precision",
        .helpText = "How precise should the percentage be?",
        .type = QounterRegistry::ConfigType::Int,
        .intMin = 0,
        .intMax = 5,
    });
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &CustomRankColors,
        .field = "CustomRankColors",
        .displayName = "Custom Rank Colors",
        .helpText = "Colors your Score Qounter depending on the rank you have in a song.",
        .type = QounterRegistry::ConfigType::Bool,
    });
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &SSColor,
        .field = "SSColor",
        .displayName = "SS Color",
        .helpText = "Change the rank color for the SS rank.",
        .type = QounterRegistry::ConfigType::Color,
    });
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &SColor,
        .field = "SColor",
        .displayName = "S Color",
        .helpText = "Change the rank color for the S rank.",
        .type = QounterRegistry::ConfigType::Color,
    });
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &AColor,
        .field = "AColor",
        .displayName = "A Color",
        .helpText = "Change the rank color for the A rank.",
        .type = QounterRegistry::ConfigType::Color,
    });
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &BColor,
        .field = "BColor",
        .displayName = "B Color",
        .helpText = "Change the rank color for the B rank.",
        .type = QounterRegistry::ConfigType::Color,
    });
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &CColor,
        .field = "CColor",
        .displayName = "C Color",
        .helpText = "Change the rank color for the C rank.",
        .type = QounterRegistry::ConfigType::Color,
    });
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &DColor,
        .field = "DColor",
        .displayName = "D Color",
        .helpText = "Change the rank color for the D rank.",
        .type = QounterRegistry::ConfigType::Color,
    });
    QounterRegistry::RegisterConfig<ScoreQounter>({
        .ptr = &EColor,
        .field = "EColor",
        .displayName = "E Color",
        .helpText = "Change the rank color for the E rank.",
        .type = QounterRegistry::ConfigType::Color,
    });
}

void logRecursive(UnityEngine::GameObject* obj, std::string path) {
    auto name = path + "/" + static_cast<std::string>(obj->get_name());
    LOG_DEBUG(name);
    for (int i = 0; i < obj->get_transform()->get_childCount(); i++) {
        logRecursive(obj->get_transform()->GetChild(i)->get_gameObject(), name);
    }
}

void QountersMinus::Qounters::ScoreQounter::Start() {
    relativeScoreText = refs->coreGameHUDController->relativeScoreGO->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
    relativeScoreText->set_color(UnityEngine::Color::get_white());
    rankText = refs->coreGameHUDController->immediateRankGO->GetComponent<TMPro::TextMeshProUGUI*>();
    rankText->set_color(UnityEngine::Color::get_white());

    auto scoreUIController = refs->coreGameHUDController->GetComponentInChildren<GlobalNamespace::ScoreUIController*>();
    auto scoreUIText = scoreUIController->scoreText;
    scoreUIText->get_transform()->SetParent(gameObject->get_transform(), true);
    refs->coreGameHUDController->relativeScoreGO->get_transform()->SetParent(scoreUIText->get_transform(), true);
    refs->coreGameHUDController->immediateRankGO->get_transform()->SetParent(scoreUIText->get_transform(), true);
    
    scoreUIText->set_enabled(true);
    relativeScoreText->set_enabled(true);
    rankText->set_enabled(true);

    if (!QountersMinus::Qounter::ItalicText) {
        scoreUIText->set_fontStyle(TMPro::FontStyles::Normal);
        relativeScoreText->set_fontStyle(TMPro::FontStyles::Normal);
        rankText->set_fontStyle(TMPro::FontStyles::Normal);
    } else {
        scoreUIText->set_fontStyle(TMPro::FontStyles::Italic);
        relativeScoreText->set_fontStyle(TMPro::FontStyles::Italic);
        rankText->set_fontStyle(TMPro::FontStyles::Italic);
    }
    auto localPosition1 = relativeScoreText->get_rectTransform()->get_localPosition();
    relativeScoreText->get_rectTransform()->set_localPosition(UnityEngine::Vector3(0.0f, localPosition1.y, localPosition1.z));
    auto localPosition2 = rankText->get_rectTransform()->get_localPosition();
    rankText->get_rectTransform()->set_localPosition(UnityEngine::Vector3(0.0f, localPosition2.y, localPosition2.z));

    auto immediateRankUIPanel = refs->coreGameHUDController->GetComponentInChildren<GlobalNamespace::ImmediateRankUIPanel*>();

    if (Mode == static_cast<int>(QountersMinus::ScoreQounterMode::RankOnly))
        UnityEngine::Object::Destroy(refs->coreGameHUDController->relativeScoreGO);
    if (Mode == static_cast<int>(QountersMinus::ScoreQounterMode::ScoreOnly))
        UnityEngine::Object::Destroy(refs->coreGameHUDController->immediateRankGO);
    if (Mode == static_cast<int>(QountersMinus::ScoreQounterMode::RemoveScore)) {
        scoreUIText->set_enabled(false);
    }

    // instead of calculating original text position based on canvas like counters+,
    // just slap that bad boy right on our unpopulated gameobject
    auto basePosition = gameObject->get_transform()->get_position();
    scoreUIText->get_transform()->set_position(basePosition);

    UnityEngine::Object::Destroy(immediateRankUIPanel);

    refs->relativeScoreAndImmediateRankCounter->add_relativeScoreOrImmediateRankDidChangeEvent(il2cpp_utils::MakeDelegate<System::Action*>(
        classof(System::Action*), this, +[](QountersMinus::Qounters::ScoreQounter* self) {
            self->UpdateText();
        }
    ));
    UpdateText();
}

UnityEngine::Color QountersMinus::Qounters::ScoreQounter::GetRankColor(GlobalNamespace::RankModel::Rank rank) {
    switch (rank) {
        case GlobalNamespace::RankModel::Rank::S: return SColor;
        case GlobalNamespace::RankModel::Rank::A: return AColor;
        case GlobalNamespace::RankModel::Rank::B: return BColor;
        case GlobalNamespace::RankModel::Rank::C: return CColor;
        case GlobalNamespace::RankModel::Rank::D: return DColor;
        case GlobalNamespace::RankModel::Rank::E: return EColor;
        default: return SSColor;
    }
}

void QountersMinus::Qounters::ScoreQounter::UpdateText() {
    auto immediateRank = refs->relativeScoreAndImmediateRankCounter->immediateRank;
    if (immediateRank != prevImmediateRank) {
        rankText->set_text(GlobalNamespace::RankModel::GetRankName(immediateRank));
        prevImmediateRank = immediateRank;
        rankText->set_color(CustomRankColors ? GetRankColor(immediateRank) : UnityEngine::Color::get_white());
    }
    float relativeScore = refs->relativeScoreAndImmediateRankCounter->relativeScore * 100.0f;
    relativeScoreText->set_text(FormatNumber(relativeScore, DecimalPrecision) + "%");
}
