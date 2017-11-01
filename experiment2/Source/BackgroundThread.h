/*
  ==============================================================================

    BackgroundThread.h
    Created: 1 Nov 2017 10:44:09am
    Author:  BorisP

  ==============================================================================
*/

#pragma once

/*
  ==============================================================================

    BackgroundThread.cpp
    Created: 1 Nov 2017 10:44:09am
    Author:  BorisP

  ==============================================================================
*/

#include "BackgroundThread.h"

//==============================================================================
class BackgroundThread  : public ThreadWithProgressWindow
{
public:
    BackgroundThread();
    void run() override;
    // This method gets called on the message thread once our thread has finished..
    void threadComplete (bool userPressedCancel) override;
};
