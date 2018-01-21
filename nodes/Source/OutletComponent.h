/*
  ==============================================================================

    OutletComponent.h
    Created: 10 Jan 2018 1:25:29pm
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Connection;

class OutletParamBlock {
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
class OutletComponent    :  public Component,
                            private Timer
{
public:
    
    OutletComponent(OutletParamBlock::Type type, OutletParamBlock::Direction direction);
    ~OutletComponent();

    void paint (Graphics&) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent &event) override;
    void timerCallback() override;

    Point<int> getWindowPos();
    bool isSource() {return desc.direction == OutletParamBlock::Direction::SOURCE;};
    bool isSink() {return desc.direction == OutletParamBlock::Direction::SINK;};
    OutletParamBlock::Type getType() {return desc.type;};
    OutletParamBlock::Direction getDirection() {return desc.direction;};
    bool addCable(Connection *cable);
    // Removes specific cable. Warning: doesn't deallocate it!
    bool removeCable(Connection *cable);
    // Removes all cables. Warning: doesn't deallocate them!
    void removeAllCables();
    // Returns list of cables for this outlet.
    std::vector<Connection *> getCables();
    // Check if this connection already inserted.
    bool hasCable(Connection *cable);
    // Returns true if outlet contains cables.
    bool isConnected(){return cables.size() > 0;};
    void signalize(Colour c, bool ena);
    // Return the offset from outlet to first cable twist.
    Point<int> getStrainReliefPos();
    
private:
    // Each connection established with the outlet is added to this vector.
    std::vector<Connection *> cables;
    OutletParamBlock desc;
    bool signalizing;
    bool signalState;
    Colour signalColor;
    Colour baseColor;
    Colour activeColor;
    Point<int> windowPosition;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutletComponent)
};
