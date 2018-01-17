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

struct _OutletDesc
{
    String name;
    String direction;
    String type;
    int maxReceivers;
};

struct ComponentDesc
{
    String cid;
    String name;
    Array<_OutletDesc> outlets;
};

class LibrarianComponent    : public Component
{
public:
    LibrarianComponent();
    ~LibrarianComponent();

    void paint (Graphics&) override;
    void resized() override;
    String getAppFolder();
    void addButton();

private:
    Array<ComponentDesc> components;
    OwnedArray<TextButton *> buttons;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibrarianComponent)
};
