#include "Qounters/FailQounter.hpp"

DEFINE_TYPE(QountersMinus::Qounters,FailQounter);

bool QountersMinus::Qounters::FailQounter::Enabled = false;
int QountersMinus::Qounters::FailQounter::Position = static_cast<int>(QountersMinus::QounterPosition::AboveCombo);
float QountersMinus::Qounters::FailQounter::Distance = 0.0f;
bool QountersMinus::Qounters::FailQounter::ShowRestartsInstead = true;

int QountersMinus::Qounters::FailQounter::restarts = 0;
std::string QountersMinus::Qounters::FailQounter::prevBeatmapHash;

void QountersMinus::Qounters::FailQounter::Register() {
    QounterRegistry::Register<FailQounter>("Fail", "Fail Qounter", "FailConfig", true);
    QounterRegistry::RegisterConfig<FailQounter>({
        .ptr = &ShowRestartsInstead,
        .field = "ShowRestartsInstead",
        .displayName = "Track Restarts",
        .helpText = "Instead of showing global fail count, show the amount of times you have restarted the same song.",
        .type = QounterRegistry::ConfigType::Bool,
    });
}

void QountersMinus::Qounters::FailQounter::Start() {
    CreateBasicTitle(ShowRestartsInstead ? "Restarts" : "Fails");

    auto songID = GetSongID(refs->difficultyBeatmap);
    auto currentBeatmapHash = songID.hash + "_" + std::to_string(songID.difficulty);

    if (ShowRestartsInstead) {
        if (prevBeatmapHash == currentBeatmapHash) {
            restarts++;
            count = restarts;
        } else {
            restarts = count = 0;
            prevBeatmapHash = currentBeatmapHash;
        }
    } else {
        count = refs->playerData->playerAllOverallStatsData->get_allOverallStatsData()->failedLevelsCount;
        auto gameEnergyCounter = UnityEngine::Object::FindObjectOfType<GlobalNamespace::GameEnergyCounter*>();
        gameEnergyCounter->add_gameEnergyDidReach0Event(il2cpp_utils::MakeDelegate<System::Action*>(
            classof(System::Action*), this, +[](QountersMinus::Qounters::FailQounter* self) {
                self->count += 1;
                self->basicText->set_text(std::to_string(self->count));
                self->animationTimer = 0.0f;
            }
        ));
    }

    CreateBasicText(std::to_string(count));
    animationTimer = 10.0f;
}

void QountersMinus::Qounters::FailQounter::Update() {
    if (animationTimer < 1.0f) {
        basicText->set_color(UnityEngine::Color::Lerp(UnityEngine::Color::get_white(), UnityEngine::Color::get_red(), animationTimer));
        animationTimer += UnityEngine::Time::get_deltaTime();
    }
}
