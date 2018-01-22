/*
  ==============================================================================

    NodeComponent.cpp
    Created: 10 Jan 2018 12:57:52pm
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "OutletComponent.h"
#include "LibrarianComponent.h"
#include "NodeComponent.h"

using namespace std;

#define OUTLET_SIZE Point<int>(15, 15)

//==============================================================================
NodeComponent::NodeComponent(ComponentDesc *_desc)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

//    OutletComponent *pin = new OutletComponent(OutletDesc::Type::POWER_BUS, OutletDesc::Direction::SOURCE);
//    addAndMakeVisible(pin);
//    outlets.add(pin);
//
//    pin = new OutletComponent(OutletDesc::Type::POWER_BUS, OutletDesc::Direction::SINK);
//    addAndMakeVisible(pin);
//    outlets.add(pin);

    desc = _desc;
    
    cout << "Creating node instance for: " << desc->name << endl;
    setName(desc->name);
    
    for (int i=0; i<desc->outlets.size(); i++) {
        // Note that pins have the same order as the desc->outlets[] array.
        OutletComponent *pin = new OutletComponent(desc->outlets[i].type, desc->outlets[i].direction);
        addAndMakeVisible(pin);
        outlets.add(pin);
    }
}

NodeComponent::~NodeComponent()
{
    cout << "Node " << getName() << " deconstructor called" << endl;
}

void NodeComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    Point<int>outletSize = OUTLET_SIZE;

    auto r = getLocalBounds();
    r.reduce(outletSize.x, outletSize.y);
    g.setColour(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.fillRect(r);

    g.setColour (Colours::grey);
    g.drawRect (r, 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (10.0f);
    g.drawText (desc->name, getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
    
//    // Sink
//    r = getLocalBounds();
//    Rectangle<int> sink;
//    sink.setBounds(0, r.getHeight()/2-5, 10, 10);
//    outlets[0]->setBounds(sink);
//    // Src
//    r = getLocalBounds();
//    Rectangle<int> src;
//    src.setBounds(r.getWidth()-10, r.getHeight()/2-5, 10, 10);
//    outlets[1]->setBounds(src);
    
    // TODO:
    // Dynamically painted outlets based on their descriptors
    
    
    r = getLocalBounds();
    int numSrcs = 0;
    for (int i=0; i<outlets.size(); i++)
        numSrcs = desc->outlets[i].direction == OutletParamBlock::Direction::SOURCE ? numSrcs + 1 : numSrcs;
    int numSinks = outlets.size() - numSrcs;
    int sinkstarty = r.getHeight()/2-OUTLET_SIZE.y/2 - numSinks/2 * (OUTLET_SIZE.y+2);
    int stepy = OUTLET_SIZE.y + 2;
    int srcstarty = r.getHeight()/2-OUTLET_SIZE.y/2 - numSrcs/2 * (OUTLET_SIZE.y+2);

    for (int i=0; i<outlets.size(); i++) {
        Rectangle<int> src;
        int startx = 0;
        if (desc->outlets[i].direction == OutletParamBlock::Direction::SINK) {
            startx = r.getWidth()-OUTLET_SIZE.x;
            src.setBounds(startx, sinkstarty, OUTLET_SIZE.x, OUTLET_SIZE.y);
            outlets[i]->setBounds(src);
            sinkstarty += stepy;
        } else {
            startx = 0;
            src.setBounds(startx, srcstarty, OUTLET_SIZE.x, OUTLET_SIZE.y);
            outlets[i]->setBounds(src);
            srcstarty += stepy;
        }
    }
}

void NodeComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());

}

void NodeComponent::mouseDown (const MouseEvent& e)
{
    // Prepares our dragger to drag this Component
    dragger.startDraggingComponent (this, e);
    mouseDownStartTime = Time::getCurrentTime().toMilliseconds();
}

void NodeComponent::mouseUp (const MouseEvent& e)
{
    // Take it as single click
    if (Time::getCurrentTime().toMilliseconds() - mouseDownStartTime < 180) {
        cout << "Selected." << endl;
    }
}

void NodeComponent::mouseDrag (const MouseEvent& e)
{
    // Moves this Component according to the mouse drag event and applies our constraints to it
    dragger.dragComponent (this, e, &constrainer);
    S::getMainComponent()->repaint();
}

void NodeComponent::mouseDoubleClick(const MouseEvent &event)
{
    S::getMainComponent()->selectNode(this, true);
}

const OwnedArray<OutletComponent>& NodeComponent::getOutlets()
{
    return outlets;
};
