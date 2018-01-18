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
    
    for (int i=0; i<desc->outlets.size(); i++) {
        // Note that pins have the same order as the desc->outlets[] array.
        OutletComponent *pin = new OutletComponent(desc->outlets[i].type, desc->outlets[i].direction);
        addAndMakeVisible(pin);
        outlets.add(pin);
    }
}

NodeComponent::~NodeComponent()
{
}

void NodeComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    auto r = getLocalBounds();
    r.reduce(10,10);
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
    for (int i=0; i<desc->outlets.size(); i++) {
        
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
}

void NodeComponent::mouseDrag (const MouseEvent& e)
{
    // Moves this Component according to the mouse drag event and applies our constraints to it
    dragger.dragComponent (this, e, &constrainer);
    S::getInstance().mainComponent->repaint();
}
