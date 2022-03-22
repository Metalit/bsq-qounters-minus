#include "util/note_count.hpp"

#include "GlobalNamespace/BeatmapCallbacksUpdater.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/ScoreModel.hpp"

#include "UnityEngine/Object.hpp"

#include "System/Collections/Generic/LinkedList_1.hpp"

using namespace GlobalNamespace;

int GetNoteCount() {
    int noteCount = 0;
    auto bcc = UnityEngine::Object::FindObjectOfType<BeatmapCallbacksUpdater*>()->beatmapCallbacksController;
    auto songTime = bcc->startFilterTime;
    auto noteDataItems = ((BeatmapData*) bcc->beatmapData)->GetBeatmapDataItems<NoteData*>();
    auto enumerator = (System::Collections::Generic::LinkedList_1<NoteData*>::Enumerator*) noteDataItems->GetEnumerator();
    while(enumerator->MoveNext()) {
        auto noteData = enumerator->get_Current();
        if(noteData->colorType != ColorType::None && noteData->get_time() > songTime)
            noteCount++;
    }
    return noteCount;
}

int GetMaxMultipliedScore() {
    auto bcc = UnityEngine::Object::FindObjectOfType<BeatmapCallbacksUpdater*>()->beatmapCallbacksController;
    return ScoreModel::ComputeMaxMultipliedScoreForBeatmap(bcc->beatmapData);
}
