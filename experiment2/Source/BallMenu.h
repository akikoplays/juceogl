/*
  ==============================================================================

    BallMenu.h
    Created: 25 Oct 2017 4:08:37pm
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class BallMenu    : public Component,
                    private Button::Listener
{
public:
    BallMenu();
    ~BallMenu();

    void paint(Graphics&) override;
    void resized() override;
    void buttonClicked(Button* button) override;
    void mouseDown (const MouseEvent& e) override;

    // User defined methods
    void launch();
    void hide();
    Button* createButton(const String name);

private:
    int numIcons;
    OwnedArray<Component> animations;
    ComponentAnimator animator;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BallMenu)
};
