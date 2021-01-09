#pragma once

#include <map>
#include "logger.hpp"
#include "config.hpp"
#include "Qounter.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"

// Include all Qounter types [ALL-QOUNTERS]
#include "Qounters/CutQounter.hpp"
#include "Qounters/MissQounter.hpp"
#include "Qounters/NotesQounter.hpp"
#include "Qounters/NotesLeftQounter.hpp"

namespace QountersMinus {
    namespace QounterRegistry {        
        void RegisterTypes();
        void Initialize();
        void DestroyAll();
        
        UnityEngine::GameObject* GetParent(QounterPosition config);

        void OnNoteCut(GlobalNamespace::NoteData* data, GlobalNamespace::NoteCutInfo* info);
        void OnNoteMiss(GlobalNamespace::NoteData* data);
        void OnScoreUpdated(int modifiedScore);
        void OnMaxScoreUpdated(int maxModifiedScore);
        
        // Typed initializer for each Qounter type [ALL-QOUNTERS]
        void Initialize(CutQounterConfig config);
        void Initialize(MissQounterConfig config);
        void Initialize(NotesQounterConfig config);
        void Initialize(NotesLeftQounterConfig config);
    };
};
