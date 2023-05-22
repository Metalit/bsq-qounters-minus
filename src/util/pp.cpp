#include "util/pp.hpp"

const int PP_CURVE_SIZE = 37;
float ppCurve[PP_CURVE_SIZE][2] = {
    {0.0, 0.0},
    {0.6, 0.18223233667439062},
    {0.65, 0.5866010012767576},
    {0.7, 0.6125565959114954},
    {0.75, 0.6451808210101443},
    {0.8, 0.6872268862950283},
    {0.825, 0.7150465663454271},
    {0.85, 0.7462290664143185},
    {0.875, 0.7816934560296046},
    {0.9, 0.825756123560842},
    {0.91, 0.8488375988124467},
    {0.92, 0.8728710341448851},
    {0.93, 0.9039994071865736},
    {0.94, 0.9417362980580238},
    {0.95, 1.0},
    {0.955, 1.0388633331418984},
    {0.96, 1.0871883573850478},
    {0.965, 1.1552120359501035},
    {0.97, 1.2485807759957321},
    {0.9725, 1.3090333065057616},
    {0.975, 1.3807102743105126},
    {0.9775, 1.4664726399289512},
    {0.98, 1.5702410055532239},
    {0.9825, 1.697536248647543},
    {0.985, 1.8563887693647105},
    {0.9875, 2.058947159052738},
    {0.99, 2.324506282149922},
    {0.99125, 2.4902905794106913},
    {0.9925, 2.685667856592722},
    {0.99375, 2.9190155639254955},
    {0.995, 3.2022017597337955},
    {0.99625, 3.5526145337555373},
    {0.9975, 3.996793606763322},
    {0.99825, 4.325027383589547},
    {0.999, 4.715470646416203},
    {0.9995, 5.019543595874787},
    {1.0, 5.367394282890631}
};
float ppCurveSlopes[36];
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
