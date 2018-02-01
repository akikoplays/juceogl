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
class NodeComponent;
class OutletDesc;

class OutletParamBlock {
public:
    enum Type {
        POWER_BUS = 0,  // AC bus
        POWER_DCBUS,    // DC bus
        BINARY,         // 1/0 true/false binary switch
        COMM            // can, uart, ...
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
    
    OutletComponent(NodeComponent *parent, OutletDesc *_odesc, OutletParamBlock::Type type, OutletParamBlock::Direction direction);
    ~OutletComponent();

    void paint (Graphics&) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent &event) override;
    void timerCallback() override;

    Point<int> getWindowPos();
    bool isSource() {return desc.direction == OutletParamBlock::Direction::SOURCE;};
    bool isSink() {return desc.direction == OutletParamBlock::Direction::SINK;};
    OutletParamBlock::Type getType() {return desc.type;};
    OutletParamBlock::Direction getDirection() {return desc.direction;};
    // Checks if this outlet's power rating is compatible with the given outlet's rating. Outlets may be of fixed or ranged voltage levels,
    // but it's only important that they overlap.
    bool isRatingCompatible(OutletComponent *outlet2);
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
    // Return parent node.
    NodeComponent *getNode();
    // Return outlet descriptor.
    OutletDesc *getOutletDesc();
    
private:
    // Parent node component containing this outlet.
    NodeComponent *node;
    // Each connection established with the outlet is added to this vector.
    std::vector<Connection *> cables;
    OutletParamBlock desc;
    // Pointer to Outlet descriptor as provided by Librarian, used for fast lookup
    OutletDesc *odesc;
    bool signalizing;
    bool signalState;
    Colour signalColor;
    Colour baseColor;
    Colour activeColor;
    Point<int> windowPosition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutletComponent)
};
