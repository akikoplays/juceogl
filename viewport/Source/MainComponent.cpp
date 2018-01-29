/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

using namespace std;

//==============================================================================

void NodeComponent::mouseDrag(const juce::MouseEvent &e)
{
    dragger.dragComponent (this, e, &constrainer);
    LayoutComponent *layout = dynamic_cast<LayoutComponent *>(getParentComponent());
    assert(layout);
    MouseEvent re (e.getEventRelativeTo(layout->getMain()));
    layout->getMain()->getViewport().autoScroll(re.x, re.y, 10,8);
}

MainContentComponent::MainContentComponent()
{
    setSize (600, 400);
    view.setViewedComponent(&layout, false);
    addAndMakeVisible(view);
    layout.setMain(this);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    auto r = getBounds();
    r.reduce(10, 10);
    view.setBounds(r);
    r = getBounds();
    layout.setBounds(0,0,1000,1000);
}
