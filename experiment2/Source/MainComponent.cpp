/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainContentComponent::MainContentComponent()
{
    setSize (640, 480);

//    addAndMakeVisible(helper);
    addAndMakeVisible(renderer);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    auto r = getLocalBounds();
//    auto h = r.removeFromBottom(64);
//    helper.setBounds(h);
    renderer.setBounds(r);
}

