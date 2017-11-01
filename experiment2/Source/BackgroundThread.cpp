/*
 ==============================================================================
 
 BackgroundThread.cpp
 Created: 1 Nov 2017 10:44:09am
 Author:  BorisP
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"
#include "BackgroundThread.h"

//==============================================================================
BackgroundThread::BackgroundThread()
: ThreadWithProgressWindow ("busy doing some important things...", true, true)
{
    setStatusMessage ("Getting ready...");
}

void BackgroundThread::run()
{
    setProgress (-1.0); // setting a value beyond the range 0 -> 1 will show a spinning bar..
    setStatusMessage ("Preparing to do some stuff...");
    wait (2000);
    
    const int thingsToDo = 10;
    
    for (int i = 0; i < thingsToDo; ++i)
    {
        // must check this as often as possible, because this is
        // how we know if the user's pressed 'cancel'
        if (threadShouldExit())
            return;
        
        // this will update the progress bar on the dialog box
        setProgress (i / (double) thingsToDo);
        
        setStatusMessage (String (thingsToDo - i) + " things left to do...");
        
        wait (500);
    }
    
    setProgress (-1.0); // setting a value beyond the range 0 -> 1 will show a spinning bar..
    setStatusMessage ("Finishing off the last few bits and pieces!");
    wait (2000);
}

// This method gets called on the message thread once our thread has finished..
void BackgroundThread::threadComplete (bool userPressedCancel)
{
    if (userPressedCancel)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Progress window",
                                          "You pressed cancel!");
    }
    else
    {
        // thread finished normally..
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Progress window",
                                          "Thread finished ok!");
    }
    
    // ..and clean up by deleting our thread object..
    delete this;
}
