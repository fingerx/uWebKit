/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_config.h"
#include "jansson.h"
#include <sstream>

#include "Poco/Path.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"

#include "uwk_process_utils.h"

using Poco::Path;
using Poco::Base64Encoder;
using Poco::Base64Decoder;

std::string UWKConfig::webRenderProcessPath_;
std::string UWKConfig::serverProcessPath_;
std::string UWKConfig::webRenderProcessWorkingPath_;
std::string UWKConfig::dataPath_;
std::string UWKConfig::persistentDataPath_;
std::string UWKConfig::temporaryCachePath_;
std::string UWKConfig::graphicsDeviceVersion_;
std::string UWKConfig::productName_;
std::string UWKConfig::companyName_;

bool UWKConfig::proxyEnabled_ = false;
std::string UWKConfig::proxyHostName_;
std::string UWKConfig::proxyUsername_;
std::string UWKConfig::proxyPassword_;
int UWKConfig::proxyPort_ = 0;

bool UWKConfig::authEnabled_ = false;
std::string UWKConfig::authUsername_;
std::string UWKConfig::authPassword_;

bool UWKConfig::imeEnabled_ = false;
int UWKConfig::serverID_ = 0;

void UWKConfig::GetJSON(std::string& json)
{
    json_t* config = json_object();
    json_t* app = json_object();

    json_object_set(config, "application", app);

    json_object_set(app, "dataPath", json_string(dataPath_.c_str()));
    json_object_set(app, "persistentDataPath", json_string(persistentDataPath_.c_str()));
    json_object_set(app, "temporaryCachePath", json_string(temporaryCachePath_.c_str()));
    json_object_set(app, "serverProcessPath", json_string(serverProcessPath_.c_str()));
    json_object_set(app, "webRenderProcessPath", json_string(webRenderProcessPath_.c_str()));
    json_object_set(app, "webRenderProcessWorkingPath", json_string(webRenderProcessWorkingPath_.c_str()));
    json_object_set(app, "graphicsDeviceVersion", json_string(graphicsDeviceVersion_.c_str()));
    json_object_set(app, "companyName", json_string(companyName_.c_str()));
    json_object_set(app, "productName", json_string(productName_.c_str()));
    json_object_set(app, "imeEnabled", imeEnabled_ ? json_true() : json_false());
    json_object_set(app, "serverID", json_integer(serverID_));

    json_object_set(app, "proxyEnabled", proxyEnabled_ ? json_true() : json_false());
    json_object_set(app, "proxyHostName", json_string(proxyHostName_.c_str()));
    json_object_set(app, "proxyUsername", json_string(proxyUsername_.c_str()));
    json_object_set(app, "proxyPassword", json_string(proxyPassword_.c_str()));
    json_object_set(app, "proxyPort", json_integer(proxyPort_));
    json_object_set(app, "authEnabled", authEnabled_ ? json_true() : json_false());
    json_object_set(app, "authUsername", json_string(authUsername_.c_str()));
    json_object_set(app, "authPassword", json_string(authPassword_.c_str()));

    json = json_dumps(config, JSON_INDENT(4));

    json_decref(config);

}

void UWKConfig::GetSharedMemoryPrefix(std::string& prefix)
{
    prefix.clear();

    if (!companyName_.length() && !productName_.length())
    {
        return;
    }

    if (companyName_.length())
    {
        for (size_t i = 0; i < companyName_.length(); i++)
        {
            if (companyName_[i] != ' ')
                prefix += companyName_[i];
        }
    }

    if (productName_.length())
    {
        if (prefix.length())
            prefix += "_";

        for (size_t i = 0; i < productName_.length(); i++)
        {
            if (productName_[i] != ' ')
                prefix += productName_[i];
        }
    }

    if (prefix.length())
        prefix += "_";


}

void UWKConfig::GetServerProcessPath(std::string& path)
{
    path = serverProcessPath_;
}

void UWKConfig::GetWebRenderProcessPath(std::string& path)
{
    path = webRenderProcessPath_;
}

void UWKConfig::GetWebRenderProcessWorkingPath(std::string& path)
{
    path = webRenderProcessWorkingPath_;
}

void UWKConfig::GetPersistentDataPath(std::string& path)
{
    path = persistentDataPath_;
}

void UWKConfig::GetDataPath(std::string& path)
{
    path = dataPath_;
}

void UWKConfig::GetProductName(std::string& productName)
{
    productName = productName_;
}

void UWKConfig::GetCompanyName(std::string& companyName)
{
    companyName = companyName_;
}


void UWKConfig::GetTemporaryCachePath(std::string& path)
{
    path = temporaryCachePath_;
}

bool UWKConfig::GetIMEEnabled()
{
    return imeEnabled_;
}

int UWKConfig::GetServerID()
{
    return serverID_;
}

bool UWKConfig::InitFromUnityJSON(std::string& json)
{
    if (!UWKProcessUtils::GetExecutablePath(Process::id(), serverProcessPath_))
        return false;

    json_error_t error;
    json_t* unityJSON = json_loads(json.c_str(), JSON_DISABLE_EOF_CHECK, &error);
    if (!unityJSON)
        return false;

    json_t* appConfig = json_object_get(unityJSON, "application");

    if (!appConfig)
    {
        json_decref(unityJSON);
        return false;
    }

    std::string svalue;

    // dataPath
    json_t* jvalue = json_object_get(appConfig, "dataPath");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(unityJSON);
        return false;
    }
    svalue = json_string_value(jvalue);
    SetDataPath(svalue);

    // persistentDataPath
    jvalue = json_object_get(appConfig, "persistentDataPath");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(unityJSON);
        return false;
    }
    svalue = json_string_value(jvalue);
    SetPersistentDataPath(svalue);

    // temporaryCachePath
    jvalue = json_object_get(appConfig, "temporaryCachePath");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(unityJSON);
        return false;
    }
    svalue = json_string_value(jvalue);
    SetTemporaryCachePath(svalue);

    // graphicsDeviceVersion
    jvalue = json_object_get(appConfig, "graphicsDeviceVersion");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(unityJSON);
        return false;
    }
    svalue = json_string_value(jvalue);
    SetGraphicsDeviceVersion(svalue);

    // companyName
    jvalue = json_object_get(appConfig, "companyName");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(unityJSON);
        return false;
    }
    svalue = json_string_value(jvalue);
    SetCompanyName(svalue);

    // productName
    jvalue = json_object_get(appConfig, "productName");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(unityJSON);
        return false;
    }
    svalue = json_string_value(jvalue);
    SetProductName(svalue);

    // imeEnabled
    jvalue = json_object_get(appConfig, "imeEnabled");
    if (!jvalue || !json_is_boolean(jvalue))
    {
        json_decref(unityJSON);
        return false;
    }

    imeEnabled_= json_is_true(jvalue);

    // serverID is not set from Unity JSON and
    // is valid once server is registered with process
    // database

    // Proxy

    // proxyEnabled
    jvalue = json_object_get(appConfig, "proxyEnabled");
    if (!jvalue || !json_is_boolean(jvalue))
    {
        proxyEnabled_ = false;
    }
    else
        proxyEnabled_ = json_is_true(jvalue);

    // proxyHostName
    jvalue = json_object_get(appConfig, "proxyHostName");
    if (!jvalue || !json_is_string(jvalue))
    {
        proxyHostName_ = "";
    }
    else
        proxyHostName_ = json_string_value(jvalue);

    // proxyUsername
    jvalue = json_object_get(appConfig, "proxyUsername");
    if (!jvalue || !json_is_string(jvalue))
    {
        proxyUsername_ = "";
    }
    else
        proxyUsername_ = json_string_value(jvalue);

    // proxyPassword
    jvalue = json_object_get(appConfig, "proxyPassword");
    if (!jvalue || !json_is_string(jvalue))
    {
        proxyPassword_ = "";
    }
    else
        proxyPassword_ = json_string_value(jvalue);

    // proxyPort
    jvalue = json_object_get(appConfig, "proxyPort");
    if (!jvalue || !json_is_integer(jvalue))
    {
        proxyPort_ = 0;
    }
    else
        proxyPort_ = json_integer_value(jvalue);

    // authEnabled
    jvalue = json_object_get(appConfig, "authEnabled");
    if (!jvalue || !json_is_boolean(jvalue))
    {
        authEnabled_ = false;
    }
    else
        authEnabled_ = json_is_true(jvalue);

    // authUsername
    jvalue = json_object_get(appConfig, "authUsername");
    if (!jvalue || !json_is_string(jvalue))
    {
        authUsername_ = "";
    }
    else
        authUsername_ = json_string_value(jvalue);

    // authPassword
    jvalue = json_object_get(appConfig, "authPassword");
    if (!jvalue || !json_is_string(jvalue))
    {
        authPassword_ = "";
    }
    else
        authPassword_ = json_string_value(jvalue);

    // isEditor
    jvalue = json_object_get(appConfig, "isEditor");
    if (!jvalue || !json_is_boolean(jvalue))
    {
        json_decref(unityJSON);
        return false;
    }

    bool isEditor = json_is_true(jvalue);

    std::string dataPath;
    GetDataPath(dataPath);

    std::string execPath;

#ifdef _MSC_VER
#ifdef _WIN64    
    SetWebRenderProcessPath (dataPath + "/StreamingAssets/uWebKit/Windows/x86_64/UWKProcess.exe");
#else
    SetWebRenderProcessPath (dataPath + "/StreamingAssets/uWebKit/Windows/x86/UWKProcess.exe");
#endif    
    SetWebRenderProcessWorkingPath(dataPath);
#else

    if (!isEditor)
    {
#ifndef __x86_64__
        execPath = dataPath + "/Data/StreamingAssets/uWebKit/Mac/x86/UWKProcess.app/Contents/MacOS/UWKProcess";
#else
        execPath = dataPath + "/Data/StreamingAssets/uWebKit/Mac/x86_64/UWKProcess.app/Contents/MacOS/UWKProcess";
#endif        
    }
    else
    {
#ifndef __x86_64__                
        execPath = dataPath + "/StreamingAssets/uWebKit/Mac/x86/UWKProcess.app/Contents/MacOS/UWKProcess";
#else
        execPath = dataPath + "/StreamingAssets/uWebKit/Mac/x86_64/UWKProcess.app/Contents/MacOS/UWKProcess";   
#endif        

    }

    SetWebRenderProcessPath(execPath);

    SetWebRenderProcessWorkingPath(dataPath);
#endif

    json_decref(unityJSON);
    return true;
}

void UWKConfig::GetBase64(std::string& base64)
{
    std::string json;
    GetJSON(json);

    std::ostringstream jsonBase64;
    Base64Encoder encoder(jsonBase64);
    encoder << json;
    encoder.close();
    base64 = jsonBase64.str();
}


bool UWKConfig::SetFromBase64(const std::string& base64)
{
    std::istringstream str(base64);
    Base64Decoder decoder(str);

    std::string json;
    int c = decoder.get();
    while (c != -1) { json += char(c); c = decoder.get(); }

    return SetFromJSON(json);

}

bool UWKConfig::InitDevDefaults()
{

#ifdef UWK_INTERNAL_DEVELOPER_BUILD
    json_error_t error;
    std::string filename = Path::home() + "uwk_debug_args.txt";

    json_t* json = json_load_file(filename.c_str(), JSON_DISABLE_EOF_CHECK, &error);
    if (!json)
        return false;

    json_t* array = json_object_get(json, "args");
    if (!array || !json_is_array(array))
        return false;

    std::string base64Config;
    std::vector<std::string> args;
    for (size_t i = 0; i < json_array_size(array); i++)
    {
        args.push_back(json_string_value(json_array_get(array, i)));
    }

    for (size_t i = 0; i < args.size(); i++)
    {
        if (args.at(i) == "-config" && i + 1 < args.size())
        {
            base64Config = args.at(i + 1);
            break;
        }

    }

    if (!base64Config.length())
        return false;

    if (!SetFromBase64(base64Config))
        return false;

    return true;
#else
    return false;
#endif

}


bool UWKConfig::SetFromJSON(const std::string& json)
{
    json_error_t error;
    json_t* config = json_loads(json.c_str(), JSON_DISABLE_EOF_CHECK, &error);
    if (!config)
        return false;

    json_t* app = json_object_get(config, "application");
    if (!app)
    {
        json_decref(config);
        return false;
    }

    // dataPath
    json_t* jvalue = json_object_get(app, "dataPath");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }
    dataPath_ = json_string_value(jvalue);

    // persistentDataPath
    jvalue = json_object_get(app, "persistentDataPath");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }
    persistentDataPath_ = json_string_value(jvalue);

    // temporaryCachePath
    jvalue = json_object_get(app, "temporaryCachePath");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }
    temporaryCachePath_ = json_string_value(jvalue);

    // serverProcessPath
    jvalue = json_object_get(app, "serverProcessPath");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }
    serverProcessPath_ = json_string_value(jvalue);


    // webRenderProcessPath
    jvalue = json_object_get(app, "webRenderProcessPath");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }
    webRenderProcessPath_ = json_string_value(jvalue);

    // webRenderProcessWorkingPath
    jvalue = json_object_get(app, "webRenderProcessWorkingPath");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }
    webRenderProcessWorkingPath_ = json_string_value(jvalue);


    // graphicsDeviceVersion
    jvalue = json_object_get(app, "graphicsDeviceVersion");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }
    graphicsDeviceVersion_ = json_string_value(jvalue);

    // companyName
    jvalue = json_object_get(app, "companyName");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }
    companyName_ = json_string_value(jvalue);

    // productName
    jvalue = json_object_get(app, "productName");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }
    productName_ = json_string_value(jvalue);

    // imeEnabled
    jvalue = json_object_get(app, "imeEnabled");
    if (!jvalue || !json_is_boolean(jvalue))
    {
        json_decref(config);
        return false;
    }

    imeEnabled_ = json_is_true(jvalue);

    // serverID
    jvalue = json_object_get(app, "serverID");
    if (!jvalue || !json_is_integer(jvalue))
    {
        json_decref(config);
        return false;
    }

    serverID_ = json_integer_value(jvalue);

    // proxyEnabled
    jvalue = json_object_get(app, "proxyEnabled");
    if (!jvalue || !json_is_boolean(jvalue))
    {
        json_decref(config);
        return false;
    }

    proxyEnabled_ = json_is_true(jvalue);

    // proxyHostName
    jvalue = json_object_get(app, "proxyHostName");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }

    proxyHostName_ = json_string_value(jvalue);

    // proxyUsername
    jvalue = json_object_get(app, "proxyUsername");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }

    proxyUsername_ = json_string_value(jvalue);

    // proxyPassword
    jvalue = json_object_get(app, "proxyPassword");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }

    proxyPassword_ = json_string_value(jvalue);

    // proxyPort
    jvalue = json_object_get(app, "proxyPort");
    if (!jvalue || !json_is_integer(jvalue))
    {
        json_decref(config);
        return false;
    }

    proxyPort_ = json_integer_value(jvalue);

    // authEnabled
    jvalue = json_object_get(app, "authEnabled");
    if (!jvalue || !json_is_boolean(jvalue))
    {
        json_decref(config);
        return false;
    }

    authEnabled_ = json_is_true(jvalue);

    // authUsername
    jvalue = json_object_get(app, "authUsername");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }

    authUsername_ = json_string_value(jvalue);

    // authPassword
    jvalue = json_object_get(app, "authPassword");
    if (!jvalue || !json_is_string(jvalue))
    {
        json_decref(config);
        return false;
    }

    authPassword_ = json_string_value(jvalue);

    json_decref(config);
    return true;

}

void UWKConfig::SetServerProcessPath(const std::string& path)
{
    serverProcessPath_ = path;
}


void UWKConfig::SetWebRenderProcessPath(const std::string& path)
{
    webRenderProcessPath_ = path;
}

void UWKConfig::SetWebRenderProcessWorkingPath(const std::string& path)
{
    webRenderProcessWorkingPath_ = path;
}

void UWKConfig::SetPersistentDataPath(const std::string& path)
{
    persistentDataPath_ = path;
}

void UWKConfig::SetTemporaryCachePath(const std::string& path)
{
    temporaryCachePath_ = path;
}

void UWKConfig::SetDataPath(const std::string& path)
{
    dataPath_ = path;
}

void UWKConfig::GetGraphicsDeviceVersion(std::string& deviceVersion)
{
    deviceVersion = graphicsDeviceVersion_;
}
void UWKConfig::SetGraphicsDeviceVersion(const std::string& deviceVersion)
{
    graphicsDeviceVersion_ = deviceVersion;
}

void UWKConfig::SetProductName(const std::string& productName)
{
    productName_ = productName;
}

void UWKConfig::SetCompanyName(const std::string& companyName)
{
    companyName_ = companyName;
}

bool UWKConfig::IsDirect3D9()
{

    if (graphicsDeviceVersion_.find("Direct3D 9") != std::string::npos)
        return true;

    return false;

}

bool UWKConfig::IsDirect3D11()
{
    if (graphicsDeviceVersion_.find("Direct3D 11") != std::string::npos)
        return true;

    return false;

}

bool UWKConfig::IsOpenGL()
{
    if (graphicsDeviceVersion_.find("OpenGL") != std::string::npos)
        return true;

    return false;
}

bool UWKConfig::IMEEnabled()
{
    return imeEnabled_;
}

void UWKConfig::SetIMEEnabled(bool imeEnabled)
{
    imeEnabled_ = imeEnabled;
}

void UWKConfig::SetServerID(int serverID)
{
    serverID_ = serverID;
}

bool UWKConfig::GetProxyEnabled()
{
    return proxyEnabled_;
}

void UWKConfig::GetProxyHostname(std::string& hostName)
{
    hostName = proxyHostName_;
}

void UWKConfig::GetProxyUsername(std::string& username)
{
    username = proxyUsername_;
}

void UWKConfig::GetProxyPassword(std::string& password)
{
    password = proxyPassword_;
}

int  UWKConfig::GetProxyPort()
{
    return proxyPort_;
}

bool UWKConfig::GetAuthEnabled()
{
    return authEnabled_;
}

void UWKConfig::GetAuthUsername(std::string& username)
{
    username = authUsername_;
}

void UWKConfig::GetAuthPassword(std::string& password)
{
    password = authPassword_;
}

void UWKConfig::SetProxyEnabled(bool value)
{
    proxyEnabled_ = value;
}

void UWKConfig::SetProxyHostname(const std::string& hostName)
{
    proxyHostName_ = hostName;
}

void UWKConfig::SetProxyUsername(const std::string& username)
{
    proxyUsername_ = username;
}

void UWKConfig::SetProxyPassword(const std::string& password)
{
    proxyPassword_ = password;
}

void UWKConfig::SetProxyPort(int port)
{
    proxyPort_ = port;
}

void UWKConfig::SetAuthEnabled(bool value)
{
    authEnabled_ = value;
}

void UWKConfig::SetAuthUsername(const std::string& username)
{
    authUsername_ = username;
}

void UWKConfig::SetAuthPassword(const std::string& password)
{
    authPassword_ = password;
}



