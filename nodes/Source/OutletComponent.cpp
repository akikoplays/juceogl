/*
  ==============================================================================

    OutletComponent.cpp
    Created: 10 Jan 2018 1:25:29pm
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OutletComponent.h"
#include "MainComponent.h"

using namespace std;

#define BLINK_FREQ_HZ 4
#define STRAIN_RELIEF_OFFSET 20 // Offset in pixels

//==============================================================================
OutletComponent::OutletComponent(NodeComponent* parent, OutletDesc *_odesc, OutletParamBlock::Type type, OutletParamBlock::Direction direction)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    odesc = _odesc;
    desc.type = type;
    desc.direction = direction;
    signalizing = false;
    signalState = false;
    if (direction == OutletParamBlock::Direction::SINK)
        baseColor = Colour(0xffaaaaff);
    else
        baseColor = Colour(0xffffaaaa);
    activeColor = baseColor;
    node = parent;
    assert(node);
}

OutletComponent::~OutletComponent()
{
}

void OutletComponent::mouseDown(const MouseEvent& e)
{
    cout << "Mouse down on outlet" << endl;
    if (e.mods.isRightButtonDown()){
        S::getMainComponent()->selectOutlet(this, true);
    }
}

void OutletComponent::mouseUp(const MouseEvent& e)
{
    // RMB? Ignore
    if (e.mods.isRightButtonDown())
        return;
    
    // Select outlet
    S::getConsole()->print("Outlet selected: " + getName());
    S::getMainComponent()->selectOutlet(this);
}

void OutletComponent::mouseEnter(const MouseEvent& e)
{
}

void OutletComponent::mouseDoubleClick(const MouseEvent &event)
{
}

bool OutletComponent::addCable(Cable *cable)
{
    if (hasCable(cable))
        return false;
    
    cables.push_back(cable);
    return true;
}

bool OutletComponent::removeCable(Cable *cable)
{
    for (auto it = cables.begin(); it != cables.end(); it++){
        Cable *c = *it;
        if (c == cable) {
            cables.erase(it);
            return true;
        }
    }
    return false;
}

void OutletComponent::removeAllCables()
{
    cables.clear();
}

bool OutletComponent::hasCable(Cable *cable)
{
    return std::find(cables.begin(), cables.end(), cable) != cables.end();
}

void OutletComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    g.fillAll (activeColor);   // clear the background
}

void OutletComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

Point<int> OutletComponent::getWindowPos()
{
    // TODO
    // new way of proper relative coords fetch
    auto r = node->getBoundsInParent();
    auto r2 = getBoundsInParent();
    
//    return Point<int>(r.getX() + r2.getX(), r.getY() + r2.getY() + getHeight()/2);
    if (isSink())
        return Point<int>(r.getX() + r.getWidth(), r.getY() + r2.getY() + getHeight()/2);
    else
        return Point<int>(r.getX(), r.getY() + r2.getY() + getHeight()/2);
}

void OutletComponent::timerCallback()
{
    if (signalizing) {
        if (signalState)
            activeColor = signalColor;
        else
            activeColor = baseColor;
        signalState = signalState ? false : true;
    }
    repaint();
}

void OutletComponent::signalize(Colour c, bool ena)
{
    if (ena == false) {
        signalizing = false;
        activeColor = baseColor;
        stopTimer();
    } else {
        signalColor = c;
        signalizing = true;
        signalState = true;
        startTimerHz(BLINK_FREQ_HZ);
    }
    repaint();
}

Point<int> OutletComponent::getStrainReliefPos()
{
    // Add 'strain relief' offset
    if (isSource())
        return getWindowPos() - Point<int>(STRAIN_RELIEF_OFFSET, 0);
    else
        return getWindowPos() + Point<int>(STRAIN_RELIEF_OFFSET, 0);
}

bool OutletComponent::isRatingCompatible(OutletComponent *outlet2)
{
    NodeComponent *parent = dynamic_cast<NodeComponent*>(getParentComponent());
    assert(parent);
    OutletDesc *desc1 = parent->getOutletDescByOutlet(this);
    assert(desc1);
    
    NodeComponent *parent2 = dynamic_cast<NodeComponent*>(outlet2->getParentComponent());
    assert(parent2);
    OutletDesc *desc2 = parent2->getOutletDescByOutlet(outlet2);
    assert(desc2);

    auto Min = std::max(desc1->rating[0], desc2->rating[0]);
    auto Max = std::min(desc1->rating[1], desc2->rating[1]);
    if (Min <= Max) {
        // There's an intersection. represented by {Min Max}
        cout << "Rating intersection OK: [" << Min << ", " << Max << "]" << std::endl;
        return true;
    }
    return false;
}

NodeComponent *OutletComponent::getNode()
{
    return node;
}

OutletDesc *OutletComponent::getOutletDesc()
{
    return odesc;
}
