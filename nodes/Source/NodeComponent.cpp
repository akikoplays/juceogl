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
void SnapConstraint::applyBoundsToComponent (Component &component, Rectangle<int> bounds)
{
    // Original code:
//    component.setBounds (bounds);
    // Snap to grid code:
    int grid = 20;
    int x = bounds.getX() / grid * grid;
    int y = bounds.getY() / grid * grid;
    bounds.setX(x);
    bounds.setY(y);
    component.setBounds(bounds);
}


//==============================================================================
NodeComponent::NodeComponent(ComponentDesc *_desc, Uuid _uuid)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    desc = _desc;
    selected = false;
    
    // When node is loaded from layout file, Uuid is provided, otherwise it is calculated.
    if (_uuid.isNull())
        uuid = Uuid();
    else
        uuid = _uuid;
    
    cout << "Creating node instance for: " << desc->name << endl;
    setName(desc->name);
    
    for (int i=0; i<desc->outlets.size(); i++) {
        // Note that pins have the same order as the desc->outlets[] array.
        OutletComponent *pin = new OutletComponent(this,
                                                   desc->outlets[i],
                                                   desc->outlets[i]->type,
                                                   desc->outlets[i]->direction);
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

    g.setColour (selected ? Colours::white : Colours::grey);
    g.drawRect (r, 1);   // draw an outline around the component

    // Dynamically painted outlets based on their descriptors
    r = getLocalBounds();
    int numSrcs = 0;
    for (int i=0; i<outlets.size(); i++)
        numSrcs = desc->outlets[i]->direction == OutletParamBlock::Direction::SOURCE ? numSrcs + 1 : numSrcs;
    int numSinks = outlets.size() - numSrcs;
    int sinkstarty = r.getHeight()/2-OUTLET_SIZE.y/2 - numSinks/2 * (OUTLET_SIZE.y+2);
    int stepy = OUTLET_SIZE.y + 2;
    int srcstarty = r.getHeight()/2-OUTLET_SIZE.y/2 - numSrcs/2 * (OUTLET_SIZE.y+2);

    for (int i=0; i<outlets.size(); i++) {
        Rectangle<int> src;
        int startx = 0;
        if (desc->outlets[i]->direction == OutletParamBlock::Direction::SINK) {
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
    
    g.setColour (Colours::white);
    g.setFont (12.0f);
    g.drawFittedText(desc->name, outletSize.x-4, 0, getWidth()-outletSize.x-10, getHeight(), Justification::verticallyCentred | Justification::horizontallyCentred, 3, 1.0);

}

void NodeComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());

}

void NodeComponent::mouseDown (const MouseEvent& e)
{
    // Node options are shown on RMB
    if (e.mods.isRightButtonDown()) {
        S::getInstance().getMainComponent()->selectNode(this, true);
        return;
    }
    
    // Select node
    if (!e.mods.isShiftDown()) {
        S::getMainComponent()->deselectAll();
    }
    select();
    S::getInstance().getMainComponent()->selectNode(this);

    // Prepares our dragger to drag this Component
    mouseDownWithinTarget = e.getEventRelativeTo(this).getMouseDownPosition();
    dragger.startDraggingComponent (this, e);
}

void NodeComponent::mouseUp (const MouseEvent& e)
{
    // RMB? ignore
    if (e.mods.isRightButtonDown())
        return;
}

void NodeComponent::mouseDrag (const MouseEvent& e)
{
    // Moves this Component according to the mouse drag event and applies our constraints to it
    dragger.dragComponent (this, e, &constrainer);
    S::getMainComponent()->repaint();

    // TODO
    // fix multi drag when snapping on! constrainer is snapgridconstrainer, so the movement is quantized
    
    // TODO
    // logic for multi dragging
    
    // Tiny movements don't start a drag
    const int minimumMovementToStartDrag = 20;
    if (e.getDistanceFromDragStart() < minimumMovementToStartDrag)
        return;
    
    Point<int> delta = e.getEventRelativeTo(this).getPosition() - mouseDownWithinTarget;
    //cout << "delta drag: " << delta.x << ", " << delta.y << endl;
    S::getMainComponent()->moveSelectedNodes(this, delta);
    
    MouseEvent re (e.getEventRelativeTo(S::getMainComponent()));
    S::getMainComponent()->getViewport().autoScroll(re.x, re.y, 10,8);
}

void NodeComponent::mouseDoubleClick(const MouseEvent &event)
{
}

const OwnedArray<OutletComponent>& NodeComponent::getOutlets()
{
    return outlets;
};

OutletDesc *NodeComponent::getOutletDescByOutlet(OutletComponent *outlet)
{
    int idx = outlets.indexOf(outlet);
    assert(idx>=0);
    // TODO check this warning.. returning reference to local temporary object
    return desc->outlets[idx];
}

OutletComponent *NodeComponent::getOutletByName(String name)
{
    for (auto outlet: outlets) {
        if (outlet->getOutletDesc()->name == name)
            return outlet;
    }
    return nullptr;
}

void NodeComponent::select()
{
    cout << "Node selected." << endl;
    selected = true;
    repaint();
}

void NodeComponent::deselect()
{
    cout << "Node deselected." << endl;
    selected = false;
    repaint();
}

ValueTree NodeComponent::serialize()
{
    ValueTree child = ValueTree("node");
    child.setProperty("uuid", uuid.toString(), nullptr);
    child.setProperty("librarian-id", this->desc->cid, nullptr);
    child.setProperty("name", getName(), nullptr);
    child.setProperty("x", getX(), nullptr);
    child.setProperty("y", getY(), nullptr);
    return child;
}

Uuid NodeComponent::getUuid()
{
    return uuid;
}
