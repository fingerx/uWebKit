/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include <string>

class UWKConfig
{    
    static std::string webRenderProcessPath_;
    static std::string serverProcessPath_;
    static std::string webRenderProcessWorkingPath_;
    static std::string persistentDataPath_;
    static std::string dataPath_;
    static std::string temporaryCachePath_;
    static std::string graphicsDeviceVersion_;
    static std::string companyName_;
    static std::string productName_;

    static bool proxyEnabled_;
    static std::string proxyHostName_;
    static std::string proxyUsername_;
    static std::string proxyPassword_;
    static int proxyPort_;

    static bool authEnabled_;
    static std::string authUsername_;
    static std::string authPassword_;

    static bool imeEnabled_;
    static int serverID_;

public:

    // TODO: proxy info

    static void GetJSON(std::string& json);
    static void GetBase64(std::string& base64);

    static void GetServerProcessPath(std::string& path);

    static void GetWebRenderProcessPath(std::string& path);
    static void GetWebRenderProcessWorkingPath(std::string& path);

    static void GetProductName(std::string& productName);
    static void GetCompanyName(std::string& companyName);

    static void GetDataPath(std::string& path);
    static void GetPersistentDataPath(std::string& path);
    static void GetTemporaryCachePath(std::string& path);
    static void GetGraphicsDeviceVersion(std::string& deviceVersion);
    static bool GetIMEEnabled();
    static int  GetServerID();

    static bool GetProxyEnabled();
    static void GetProxyHostname(std::string& hostName);
    static void GetProxyUsername(std::string& username);
    static void GetProxyPassword(std::string& password);
    static int  GetProxyPort();

    static bool GetAuthEnabled();
    static void GetAuthUsername(std::string& username);
    static void GetAuthPassword(std::string& password);

    // a combination of Company and Product name
    // which provides a folder as to not conflict
    // with other products using shared memory
    static void GetSharedMemoryPrefix(std::string& prefix);

    static bool InitFromUnityJSON(std::string& json);
    static bool InitDevDefaults();

    static bool SetFromJSON(const std::string& json);
    static bool SetFromBase64(const std::string& base64);

    static void SetServerProcessPath(const std::string& path);
    static void SetWebRenderProcessPath(const std::string& path);
    static void SetWebRenderProcessWorkingPath(const std::string& path);
    static void SetDataPath(const std::string& path);
    static void SetPersistentDataPath(const std::string& path);
    static void SetTemporaryCachePath(const std::string& path);
    static void SetGraphicsDeviceVersion(const std::string& graphicsVersion);
    static void SetIMEEnabled(bool imeEnabled);
    static void SetServerID(int serverID);

    static void SetCompanyName(const std::string& companyName);
    static void SetProductName(const std::string& productName);

    // Proxy
    static void SetProxyEnabled(bool value);
    static void SetProxyHostname(const std::string& hostName);
    static void SetProxyUsername(const std::string& username);
    static void SetProxyPassword(const std::string& password);
    static void SetProxyPort(int port);

    static void SetAuthEnabled(bool value);
    static void SetAuthUsername(const std::string& username);
    static void SetAuthPassword(const std::string& password);

    static bool IsDirect3D9();
    static bool IsDirect3D11();
    static bool IsOpenGL();

    static bool IMEEnabled();

};
