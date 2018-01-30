/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

using namespace std;

//==============================================================================


void SnapConstraint::applyBoundsToComponent (Component &component, Rectangle<int> bounds)
{
    cout << "Apply bounds" << endl;
    if (auto* positioner = component.getPositioner())
        positioner->applyNewBounds (bounds);
    else {
// original code:
//        component.setBounds (bounds);
        int grid = 20;
        int x = bounds.getX() / grid * grid;
        int y = bounds.getY() / grid * grid;
        bounds.setX(x);
        bounds.setY(y);
        component.setBounds(bounds);
    }
}

void NodeComponent::mouseDown (const MouseEvent& e)
{
    dragger.startDraggingComponent (this, e);\
}

void NodeComponent::mouseUp (const MouseEvent& e)
{
}

void NodeComponent::mouseDrag(const juce::MouseEvent &e)
{
    dragger.dragComponent (this, e, &constrainer);
    //    int x = e.getDistanceFromDragStartX(), y = e.getDistanceFromDragStartY();
    //    if(e.getDistanceFromDragStartX()%10==0)
    //        x = e.getDistanceFromDragStartX();
    //    if(e.getDistanceFromDragStartY()%10==0)
    //        y = e.getDistanceFromDragStartY();
    //    setTopLeftPosition(x+pos.getX(), y+pos.getY());
    
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
