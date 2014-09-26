/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "UWKCommon/uwk_log.h"
#include "Poco/DateTime.h"
#include "Poco/TimeSpan.h"

#include "uwk_unity_beta.h"

#ifdef UWK_UNITY_BETA

#define UWK_UNITY_BETA_YEAR 2014
#define UWK_UNITY_BETA_MONTH 7
#define UWK_UNITY_BETA_DAY 25

int UWKUnityBeta::GetBetaDaysLeft()
{
#ifndef UWK_UNITY_BETA

    return 0;

#else

    Poco::DateTime currentTime;
    Poco::DateTime betaTime(UWK_UNITY_BETA_YEAR, UWK_UNITY_BETA_MONTH, UWK_UNITY_BETA_DAY);

    if (currentTime < betaTime)
        return -1;

    Poco::Timespan diff = currentTime - betaTime;

    // beta is good for 3 weeks

    if (diff.days() > 21)
        return -1;

    return 21 - diff.days();

#endif

}




#endif
