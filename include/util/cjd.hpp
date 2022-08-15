#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "GlobalNamespace/NoteData.hpp"

bool GetHasCJD();

std::optional<std::reference_wrapper<const rapidjson::Value>> GetCustomData(GlobalNamespace::NoteData* noteData);

bool IsRealNote(GlobalNamespace::NoteData* noteData);
