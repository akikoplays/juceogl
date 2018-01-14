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

//==============================================================================
OutletComponent::OutletComponent(OutletDesc::Type type, OutletDesc::Direction direction)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    desc.type = type;
    desc.direction = direction;
}

OutletComponent::~OutletComponent()
{
}

void OutletComponent::mouseDown (const MouseEvent& e)
{
    cout << "Mouse down on outlet" << endl;
    if (isLocked()) {
        cout << "-- already locked, ignore" << endl;
    } else {
        cout << "-- selecting outlet" << endl;
        S::getInstance().mainComponent->selectOutlet(this);
    }
}

void OutletComponent::mouseEnter (const MouseEvent& e)
{
    cout << "Mouse entered outlet" << endl;
}

bool OutletComponent::isLocked()
{
    return locked;
}

bool OutletComponent::lock()
{
    if (locked)
        return false;
    locked = true;
    return true;
}

bool OutletComponent::release()
{
    if (locked == false)
        return false;
    locked = false;
    return true;
}

void OutletComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    
    g.fillAll (isLocked()? Colours::green : Colours::red);   // clear the background

    Component *w = findParentComponentOfClass<DocumentWindow>();
    auto wr = w->getBoundsInParent();
    Point<int> ret(0,0);
    windowPosition.x = getScreenX() - wr.getX() + getWidth()/2;
    windowPosition.y = getScreenY() - wr.getY() + getHeight()/2;
}

void OutletComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    cout << "outlet pos: " << windowPosition.x << " " << windowPosition.y << endl;
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
