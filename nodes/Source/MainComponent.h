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
#include "LayoutComponent.h"

class OutletComponent;

using namespace std;

class Connection : public Component {
public:
    Connection(OutletComponent *_a, OutletComponent *_b, Uuid _uuid=Uuid::null())
    {
        // When cable is loaded from layout file, Uuid is provided, otherwise it is calculated.
        if (_uuid.isNull())
            uuid = Uuid();
        else
            uuid = _uuid;
        
        a = _a;
        b = _b;
        
        cout << "connection created " + uuid.toString() << endl;
    };
    
    ~Connection()
    {
        cout << "connection destroyed " + uuid.toString() << endl;
    };
    
    bool linksToOutlet(OutletComponent *outlet)
    {
        return (outlet == a || outlet == b);
    }

    ValueTree serialize()
    {
        ValueTree child = ValueTree("cable");
        child.setProperty("uuid", uuid.toString(), nullptr);
        child.setProperty("nodeA", a->getNode()->getUuid().toString(), nullptr);
        child.setProperty("nodeB", b->getNode()->getUuid().toString(), nullptr);
        OutletDesc *odesc = a->getOutletDesc();
        child.setProperty("outletA", odesc->name, nullptr);
        odesc = b->getOutletDesc();
        child.setProperty("outletB", odesc->name, nullptr);

        return child;
    }
    
public:
    OutletComponent *a;
    OutletComponent *b;

private:
    Uuid uuid;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Connection)
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
class NodeOptionsComponent : public Component,
public ButtonListener
{
public:
    NodeOptionsComponent(NodeComponent *_parent);
    ~NodeOptionsComponent();
    
    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked(Button *button) override;
    
private:
    NodeComponent *parent;
    TextButton deleteButton;
    TextButton deleteSelectedButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeOptionsComponent)
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   :  public Component,
//                                public DragAndDropTarget,
                                public DragAndDropContainer,
                                private Timer
{
public:
    
    struct ValidationResult{
        int code;
        String message;
    };
    
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown (const MouseEvent& e) override;

    // Selects the given outlet as A or B endpoint of future connection, opens options callout if options = true.
    void selectOutlet(OutletComponent *outlet, bool options = false);
    // Selects node, opens options callout if options = true.
    void selectNode(NodeComponent *node, bool options = false);
    // Deselect nodes and outlets.
    void deselectAll();
    // Get area of all selected nodes.
    Rectangle<int> getAreaOfSelectedNodes();
    // Move all selected nodes by delta value.
    void moveSelectedNodes(NodeComponent *chief, Point<int> delta);
    // Validates connection following predefined set of rules, and returns true if it's ok.
    ValidationResult validateConnection(OutletComponent *a, OutletComponent *b);
    // Creates and registers new connection object.
    bool createConnection(OutletComponent *a, OutletComponent *b, Uuid uuid=nullptr);
    // Return list of cables.
    std::vector<Connection*> getConnections() {return connections;};
    // Removes specific connection from vector, calls delete().
    bool removeAndDeleteConnection(Connection *cable);
    // Remove and delete specific Node.
    void removeAndDeleteNode(NodeComponent *node);
    // Remove and delete selected nodes.
    void removeAndDeleteSelectedNodes();
    // Popup callout with outlet options.
    void showOutletOptions(OutletComponent *outlet);
    // Popup callout with node options.
    void showNodeOptions(NodeComponent *node);
    // Kill all outlet cables.
    void killAllCablesForOutlet(OutletComponent *outlet);
    // Kill all node cables.
    void killAllCablesForNode(NodeComponent *node);
    // Returns a vector with all cables that are linked to this outlet.
    std::vector<Connection *> getConnectionsLinkedToOutlet(OutletComponent *outlet);
    // Returns a cable connecting these two outlets.
    Connection *getConnectionByOutlets(OutletComponent *a, OutletComponent *b);
    // Hide optionsCalloutBox, if there. If not - ignore.
    void hideOptionsCallout();
    // Add node to internal list.
    void addNode(NodeComponent *node)
    {
        nodes.add(node);
    };
    LibrarianComponent *getLibrarian()
    {
        return librarian;
    };
    Viewport &getViewport()
    {
        return viewport;
    }
    
private:
    // First viewport that contains LayoutComponent
    Viewport viewport;
    // Second viewport that contains LibrarianComponent
    Viewport viewportLibrarian;
    CallOutBox *optionsCallout;
    OutletComponent *selectedOutletA;
    OutletComponent *selectedOutletB;
    ScopedPointer<LibrarianComponent> librarian;
    ScopedPointer<LayoutComponent> layout;
    OwnedArray<NodeComponent> nodes;
    std::vector<NodeComponent*> selectedNodes;
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
    
    // Shorter way to get mainComponent from Singleton.
    static MainContentComponent *getMainComponent()
    {
        return S::getInstance().mainComponent;
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
