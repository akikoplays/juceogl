/*
  ==============================================================================

    OntopComponent.h
    Created: 10 Jan 2018 5:55:14pm
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class OntopComponent    : public Component
{
public:
    OntopComponent();
    ~OntopComponent();

    void paint (Graphics&) override;
    void resized() override;
    void mouseMove (const MouseEvent& e) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OntopComponent)
};
