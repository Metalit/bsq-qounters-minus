#include "util/pp.hpp"

const int PP_CURVE_SIZE = 16;
float ppCurve[PP_CURVE_SIZE][2] = {
    {0, 0},
    {.45f, .015f},
    {.50f, .03f},
    {.55f, .06f},
    {.60f, .105f},
    {.65f, .16f},
    {.68f, .24f},
    {.70f, .285f},
    {.80f, .563f},
    {.84f, .695f},
    {.88f, .826f},
    {.945f, 1.015f},
    {.95f, 1.046f},
    {1.00f, 1.12f},
    {1.10f, 1.18f},
    {1.14f, 1.25f}
};
float ppCurveSlopes[15];
static std::unordered_set<std::string> songsAllowingPositiveModifiers = {
    "2FDDB136BDA7F9E29B4CB6621D6D8E0F8A43B126", // Overkill Nuketime
    "27FCBAB3FB731B16EABA14A5D039EEFFD7BD44C9" // Overkill Kry
};
const std::string PP_DATA_URI = "https://cdn.pulselane.dev/raw_pp.json";

void QountersMinus::PP::Initialize() {
    LOG_CALLER;
    request = UnityEngine::Networking::UnityWebRequest::Get(PP_DATA_URI);
    request->SetRequestHeader("User-Agent", ID " " VERSION);
    request->SendWebRequest()->add_completed(il2cpp_utils::MakeDelegate<DownloadCompleteDelegate>(
        classof(DownloadCompleteDelegate), (void*)nullptr, QountersMinus::PP::HandleWebRequestCompleted
    ));

    // precalculate curve slopes
    for (auto i = 0; i < PP_CURVE_SIZE - 1; i++) {
        auto x1 = ppCurve[i][0];
        auto y1 = ppCurve[i][1];
        auto x2 = ppCurve[i+1][0];
        auto y2 = ppCurve[i+1][1];

        auto m = (y2 - y1) / (x2 - x1);
        ppCurveSlopes[i] = m;
    }
}

void QountersMinus::PP::HandleWebRequestCompleted() {
    LOG_CALLER;
    auto response = request->get_downloadHandler()->GetText();
    rapidjson::Document document;
    document.Parse(response);

    for (auto itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr) {
        RawPPData data;
        if (itr->value.HasMember("_Easy_SoloStandard"))
            data._Easy_SoloStandard = itr->value["_Easy_SoloStandard"].GetFloat();
        if (itr->value.HasMember("_Normal_SoloStandard"))
            data._Normal_SoloStandard = itr->value["_Normal_SoloStandard"].GetFloat();
        if (itr->value.HasMember("_Hard_SoloStandard"))
            data._Hard_SoloStandard = itr->value["_Hard_SoloStandard"].GetFloat();
        if (itr->value.HasMember("_Expert_SoloStandard"))
            data._Expert_SoloStandard = itr->value["_Expert_SoloStandard"].GetFloat();
        if (itr->value.HasMember("_ExpertPlus_SoloStandard"))
            data._ExpertPlus_SoloStandard = itr->value["_ExpertPlus_SoloStandard"].GetFloat();
        index.insert({std::string(itr->name.GetString()), data});
    }
}

float RatioOfMaxPP(float accuracy) {
    if (accuracy >= 1.14) return 1.25f;
    if (accuracy <= 0.0f) return 0.0f;

    int i = 0;
    for (; i < PP_CURVE_SIZE; i++)
        if (i == PP_CURVE_SIZE - 1 || ppCurve[i + 1][0] > accuracy) break;

    auto accuracyFloor = ppCurve[i][0];
    auto ppFloor = ppCurve[i][1];
    return ppCurveSlopes[i] * (accuracy - accuracyFloor) + ppFloor;
}

std::string SongIDToHash(std::string songID) {
    if (!songID.starts_with("custom_level_")) return "";
    for (auto& c: songID) c = toupper(c);
    return songID.substr(13);
}

float QountersMinus::PP::CalculatePP(float maxPP, float accuracy) {
    return maxPP * RatioOfMaxPP(accuracy);
}

bool QountersMinus::PP::SongAllowsPositiveModifiers(std::string songID) {
    return songsAllowingPositiveModifiers.contains(SongIDToHash(songID));
}

std::optional<float> QountersMinus::PP::BeatmapMaxPP(std::string songID, GlobalNamespace::BeatmapDifficulty difficulty) {
    auto itr = index.find(SongIDToHash(songID));
    if (itr == index.end()) return {};

    switch ((int)difficulty) {
        case 0:
            return itr->second._Easy_SoloStandard;
        case 1:
            return itr->second._Normal_SoloStandard;
        case 2:
            return itr->second._Hard_SoloStandard;
        case 3:
            return itr->second._Expert_SoloStandard;
        case 4:
            return itr->second._ExpertPlus_SoloStandard;
        default:
            return {};
    }
}
