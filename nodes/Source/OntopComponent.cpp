/*
  ==============================================================================

    OntopComponent.cpp
    Created: 10 Jan 2018 5:55:14pm
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OntopComponent.h"

using namespace std;

//==============================================================================
OntopComponent::OntopComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    setAlpha(0.1f);
}

OntopComponent::~OntopComponent()
{
}

void OntopComponent::mouseMove (const MouseEvent& e)
{
    cout << "#mouse moves " << e.getPosition().x << ", " << e.getPosition().y << endl;
}

void OntopComponent::paint (Graphics& g)
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
    g.drawText ("OntopComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void OntopComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
