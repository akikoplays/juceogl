/*
  ==============================================================================

    ButtonBar.h
    Created: 23 Oct 2017 12:16:41pm
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class ButtonBar    : public Component
{
public:
    ButtonBar();
    ~ButtonBar();

    void paint (Graphics&) override;
    void resized() override;

    void mouseDown(const MouseEvent & e) override;
    void mouseExit(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonBar)
};
