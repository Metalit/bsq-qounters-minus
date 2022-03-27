#include "util/note_count.hpp"
#include "util/logger.hpp"

#include "GlobalNamespace/BeatmapCallbacksUpdater.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/ScoreModel.hpp"

#include "UnityEngine/Object.hpp"

#include "GlobalNamespace/BeatmapDataSortedListForTypes_1.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"

#include "System/Collections/IEnumerator.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"

using namespace GlobalNamespace;

int GetNoteCount() {
    using LinkedList = System::Collections::Generic::LinkedList_1<NoteData*>;
    int noteCount = 0;
    auto bcc = UnityEngine::Object::FindObjectOfType<BeatmapCallbacksUpdater*>()->beatmapCallbacksController;
    auto songTime = bcc->startFilterTime;
    auto noteDataItemsList = (LinkedList*) ((BeatmapData*) bcc->beatmapData)->beatmapDataItemsPerType->GetList(csTypeOf(NoteData*))->get_items();
    auto enumerator = (LinkedList::Enumerator*) noteDataItemsList->System_Collections_IEnumerable_GetEnumerator();
    while(enumerator->MoveNext()) {
        auto noteData = (NoteData*) enumerator->System_Collections_IEnumerator_get_Current();
        if(noteData->colorType != ColorType::None && noteData->get_time() > songTime)
            noteCount++;
    }
    return noteCount;
}

int GetMaxMultipliedScore() {
    auto bcc = UnityEngine::Object::FindObjectOfType<BeatmapCallbacksUpdater*>()->beatmapCallbacksController;
    return ScoreModel::ComputeMaxMultipliedScoreForBeatmap(bcc->beatmapData);
}
