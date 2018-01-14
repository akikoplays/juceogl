/*
  ==============================================================================

    OutletComponent.h
    Created: 10 Jan 2018 1:25:29pm
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class OutletDesc {
public:
    enum Type {
        POWER_BUS = 0,
        BINARY,
        COMM
    };
    
    enum Direction {
        SOURCE = 0,
        SINK
    };

    Type type;
    Direction direction;

};

//==============================================================================
/*
*/
class OutletComponent    : public Component
{
public:
    
    OutletComponent(OutletDesc::Type type, OutletDesc::Direction direction);
    ~OutletComponent();

    void paint (Graphics&) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseEnter (const MouseEvent& e) override;
    bool isLocked();
    bool lock();
    bool release();
    Point<int> getWindowPos();

private:
    OutletDesc desc;
    bool locked;
    Point<int> windowPosition;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutletComponent)
};
