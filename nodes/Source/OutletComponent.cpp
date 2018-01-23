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
OutletComponent::OutletComponent(OutletParamBlock::Type type, OutletParamBlock::Direction direction)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    desc.type = type;
    desc.direction = direction;
    signalizing = false;
    signalState = false;
    if (direction == OutletParamBlock::Direction::SINK)
        baseColor = Colours::green;
    else
        baseColor = Colours::blue;
    activeColor = baseColor;
}

OutletComponent::~OutletComponent()
{
}

void OutletComponent::mouseDown(const MouseEvent& e)
{
    cout << "Mouse down on outlet" << endl;
    mouseDownStartTime = Time::getCurrentTime().toMilliseconds();
}

void OutletComponent::mouseUp(const MouseEvent& e)
{
    // Take it as single click
    if (Time::getCurrentTime().toMilliseconds() - mouseDownStartTime > 100) {
        cout << "Outlet selected." << endl;
        S::getMainComponent()->selectOutlet(this);
    }
}

void OutletComponent::mouseEnter(const MouseEvent& e)
{
    cout << "Mouse entered outlet" << endl;
}

void OutletComponent::mouseDoubleClick(const MouseEvent &event)
{
    S::getMainComponent()->selectOutlet(this, true);
}

bool OutletComponent::addCable(Connection *cable)
{
    if (hasCable(cable))
        return false;
    
    cables.push_back(cable);
    return true;
}

bool OutletComponent::removeCable(Connection *cable)
{
    for (auto it = cables.begin(); it != cables.end(); it++){
        Connection *c = *it;
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

bool OutletComponent::hasCable(Connection *cable)
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

    // Calulate new position
    Component *w = findParentComponentOfClass<DocumentWindow>();
    auto wr = w->getBoundsInParent();
    Point<int> ret(0,0);
    windowPosition.x = getScreenX() - wr.getX() + getWidth()/2;
    windowPosition.y = getScreenY() - wr.getY() + getHeight()/2;
    
//    cout << "outlet pos: " << windowPosition.x << " " << windowPosition.y << endl;

}

void OutletComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

Point<int> OutletComponent::getWindowPos()
{
    return windowPosition;
    
    // If previous doesn't work, this will for sure.
//    auto r = getBoundsInParent();
//    auto r2 = getParentComponent()->getBoundsInParent();
//
//    cout << "outlet pos: " << r.getX() + r2.getX() << " " << r.getY() + r2.getY() << endl;
//    return Point<int>(r.getX() + r2.getX() + getWidth()/2, r.getY() + r2.getY() + getHeight()/2);
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
        return Point<int>(windowPosition.x - STRAIN_RELIEF_OFFSET, windowPosition.y);
    else
        return Point<int>(windowPosition.x + STRAIN_RELIEF_OFFSET, windowPosition.y);
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
