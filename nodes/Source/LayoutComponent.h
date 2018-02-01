/*
  ==============================================================================

    LayoutComponent.h
    Created: 21 Jan 2018 6:10:20pm
    Author:  Akiko

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ComponentDesc;
class NodeComponent;

//==============================================================================
/*
*/
class LayoutComponent    :  public Component,
                            public DragAndDropTarget,
                            public DragAndDropContainer

{
public:
    LayoutComponent();
    ~LayoutComponent();

    void paint (Graphics&) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;

    bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragEnter (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragMove (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragExit (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDropped (const SourceDetails& dragSourceDetails) override;

    // Creates node based on provided component descriptor. Makes it visible and adds it to the scene.
    NodeComponent *createNode(ComponentDesc *cdesc);
    NodeComponent *createNode(String cdescstr);
    
private:
    PopupMenu popupMenu;
    bool somethingIsBeingDraggedOver;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutComponent)
};
