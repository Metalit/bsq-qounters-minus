#include "hooks.hpp"

#include "util/logger.hpp"
#include "util/pp.hpp"
#include "config.hpp"
#include "QounterRegistry.hpp"
#include "Qounter.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "GlobalNamespace/CoreGameHUDController.hpp"
#include "GlobalNamespace/CoreGameHUDController_InitData.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/BeatmapObjectManager_NoteWasCutDelegate.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/QuestAppInit.hpp"
#include "System/Action_2.hpp"
#include "System/Action_1.hpp"

using namespace QountersMinus;
using namespace GlobalNamespace;

typedef BeatmapObjectManager::NoteWasCutDelegate* NoteWasCutDelegate;
typedef System::Action_1<NoteController*>* NoteWasMissedDelegate;
typedef System::Action_2<int, int>* ScoreChangeDelegate;

MAKE_HOOK_MATCH(CoreGameHUDController_Start, &CoreGameHUDController::Start,
        void, CoreGameHUDController* self) {
    LOG_CALLER;

    self->initData->advancedHUD = true;

    CoreGameHUDController_Start(self);

    QounterRegistry::Initialize();
}

MAKE_HOOK_MATCH(ScoreController_Start, &ScoreController::Start,
        void, ScoreController* self) {
    LOG_CALLER;

    ScoreController_Start(self);

    // self->beatmapObjectManager->add_noteWasCutEvent(il2cpp_utils::MakeDelegate<NoteWasCutDelegate>(
    //     classof(NoteWasCutDelegate), self, +[](ScoreController* self, NoteController* noteController, ByRef<NoteCutInfo> info) {
    //         QounterRegistry::BroadcastEvent(QounterRegistry::Event::NoteCut, noteController->noteData, &info.heldRef);
    //     }
    // ));
    // self->beatmapObjectManager->add_noteWasMissedEvent(il2cpp_utils::MakeDelegate<NoteWasMissedDelegate>(
    //     classof(NoteWasMissedDelegate), self, +[](ScoreController* self, NoteController* noteController) {
    //         QounterRegistry::BroadcastEvent(QounterRegistry::Event::NoteMiss, noteController->noteData);
    //     }
    // ));
    self->add_scoreDidChangeEvent(il2cpp_utils::MakeDelegate<ScoreChangeDelegate>(
        classof(ScoreChangeDelegate), self, +[](ScoreController* self, int rawScore, int modifiedScore) {
            QounterRegistry::BroadcastEvent(QounterRegistry::Event::ScoreUpdated, modifiedScore);
        }
    ));
    // delegate no longer exists, but it seems redundant with scoreDidChange
    // self->add_immediateMaxPossibleScoreDidChangeEvent(il2cpp_utils::MakeDelegate<ScoreChangeDelegate>(
    //     classof(ScoreChangeDelegate), self, +[](ScoreController* self, int multipliedSccore, int modifiedScore) {
    //         QounterRegistry::BroadcastEvent(QounterRegistry::Event::MaxScoreUpdated, modifiedScore);
    //     }
    // ));
}

// replacement hooks for delegates
MAKE_HOOK_MATCH(BeatmapObjectManager_HandleNoteControllerNoteWasCut, &BeatmapObjectManager::HandleNoteControllerNoteWasCut,
        void, BeatmapObjectManager* self, NoteController* noteController, ByRef<NoteCutInfo> info) {
    
    BeatmapObjectManager_HandleNoteControllerNoteWasCut(self, noteController, info);

    QounterRegistry::BroadcastEvent(QounterRegistry::Event::NoteCut, noteController->noteData, &info.heldRef);
}

MAKE_HOOK_MATCH(BeatmapObjectManager_HandleNoteControllerNoteWasMissed, &BeatmapObjectManager::HandleNoteControllerNoteWasMissed,
        void, BeatmapObjectManager* self, NoteController* noteController) {
    
    BeatmapObjectManager_HandleNoteControllerNoteWasMissed(self, noteController);

    QounterRegistry::BroadcastEvent(QounterRegistry::Event::NoteMiss, noteController->noteData);
}

MAKE_HOOK_MATCH(CutScoreBuffer_HandleSaberSwingRatingCounterDidFinish, &CutScoreBuffer::HandleSaberSwingRatingCounterDidFinish,
        void, CutScoreBuffer* self, ISaberSwingRatingCounter* swingRatingCounter) {
    
    CutScoreBuffer_HandleSaberSwingRatingCounterDidFinish(self, swingRatingCounter);
    
    QounterRegistry::BroadcastEvent(QounterRegistry::Event::SwingRatingFinished, self);
}

MAKE_HOOK_MATCH(QuestAppInit_AppStartAndMultiSceneEditorSetup, &QuestAppInit::AppStartAndMultiSceneEditorSetup,
        void, QuestAppInit* self) {
    
    QuestAppInit_AppStartAndMultiSceneEditorSetup(self);

    // Defer config loading to this point to give Custom Qounters a chance to register
    LOG_DEBUG("Loading config");
    if(!QountersMinus::LoadConfig())
        QountersMinus::SaveConfig();

    PP::Initialize();
}

void QountersMinus::InstallHooks() {
    INSTALL_HOOK(getLogger(), CoreGameHUDController_Start);
    INSTALL_HOOK(getLogger(), ScoreController_Start);
    INSTALL_HOOK(getLogger(), BeatmapObjectManager_HandleNoteControllerNoteWasCut);
    INSTALL_HOOK(getLogger(), BeatmapObjectManager_HandleNoteControllerNoteWasMissed);
    INSTALL_HOOK(getLogger(), CutScoreBuffer_HandleSaberSwingRatingCounterDidFinish);
    INSTALL_HOOK(getLogger(), QuestAppInit_AppStartAndMultiSceneEditorSetup);
}
