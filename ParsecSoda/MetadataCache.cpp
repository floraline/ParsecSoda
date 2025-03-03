#include "MetadataCache.h"

// This is not ideal, especially in an open source environment.
// I'm using these values just as placeholders until I find an
// actual solution. You should change them in your build.
string MetadataCache::_key = "ParsecSodaKey***";
string MetadataCache::_nonce = "ParsecSoda**";
MetadataCache::Preferences MetadataCache::preferences = MetadataCache::Preferences();

MetadataCache::SessionCache MetadataCache::loadSessionCache()
{
    SessionCache result = SessionCache();

    string dirPath = getUserDir();

    if (!dirPath.empty())
    {
        string filepath = dirPath + "session.json";

        if (MTY_FileExists(filepath.c_str()))
        {
            size_t size;
            const char* encryptedContent;
            encryptedContent = (char*)MTY_ReadFile(filepath.c_str(), &size);

            char originalText[256];
            MTY_AESGCM* ctx = MTY_AESGCMCreate(_key.c_str());
            char tag[16] = "ParsecSodaTag**";
            
            MTY_AESGCMDecrypt(ctx, _nonce.c_str(), encryptedContent, size, tag, (void*)originalText);
            MTY_JSON* json = MTY_JSONParse(originalText);
            if (json != nullptr)
            {
                char sessionID[128];
                char peerID[64];   
                if (
                    MTY_JSONObjGetString(json, "sessionID", sessionID, 128) &&
                    MTY_JSONObjGetString(json, "peerID", peerID, 64)
                )
                {
                    result.isValid = true;
                    result.sessionID = sessionID;
                    result.peerID = peerID;
                }
                
                MTY_JSONDestroy(&json);
            }

            MTY_AESGCMDestroy(&ctx);
        }
    }

    return result;
}

bool MetadataCache::saveSessionCache(SessionCache sessionCache)
{
    bool result = false;
    string dirPath = getUserDir();

    if (!dirPath.empty())
    {
        string filepath = dirPath + "session.json";

        if (sessionCache.isValid)
        {
            MTY_JSON* json = MTY_JSONObjCreate();
            MTY_JSONObjSetString(json, "sessionID", sessionCache.sessionID.c_str());
            MTY_JSONObjSetString(json, "peerID", sessionCache.peerID.c_str());

            string jsonStr = MTY_JSONSerialize(json);
            char encryptedJson[256];
            char tag[16] = "ParsecSodaTag**";
            MTY_AESGCM* ctx = MTY_AESGCMCreate(_key.c_str());
            if (MTY_AESGCMEncrypt(ctx, _nonce.c_str(), jsonStr.c_str(), jsonStr.size(), tag, encryptedJson))
            {
                if (MTY_WriteFile(filepath.c_str(), encryptedJson, jsonStr.size()))
                {
                    result = true;
                }
            }

            MTY_AESGCMDestroy(&ctx);
            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

MetadataCache::Preferences MetadataCache::loadPreferences()
{
    preferences = Preferences();

    string dirPath = getUserDir();
    if (!dirPath.empty())
    {
        string filepath = dirPath + "preferences.json";

        if (MTY_FileExists(filepath.c_str()))
        {
            MTY_JSON *json = MTY_JSONReadFile(filepath.c_str());
            char roomName[256] = "", gameID[72] = "", secret[32] = "";

            if (!MTY_JSONObjGetUInt(json, "audioInputDevice", &preferences.audioInputDevice)) {
                preferences.audioInputDevice = 0;
            }
            
            if (!MTY_JSONObjGetUInt(json, "audioOutputDevice", &preferences.audioOutputDevice)) {
                preferences.audioOutputDevice = 0;
            }

            if (!MTY_JSONObjGetUInt(json, "micVolume", &preferences.micVolume)) {
                preferences.micVolume = 80;
            }
            
            if (!MTY_JSONObjGetBool(json, "micEnabled", &preferences.micEnabled)) {
                preferences.micEnabled = true;
            }

            if (!MTY_JSONObjGetUInt(json, "speakersVolume", &preferences.speakersVolume)) {
                preferences.speakersVolume = 30;
            }

            if (!MTY_JSONObjGetBool(json, "speakersEnabled", &preferences.speakersEnabled)) {
                preferences.speakersEnabled = true;
            }
            
            if (MTY_JSONObjGetString(json, "roomName", roomName, 256)) preferences.roomName = roomName;
            else preferences.roomName = "Let's have fun!";

            if (MTY_JSONObjGetString(json, "gameID", gameID, 72)) preferences.gameID = gameID;
            else preferences.gameID = "";

            if (MTY_JSONObjGetString(json, "secret", secret, 32)) preferences.secret = secret;
            else preferences.secret = "";

            if (!MTY_JSONObjGetUInt(json, "guestCount", &preferences.guestCount)) {
                preferences.guestCount = 1;
            }

            if (!MTY_JSONObjGetBool(json, "publicRoom", &preferences.publicRoom)) {
                preferences.publicRoom = false;
            }

            static int LIMIT_POS = 4096 * 2;
            if (!MTY_JSONObjGetInt(json, "windowX", &preferences.windowX) || preferences.windowX < -LIMIT_POS || preferences.windowX > LIMIT_POS) {
                preferences.windowX = 0;
            }

            if (!MTY_JSONObjGetInt(json, "windowY", &preferences.windowY) || preferences.windowY < -LIMIT_POS || preferences.windowY > LIMIT_POS) {
                preferences.windowY = 0;
            }

            if (!MTY_JSONObjGetUInt(json, "windowW", &preferences.windowW) || preferences.windowW < 400) {
                preferences.windowW = 1280;
            }

            if (!MTY_JSONObjGetUInt(json, "windowH", &preferences.windowH) || preferences.windowH < 400) {
                preferences.windowH = 720;
            }
            
            preferences.isValid = true;

            MTY_JSONDestroy(&json);
        }
    }

    return preferences;
}

bool MetadataCache::savePreferences(MetadataCache::Preferences preferences)
{
    if (!preferences.isValid)
    {
        return false;
    }

    string dirPath = getUserDir();
    if (!dirPath.empty())
    {
        string filepath = dirPath + "preferences.json";

        MTY_JSON *json = MTY_JSONObjCreate();
            
        MTY_JSONObjSetUInt(json, "audioInputDevice", preferences.audioInputDevice);
        MTY_JSONObjSetUInt(json, "audioOutputDevice", preferences.audioOutputDevice);
        MTY_JSONObjSetUInt(json, "micVolume", preferences.micVolume);
        MTY_JSONObjSetBool(json, "micEnabled", preferences.micEnabled);
        MTY_JSONObjSetUInt(json, "speakersVolume", preferences.speakersVolume);
        MTY_JSONObjSetBool(json, "speakersEnabled", preferences.speakersEnabled);
        MTY_JSONObjSetString(json, "roomName", preferences.roomName.c_str());
        MTY_JSONObjSetString(json, "gameID", preferences.gameID.c_str());
        MTY_JSONObjSetString(json, "secret", preferences.secret.c_str());
        MTY_JSONObjSetUInt(json, "guestCount", preferences.guestCount);
        MTY_JSONObjSetBool(json, "publicRoom", preferences.publicRoom);
        MTY_JSONObjSetInt(json, "windowX", preferences.windowX);
        MTY_JSONObjSetInt(json, "windowY", preferences.windowY);
        MTY_JSONObjSetUInt(json, "windowW", preferences.windowW);
        MTY_JSONObjSetUInt(json, "windowH", preferences.windowH);

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);
            
        return true;
    }

    return false;
}

bool MetadataCache::savePreferences()
{
    return savePreferences(preferences);
}

vector<GuestData> MetadataCache::loadBannedUsers()
{
    vector<GuestData> result;

    string dirPath = getUserDir();
    if (!dirPath.empty())
    {
        string filepath = dirPath + "banned.json";

        if (MTY_FileExists(filepath.c_str()))
        {
            MTY_JSON* json = MTY_JSONReadFile(filepath.c_str());
            uint32_t size = MTY_JSONGetLength(json);

            for (size_t i = 0; i < size; i++)
            {
                const MTY_JSON* guest = MTY_JSONArrayGetItem(json, i);
                
                char name [128] = "";
                uint32_t userID = 0;
                bool nameSuccess = MTY_JSONObjGetString(guest, "name", name, 128);
                bool userIDSuccess = MTY_JSONObjGetUInt(guest, "userID", &userID);

                if (nameSuccess && userIDSuccess)
                {
                    result.push_back(GuestData(name, userID));
                }
            }

            std::sort(result.begin(), result.end(), [](const GuestData a, const GuestData b) {
                return a.userID < b.userID;
            });

            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

bool MetadataCache::saveBannedUsers(vector<GuestData> guests)
{
    string dirPath = getUserDir();

    if (!dirPath.empty())
    {
        string filepath = dirPath + "banned.json";

        MTY_JSON* json = MTY_JSONArrayCreate();

        vector<GuestData>::iterator gi = guests.begin();
        for (; gi != guests.end(); ++gi)
        {
            MTY_JSON* guest = MTY_JSONObjCreate();

            MTY_JSONObjSetString(guest, "name", (*gi).name.c_str());
            MTY_JSONObjSetUInt(guest, "userID", (*gi).userID);
            MTY_JSONArrayAppendItem(json, guest);
        }

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);

        return true;
    }

    return false;
}

vector<GuestTier> MetadataCache::loadGuestTiers()
{
    vector<GuestTier> result;

    string dirPath = getUserDir();
    if (!dirPath.empty())
    {
        string filepath = dirPath + "tiers.json";

        if (MTY_FileExists(filepath.c_str()))
        {
            MTY_JSON* json = MTY_JSONReadFile(filepath.c_str());
            uint32_t size = MTY_JSONGetLength(json);

            for (size_t i = 0; i < size; i++)
            {
                const MTY_JSON* guest = MTY_JSONArrayGetItem(json, i);

                char name[128] = "";
                uint32_t userID, tier = 0;
                bool tierSuccess = MTY_JSONObjGetUInt(guest, "tier", &tier);
                bool userIDSuccess = MTY_JSONObjGetUInt(guest, "userID", &userID);

                if (tierSuccess && userIDSuccess)
                {
                    result.push_back(GuestTier(userID, (Tier)tier));
                }
            }

            std::sort(result.begin(), result.end(), [](const GuestTier a, const GuestTier b) {
                return a.userID < b.userID;
                });

            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

bool MetadataCache::saveGuestTiers(vector<GuestTier> guestTiers)
{
    string dirPath = getUserDir();

    if (!dirPath.empty())
    {
        string filepath = dirPath + "tiers.json";

        MTY_JSON* json = MTY_JSONArrayCreate();

        vector<GuestTier>::iterator gi = guestTiers.begin();
        for (; gi != guestTiers.end(); ++gi)
        {
            MTY_JSON* guest = MTY_JSONObjCreate();

            MTY_JSONObjSetUInt(guest, "tier", (uint32_t)(*gi).tier);
            MTY_JSONObjSetUInt(guest, "userID", (*gi).userID);
            MTY_JSONArrayAppendItem(json, guest);
        }

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);

        return true;
    }

    return false;
}


string MetadataCache::getUserDir()
{
    TCHAR tAppdata[1024];
    if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_APPDATA, NULL, 0, tAppdata)))
    {
        wstring wAppdata(tAppdata);
        string appdata(wAppdata.begin(), wAppdata.end());
        string dirPath = appdata + "\\ParsecSoda\\";
        
        bool isDirOk = false;

        if (!MTY_FileExists(dirPath.c_str()))
        {
            if (MTY_Mkdir(dirPath.c_str()))
            {
                isDirOk = true;
            }
        }
        else
        {
            isDirOk = true;
        }

        if (isDirOk)
        {
            return dirPath;
        }
    }

    return string();
}