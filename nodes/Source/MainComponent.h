/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "LibrarianComponent.h"
#include "NodeComponent.h"
#include "OntopComponent.h"

class OutletComponent;

class Connection {
public:
    Connection(OutletComponent *_a, OutletComponent *_b)
    {
        a = _a;
        b = _b;
    };
    
    ~Connection() {};
    bool linksToOutlet(OutletComponent *outlet)
    {
        return (outlet == a || outlet == b);
    }
        
public:
    OutletComponent *a;
    OutletComponent *b;
};

//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class OutletOptionsComponent : public Component,
                                public ButtonListener
{
public:
    OutletOptionsComponent(OutletComponent *_parent);
    ~OutletOptionsComponent();

    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked(Button *button) override;

private:
    OutletComponent *parent;
    TextButton disconnectAllButton;
    TextButton disconnectSingleButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutletOptionsComponent)
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   :  public Component,
                                public DragAndDropTarget,
                                public DragAndDropContainer
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;
    void mouseMove (const MouseEvent& e) override;
    bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragEnter (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragMove (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDragExit (const SourceDetails& /*dragSourceDetails*/) override;
    void itemDropped (const SourceDetails& dragSourceDetails) override;

    // Selects the given outlet as A or B endpoint of future connection.
    void selectOutlet(OutletComponent *outlet, bool options = false);
    // Validates connection following predefined set of rules, and returns true if it's ok.
    bool validateConnection(OutletComponent *a, OutletComponent *b);
    // Creates and registers new connection object.
    bool createConnection(OutletComponent *a, OutletComponent *b);
    // Removes specific connection from vector, calls delete().
    bool removeAndDeleteConnection(Connection *cable);
    // Popup callout with outlet options.
    void showOutletOptions(OutletComponent *outlet);
    // Kill all outlet cables.
    void killAllCablesForOutlet(OutletComponent *outlet);
    // Returns a vector with all cables that are linked to this outlet.
    std::vector<Connection *> getConnectionsLinkedToOutlet(OutletComponent *outlet);
    // Returns a cable connecting these two outlets.
    Connection *getConnectionByOutlets(OutletComponent *a, OutletComponent *b);

private:
//    OntopComponent ontop;
    OutletComponent *selectedOutletA;
    OutletComponent *selectedOutletB;
    LibrarianComponent librarian;
    OwnedArray<Component> nodes;

    std::vector<Connection*> connections;
    bool somethingIsBeingDraggedOver;

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
