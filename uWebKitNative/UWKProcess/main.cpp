/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <string>
#include "uwk_application.h"

int main(int argc, char *argv[])
{
    UWK::UWKApplication app(argc, argv);

    app.init();

    int ret = app.exec();

    app.shutdown();

    return ret;
}
