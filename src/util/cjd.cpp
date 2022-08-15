#include "util/cjd.hpp"
#include "config.hpp"
#include "main.hpp"

#include <any>

std::optional<bool> GetBool(const rapidjson::Value &object, std::string_view const key) {
    auto itr = object.FindMember(key.data());
    if (itr != object.MemberEnd()) {
        if (itr->value.IsString()) {
            if (std::string(itr->value.GetString()) == "true")
                return true;
            return false;
        }
        return itr->value.GetBool();
    }
    return std::nullopt;
}

class JSONWrapper : public Il2CppObject {
    public:
    std::optional<std::reference_wrapper<const rapidjson::Value>> value;
    std::unordered_map<char, std::any> associatedData;
};
Il2CppClass* customNoteDataClass = nullptr;

bool GetHasCJD() {
    static bool hasCJD = Modloader::getMods().contains("CustomJSONData");
    return hasCJD;
}

bool InitializeClasses() {
    static std::optional<bool> initialized = std::nullopt;
    if (initialized.has_value())
        return initialized.value();
    
    if (!GetHasCJD()) {
        customNoteDataClass = nullptr;
        initialized = false;
    } else {
        customNoteDataClass = il2cpp_utils::GetClassFromName("CustomJSONData", "CustomNoteData");
        initialized = true;
    }
    return initialized.value();
}

std::optional<std::reference_wrapper<const rapidjson::Value>> GetCustomData(GlobalNamespace::NoteData* noteData) {
    if (!InitializeClasses())
        return std::nullopt;
    if (noteData->klass != customNoteDataClass)
        return std::nullopt;
    auto json = (JSONWrapper*) *il2cpp_utils::GetFieldValue(noteData, "customData");
    return json->value;
}

#define RET_VALUE(key) \
auto k##key = GetBool(doc, #key); \
if (k##key) return k##key.value();

#define RET_NOT_VALUE(key) \
auto k##key = GetBool(doc, #key); \
if (k##key) return !k##key.value();

bool IsRealNote(GlobalNamespace::NoteData* noteData) {
    auto opt = GetCustomData(noteData);
    if (!opt.has_value())
        return true;
    auto& doc = opt->get();
    RET_VALUE(_interactable);
    RET_VALUE(interactable);
    RET_NOT_VALUE(uninteractable);
    RET_NOT_VALUE(_fake);
    RET_NOT_VALUE(NE_fake);
    return true;
}
