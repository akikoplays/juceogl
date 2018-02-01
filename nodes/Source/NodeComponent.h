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
/* This is my snap to grid constraint.
 */
class SnapConstraint : public ComponentBoundsConstrainer
{
public:
    void applyBoundsToComponent (Component &, Rectangle<int> bounds) override;
};


//==============================================================================
/*
*/
class NodeComponent    : public Component
{
public:
    // Create node according to the descriptor received from LibrarianComponent via drag and drop.
    NodeComponent(ComponentDesc *_desc, Uuid _uuid=Uuid::null());
    ~NodeComponent();

    void paint (Graphics&) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent &event) override;

    const OwnedArray<OutletComponent>& getOutlets();
    // Returns outlet found by its unique name, as described in the library.
    OutletComponent *getOutletByName(String name);
    OutletDesc *getOutletDescByOutlet(OutletComponent *outlet);
    void select();
    void deselect();
    Uuid getUuid();
    ValueTree serialize();

private:
    OwnedArray<OutletComponent> outlets;
    ComponentDragger dragger;
//    ComponentBoundsConstrainer constrainer;
    SnapConstraint constrainer;
    Uuid uuid;
    ComponentDesc *desc;
    Point<int> mouseDownWithinTarget;
    bool selected;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeComponent)
};
