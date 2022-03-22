#include "util/song_id.hpp"

SongID GetSongID(GlobalNamespace::IDifficultyBeatmap* beatmap) {
    auto levelID = il2cpp_utils::RunMethod<StringW>(beatmap->get_level(), "get_levelID").value();
    return { levelID, beatmap->get_difficulty() };
}
