
#include "LEDController.h"
#include "DemoOne.h"
#include "Delegate.h"
#include "AppConfiguration.h"


extern AppType          app;
ButtonPressedDelegate   buttonDelegate(app);
ButtonType              button(buttonDelegate);
LEDControllerType       ledController;
AppType                 app(ledController);





//
// Entry point for the application.
// 
// Note1: The main application object is created static within main instead
// of being global in order to control initialisation-time.
// Note2: 'Run' methods are one-shot. The loop is implicit to allow for transition
// between threaded and non-threaded mechanisms.
//
extern "C" void AppMain()
{
    button.Poll();
    
    while(true)
    {
        app.Run();
    }
}




