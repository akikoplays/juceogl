/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "NodeComponent.h"
#include "OutletComponent.h"
#include "OntopComponent.h"

class Connection {
public:
    Connection(OutletComponent *_a, OutletComponent *_b)
    {
        a = _a;
        b = _b;
    };
    
    ~Connection() {};

public:
    OutletComponent *a;
    OutletComponent *b;
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;
    void selectOutlet(OutletComponent *outlet);
    void mouseMove (const MouseEvent& e) override;

private:
    OntopComponent ontop;
    OutletComponent *selectedOutletA;
    OutletComponent *selectedOutletB;
    OwnedArray<Component> nodes;
    
    std::vector<Connection*> connections;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

class S
{
public:
    static S& getInstance()
    {
        static S    instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    
    
private:
    S() {mainComponent = nullptr;}                    // Constructor? (the {} brackets) are needed here.
    
public:
    S(S const&)               = delete;
    void operator=(S const&)  = delete;
    
public:
    // Main component store
    MainContentComponent *mainComponent;
};
