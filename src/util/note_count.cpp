#include "Qounters/NotesLeftQounter.hpp"

#include "GlobalNamespace/BeatmapCallbacksUpdater.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/ScoreModel.hpp"

#include "GlobalNamespace/BeatmapDataSortedListForTypeAndIds_1.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"

#include "System/Collections/IEnumerator.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"

using namespace GlobalNamespace;
using namespace QountersMinus::Qounters;

int GetNoteCount() {
    using LinkedList = System::Collections::Generic::LinkedList_1<NoteData*>;
    int noteCount = 0;
    auto bcc = UnityEngine::Object::FindObjectOfType<BeatmapCallbacksUpdater*>()->beatmapCallbacksController;
    auto songTime = bcc->startFilterTime;
    auto noteDataItemsList = (LinkedList*) ((BeatmapData*) bcc->beatmapData)->beatmapDataItemsPerTypeAndId->GetList(csTypeOf(NoteData*), 0)->get_items();
    auto enumerator = (LinkedList::Enumerator*) noteDataItemsList->System_Collections_IEnumerable_GetEnumerator();
    while(enumerator->MoveNext()) {
        auto noteData = (NoteData*) enumerator->System_Collections_IEnumerator_get_Current();
        if(NotesLeftQounter::ShouldProcessNote(noteData) && noteData->get_time() > songTime)
            noteCount++;
    }
    return noteCount;
}

int GetMaxMultipliedScore() {
    auto bcc = UnityEngine::Object::FindObjectOfType<BeatmapCallbacksUpdater*>()->beatmapCallbacksController;
    return ScoreModel::ComputeMaxMultipliedScoreForBeatmap(bcc->beatmapData);
}
