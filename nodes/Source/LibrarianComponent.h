/*
  ==============================================================================

    LibrarianComponent.h
    Created: 16 Jan 2018 11:45:20pm
    Author:  Akiko

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OutletComponent.h"

//==============================================================================
/*
*/

struct OutletDesc
{
    String name;
    OutletParamBlock::Direction direction;
    OutletParamBlock::Type type;
    int maxReceivers;
};

struct ComponentDesc
{
    String cid;
    String name;
    Array<OutletDesc> outlets;
};

class LibrarianComponent    :   public Component,
                                public DragAndDropContainer
{
public:

    class ComponentButton : public TextButton
    {
    public:
        ComponentButton()
        {
            
        };
        ~ComponentButton()
        {
            
        };
        
        void mouseDrag(const MouseEvent &event) override {
            findParentDragContainerFor(getParentComponent())->startDragging(var(getName()), this); //for now...
        }
    private:
        
    };

    LibrarianComponent();
    ~LibrarianComponent();

    void paint (Graphics&) override;
    void resized() override;

    String getAppFolder();
    void addButton(ComponentDesc *cdesc);
    ComponentDesc *getComponentById(const String cid);
    
private:
    Array<ComponentDesc *> components;
    OwnedArray<ComponentButton> buttons;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibrarianComponent)
};
