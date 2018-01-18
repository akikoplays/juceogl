/*
  ==============================================================================

    NodeComponent.h
    Created: 10 Jan 2018 12:57:52pm
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ComponentDesc;

//==============================================================================
/*
*/
class NodeComponent    : public Component
{
public:
    // Create node according to the descriptor received from LibrarianComponent via drag and drop.
    NodeComponent(ComponentDesc *_desc);
    ~NodeComponent();

    void paint (Graphics&) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;

private:
    OwnedArray<Component> outlets;
    ComponentDragger dragger;
    ComponentBoundsConstrainer constrainer;
    ComponentDesc *desc;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeComponent)
};
