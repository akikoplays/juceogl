/*
  ==============================================================================

    ButtonBar.cpp
    Created: 23 Oct 2017 12:16:41pm
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ButtonBar.h"
#include "HelperBar.h"

using namespace std;

//==============================================================================
ButtonBar::ButtonBar()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setInterceptsMouseClicks(false, false);
}

ButtonBar::~ButtonBar()
{
}

void ButtonBar::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("ButtonBar", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void ButtonBar::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void ButtonBar::mouseDown (const MouseEvent& e)
{
    cout << "BB Mouse down" << endl;
}

void ButtonBar::mouseEnter (const MouseEvent& e)
{
    cout << "BB Mouse enter" << endl;
    HelperBar *parent = (HelperBar *)getParentComponent();
    parent->lockMe();
}

void ButtonBar::mouseExit (const MouseEvent& e)
{
    cout << "BB Mouse exit" << endl;    
    HelperBar *parent = (HelperBar *)getParentComponent();
    parent->unlockMe();
}

