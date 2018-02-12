/*
  ==============================================================================

    LayoutComponent.h
    Created: 21 Jan 2018 6:10:20pm
    Author:  Akiko

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "config.h"

class ComponentDesc;
class NodeComponent;

//==============================================================================
/*
*/
class LayoutComponent    :  public Component,
                            public DragAndDropTarget,
                            public DragAndDropContainer,
                            private KeyListener

{
public:
    LayoutComponent();
    ~LayoutComponent();

    void paint (Graphics&) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseDoubleClick(const MouseEvent &event) override;
#ifdef USE_MOUSEWHEEL_TO_ZOOM
    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails&) override;
#endif
    bool keyPressed (const KeyPress& key, Component* originatingComponent);

    bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragEnter (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragMove (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragExit (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDropped (const SourceDetails& dragSourceDetails) override;

    // Creates node based on provided component descriptor. Makes it visible and adds it to the scene.
    NodeComponent *createNode(ComponentDesc *cdesc, Uuid _uuid = Uuid::null());
    // Creates node based on provided librarian ID (name of the component as stored in the librarian database). Makes it visible and adds it to the scene.
    NodeComponent *createNode(String cdescstr, Uuid _uuid = Uuid::null());
    // Positions node on the layout.
    void setNodePosition(NodeComponent *node, int x, int y);
    
private:
    PopupMenu popupMenu;
    // Layout zoom factor. Controlled by -/+ and or mousewheel if (USE_MOUSEWHEEL_TO_ZOOM) is defined.
    float scale;
    bool somethingIsBeingDraggedOver;
    bool areaSelecting;
    Point<int> areaSelectionCorner1;
    Point<int> areaSelectionCorner2;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutComponent)
};
