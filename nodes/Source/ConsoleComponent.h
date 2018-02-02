/*
  ==============================================================================

    ConsoleComponent.h
    Created: 2 Feb 2018 1:52:21pm
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class ConsoleComponent    : public Component
{
public:
    ConsoleComponent();
    ~ConsoleComponent();

    void paint (Graphics&) override;
    void resized() override;
    void print(String string);

private:
    TextEditor editor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConsoleComponent)
};
