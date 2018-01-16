/*
  ==============================================================================

    LibrarianComponent.h
    Created: 16 Jan 2018 11:45:20pm
    Author:  Akiko

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class LibrarianComponent    : public Component
{
public:
    LibrarianComponent();
    ~LibrarianComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibrarianComponent)
};
