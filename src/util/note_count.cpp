#include "util/note_count.hpp"
#include "util/logger.hpp"

#include "GlobalNamespace/BeatmapCallbacksUpdater.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/IBeatmapDataBasicInfo.hpp"
#include "GlobalNamespace/ScoreModel.hpp"

#include "UnityEngine/Object.hpp"

using namespace GlobalNamespace;

int GetNoteCount() {
    auto bcc = UnityEngine::Object::FindObjectOfType<BeatmapCallbacksUpdater*>()->beatmapCallbacksController;
    return ((IBeatmapDataBasicInfo*) bcc->beatmapData)->get_cuttableNotesCount();
}

int GetMaxMultipliedScore() {
    auto bcc = UnityEngine::Object::FindObjectOfType<BeatmapCallbacksUpdater*>()->beatmapCallbacksController;
    return ScoreModel::ComputeMaxMultipliedScoreForBeatmap(bcc->beatmapData);
}
