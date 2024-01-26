#include "Utils/HashUtils.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include <filesystem>

#include "CustomLogger.hpp"

#include "Utils/FileUtils.hpp"
#include "Utils/CacheUtils.hpp"

#include "libcryptopp/shared/sha.h"
#include "libcryptopp/shared/hex.h"
#include "libcryptopp/shared/files.h"

#include "GlobalNamespace/StandardLevelInfoSaveData.hpp"
#include "CustomTypes/CustomLevelInfoSaveData.hpp"

using namespace GlobalNamespace;
using namespace CryptoPP;

namespace RuntimeSongLoader::HashUtils {

    std::optional<std::string> GetCustomLevelHash(CustomJSONData::CustomLevelInfoSaveData* level, std::string const& customLevelPath) {
        auto start = std::chrono::high_resolution_clock::now();
        std::string hashHex;
        LOG_DEBUG("GetCustomLevelHash Start");

        auto cacheDataOpt = CacheUtils::GetCacheData(customLevelPath);
        if(!cacheDataOpt.has_value())
            return std::nullopt;
        auto cacheData = *cacheDataOpt;
        auto cacheSHA1 = cacheData.sha1;
        if(cacheSHA1.has_value())
        {
            hashHex = *cacheSHA1;
            LOG_DEBUG("GetCustomLevelHash Stop Result %s from cache", hashHex.c_str());
            return hashHex;
        }

        std::string actualPath = customLevelPath + "/Info.dat";
        if(!fileexists(actualPath))
            actualPath = customLevelPath + "/info.dat";
        if(!fileexists(actualPath))
            return std::nullopt;

        SHA1 hashType;
        std::string hashResult;
        HashFilter hashFilter(hashType, new StringSink(hashResult));

        FileSource fs(actualPath.c_str(), false);
        fs.Attach(new Redirector(hashFilter));
        fs.Pump(LWORD_MAX);
        fs.Detach();
        for(auto val : level->difficultyBeatmapSets) {
            if (!val) continue;
            auto difficultyBeatmaps = val->difficultyBeatmaps;
            if (!difficultyBeatmaps) continue;
            for(auto difficultyBeatmap : difficultyBeatmaps) {
                std::string diffFile = difficultyBeatmap->beatmapFilename;
                std::string path(customLevelPath);
                path.append("/").append(diffFile);
                if(!fileexists(path)) {
                    LOG_ERROR("GetCustomLevelHash File %s did not exist", path.c_str());
                    continue;
                }
                FileSource fs(path.c_str(), false);
                fs.Attach(new Redirector(hashFilter));
                fs.Pump(LWORD_MAX);
                fs.Detach();
            }
        }

        hashFilter.MessageEnd();

        HexEncoder hexEncoder(new StringSink(hashHex));
        hexEncoder.Put((const byte*)hashResult.data(), hashResult.size());

        cacheData.sha1 = hashHex;
        CacheUtils::UpdateCacheData(customLevelPath, cacheData);

        std::chrono::milliseconds duration = duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        LOG_DEBUG("GetCustomLevelHash Stop Result %s Time %d", hashHex.c_str(), (int)duration.count());
        return hashHex;
    }

    std::optional<int> GetDirectoryHash(std::string_view path) {
        if(!std::filesystem::is_directory(path))
            return std::nullopt;
        int hash = 0;
        bool hasFile = false;
        for (auto const& entry : std::filesystem::directory_iterator(path)) {
            if(!entry.is_directory()) {
                hasFile = true;
                hash ^= entry.file_size() ^ std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(entry).time_since_epoch()).count();
            }
        }
        if(!hasFile)
            return std::nullopt;
        return hash;
    }

}
