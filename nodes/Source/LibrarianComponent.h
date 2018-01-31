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

class OutletDesc
{
public:
    OutletDesc()
    {
        
    }
    ~OutletDesc()
    {
        std::cout << "Deconstructor for OutletDesc: " << name << std::endl;
    }
    
    String name;
    OutletParamBlock::Direction direction;
    OutletParamBlock::Type type;
    int maxReceivers;
    int rating[2]; // Rating is min, max. In case of fixed rating, min = max.
};

struct ComponentDesc
{
    String cid;
    String name;
    OwnedArray<OutletDesc> outlets;
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

    static String getAppFolder();
    void addButton(ComponentDesc *cdesc);
    ComponentDesc *getComponentById(const String cid);
    OutletDesc *getOutletDescByOutlet(OutletComponent *outlet);
    
private:
    Array<ComponentDesc *> components;
    OwnedArray<ComponentButton> buttons;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibrarianComponent)
};
