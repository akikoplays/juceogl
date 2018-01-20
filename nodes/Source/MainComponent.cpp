/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "NodeComponent.h"
#include "OutletComponent.h"
#include "OntopComponent.h"
#include "LibrarianComponent.h"

using namespace std;

//==============================================================================
#pragma mark OutletOptionsComponent

OutletOptionsComponent::OutletOptionsComponent(OutletComponent *_parent)
{
    setName("Options");
    addAndMakeVisible(disconnectAllButton);
    addAndMakeVisible(disconnectSingleButton);
    disconnectAllButton.setButtonText("Disconnect All");
    disconnectSingleButton.setButtonText("Disconnect");
    disconnectSingleButton.addListener(this);
    disconnectAllButton.addListener(this);
    setAlpha(0.5f);
    parent = _parent;
}


OutletOptionsComponent::~OutletOptionsComponent()
{
    
}

void OutletOptionsComponent::buttonClicked(Button *button)
{
    if (button == &disconnectAllButton) {
        cout << "Disconnect all pressed" << endl;
        S::getInstance().mainComponent->killAllCablesForOutlet(parent);
    } else if (button == &disconnectSingleButton) {
        cout << "Disconnect single pressed" << endl;
    }
    S::getInstance().mainComponent->hideOptionsCallout();
}

void OutletOptionsComponent::paint(Graphics &g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void OutletOptionsComponent::resized()
{
    Rectangle<int> area(getLocalBounds());
    Rectangle<int> left (area.removeFromLeft(100));
    disconnectAllButton.setBounds(left);
    disconnectSingleButton.setBounds(area.removeFromLeft(100));

}

//==============================================================================
#pragma mark NodeOptionsComponent
NodeOptionsComponent::NodeOptionsComponent(NodeComponent *_parent)
{
    setName("NodeOptions");
    addAndMakeVisible(deleteButton);
    deleteButton.setButtonText("Delete Node");
    deleteButton.addListener(this);
    setAlpha(0.5f);
    parent = _parent;
}


NodeOptionsComponent::~NodeOptionsComponent()
{
    
}

void NodeOptionsComponent::buttonClicked(Button *button)
{
    if (button == &deleteButton) {
        cout << "Delete Node pressed" << endl;
        // TODO:
        S::getInstance().mainComponent->removeAndDeleteNode(parent);
    }
    S::getInstance().mainComponent->hideOptionsCallout();
}

void NodeOptionsComponent::paint(Graphics &g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void NodeOptionsComponent::resized()
{
    Rectangle<int> area(getLocalBounds());
    Rectangle<int> left (area.removeFromLeft(100));
    deleteButton.setBounds(left);
    
}

//==============================================================================
#pragma mark MainContentComponent

MainContentComponent::MainContentComponent()
: selectedOutletA(nullptr), selectedOutletB(nullptr), optionsCallout(nullptr), somethingIsBeingDraggedOver(false)
{
    setSize(1200, 800);
    
//    addAndMakeVisible(&ontop);
//    ontop.setBounds(getLocalBounds());
//    ontop.setInterceptsMouseClicks(false, false);

    addAndMakeVisible(&librarian);
    
    // Sanity check
    S::getInstance().mainComponent = this;
    assert(S::getInstance().mainComponent == this);
    
    // Main update timer used for animating elements on screen
    startTimerHz(50);
}

MainContentComponent::~MainContentComponent()
{
}

bool MainContentComponent::validateConnection(OutletComponent *a, OutletComponent *b)
{
    // Rules:
    // a) type must match
    // b) direction must be compatible (SOURCE/SINK)
    cout << "Validate connection.. " << endl;
    if (a == b) {
        cout << "-- warning: a == b" << endl;
        return false;
    }
    // Connection between same two outlets already exists?
    if (getConnectionByOutlets(a, b)) {
        cout << "-- warning: a/b cable already exists" << endl;
        return false;
    }
    if (a->getDirection() == b->getDirection()) {
        cout << "-- warning: a and b are of same direction" << endl;
        return false;
    }
    if (a->getType() != b->getType()) {
        cout << "-- warning: a and b are not of same type" << endl;
        return false;
    }
    // Does SOURCE outlet already has a connection?
    if (a->isSource() && getConnectionsLinkedToOutlet(a).size() > 0) {
        cout << "-- warning: a is SOURCE and already has a cable attached " << endl;
        return false;
    }
    if (b->isSource() && getConnectionsLinkedToOutlet(b).size() > 0) {
        cout << "-- warning: b is SOURCE and already has a cable attached " << endl;
        return false;
    }
    return true;
}

bool MainContentComponent::createConnection(OutletComponent *a, OutletComponent *b)
{
    cout << "Creating new connection" << endl;
    Connection *conn = new Connection(a, b);
    connections.push_back(conn);
    a->addCable(conn);
    b->addCable(conn);
    cout << "-- registered" << endl;
    return true;
}


void MainContentComponent::showOutletOptions(OutletComponent *outlet)
{
    OutletOptionsComponent *options = new OutletOptionsComponent(outlet);
    options->setSize(300, 100);
    auto rect = outlet->getScreenBounds();
    rect.setY(rect.getY() + 10);
    optionsCallout = &CallOutBox::launchAsynchronously(options, rect, nullptr);
}

void MainContentComponent::showNodeOptions(NodeComponent *node)
{
    NodeOptionsComponent *options = new NodeOptionsComponent(node);
    options->setSize(300, 100);
    auto rect = node->getScreenBounds();
    rect.setY(rect.getY() + 10);
    optionsCallout = &CallOutBox::launchAsynchronously(options, rect, nullptr);
}

void MainContentComponent::selectNode(NodeComponent *node, bool options)
{
    // If node was double clicked, we want to show options callout.
    if (options) {
        cout << "Dblclick, going into options mode" << endl;
        showNodeOptions(node);
        return;
    }
    // TODO:
    // implement node selection logic
}

void MainContentComponent::selectOutlet(OutletComponent *outlet, bool options)
{
    // If outlet was double clicked, we want to show options callout.
    if (options) {
        cout << "Dblclick, going into options mode" << endl;
        showOutletOptions(outlet);
        
        return;
    }
    
    // Otherwise, go into outlet selection mode, for establishing new connections
    // Determine whether this is beginning or end of selection
    if (selectedOutletA == nullptr) {
        // Ok, wait for endpoint 2 now
        selectedOutletA = outlet;
        outlet->signalize(Colours::white, true);
    }
    else if (selectedOutletB == nullptr) {
        selectedOutletB = outlet;
        
        // Validate connection
        if (validateConnection(selectedOutletA, selectedOutletB)) {
            createConnection(selectedOutletA, selectedOutletB);
        }
        
        // If connection successful, release selectedOutlets, and stop signaling on them
        selectedOutletA->signalize(Colours::white, false);
        selectedOutletB->signalize(Colours::white, false);
        selectedOutletA = nullptr;
        selectedOutletB = nullptr;
    }
        
    // Force repaint
    repaint();
}

std::vector<Connection *> MainContentComponent::getConnectionsLinkedToOutlet(OutletComponent *outlet)
{
    std::vector<Connection *> list;
    for (auto it = connections.begin(); it != connections.end(); it++) {
        Connection *c = *it;
        if (!c->linksToOutlet(outlet))
            continue;
        list.push_back(c);
    }
    return list;
}

bool MainContentComponent::removeAndDeleteConnection(Connection *cable)
{
    cout << "Remove and delete a connection" << endl;
    for (auto it = connections.begin(); it != connections.end(); it++) {
        Connection *c = *it;
        if (c != cable)
            continue;
        delete(c);
        connections.erase(it);
        cout << "-- erased connection" << endl;
        return true;
    }
    return false;
}

void MainContentComponent::killAllCablesForOutlet(OutletComponent *outlet)
{
    cout << "Kill all cables for outlet " << outlet->getName() <<  endl;
    auto list = getConnectionsLinkedToOutlet(outlet);
    static int i = 0;
    for (auto it = list.begin(); it != list.end(); it++, i++){
        Connection *c = *it;
        removeAndDeleteConnection(c);
        cout << "-- killed cable " << i << endl;
    }
    
    // For redraw, theater has changed.
    repaint();
}

void MainContentComponent::killAllCablesForNode(NodeComponent *node)
{
    cout << "Kill all cables for node " << node->getName() <<  endl;
    const OwnedArray<OutletComponent> &outlets = node->getOutlets();
    for (int i=0; i<outlets.size(); i++) {
        killAllCablesForOutlet(outlets[i]);
    }
}

void MainContentComponent::removeAndDeleteNode(NodeComponent *node)
{
    cout << "Deleting node: " << node->getName() << endl;
    
    for (int i=0; i<nodes.size(); i++) {
        if(nodes[i] != node)
            continue;
        cout << "-- found"<<endl;
        
        // Detach all cables from this node.
        killAllCablesForNode(node);
        // note that remove() will call object's destructor.
        nodes.remove(i);

    }
}

Connection *MainContentComponent::getConnectionByOutlets(OutletComponent *a, OutletComponent *b)
{
    for (auto it = connections.begin(); it != connections.end(); it++) {
        Connection *c = *it;
        if (c->linksToOutlet(a) && c->linksToOutlet(b))
            return c;
    }
    return nullptr;
}

void MainContentComponent::hideOptionsCallout()
{
    if (optionsCallout == nullptr)
        return;
    optionsCallout->dismiss();
    optionsCallout = nullptr;
}

void MainContentComponent::mouseMove (const MouseEvent& e)
{
//    cout << "mouse moves " << e.getPosition().x << ", " << e.getPosition().y << endl;
}


void MainContentComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
    
    // Update Librarian position
    auto r = getLocalBounds();
    auto lr = r.removeFromRight(200);
    librarian.setBounds(lr);
    
    // Draw connections
    auto it = connections.begin();
    int i = 0;
    while (it != connections.end()){
        cout << "Painting connection " << i++ << endl;
        Connection *conn = *it++;
        assert(conn);
        g.setColour(Colours::white);
        OutletComponent *a = conn->a;
        OutletComponent *b = conn->b;
        
        Point<int>apos = a->getWindowPos();
        Point<int>bpos = b->getWindowPos();
        
        // Manhattan
        float width = 1.5f;
        if ((apos.x == bpos.x) || (apos.y == bpos.y))
            g.drawLine(apos.x, apos.y, bpos.x, bpos.y, width);
        else {
            g.drawLine(apos.x, apos.y, bpos.x, apos.y, width);
            g.drawLine(bpos.x, apos.y, bpos.x, bpos.y, width);
        }
    }

}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

//==============================================================================
// These methods implement the DragAndDropTarget interface, and allow our component
// to accept drag-and-drop of objects from other Juce components..

bool MainContentComponent::isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/)
{
    // normally you'd check the sourceDescription value to see if it's the
    // sort of object that you're interested in before returning true, but for
    // the demo, we'll say yes to anything..
    return true;
}

void MainContentComponent::itemDragEnter (const SourceDetails& /*dragSourceDetails*/)
{
    somethingIsBeingDraggedOver = true;
    repaint();
}

void MainContentComponent::itemDragMove (const SourceDetails& /*dragSourceDetails*/)
{
}

void MainContentComponent::itemDragExit (const SourceDetails& /*dragSourceDetails*/)
{
    somethingIsBeingDraggedOver = false;
    repaint();
}

void MainContentComponent::itemDropped (const SourceDetails& dragSourceDetails)
{
    if (somethingIsBeingDraggedOver) {
        cout << "Items dropped: " << dragSourceDetails.description.toString() << endl;
        cout << "drag state: " << somethingIsBeingDraggedOver << endl;
        cout << "pos: " << dragSourceDetails.localPosition.x << ", " << dragSourceDetails.localPosition.y << endl;

        // TODO:
        // Create node based on what's dragged into the screen
        ComponentDesc *desc = librarian.getComponentById(dragSourceDetails.description.toString());
        if (desc == nullptr){
            cout << "Error: unknown component dragged over" << endl;
            return;
        }
        
        NodeComponent *node = new NodeComponent(desc);
        addAndMakeVisible(node);
        nodes.add(node);
        node->setBounds(dragSourceDetails.localPosition.x, dragSourceDetails.localPosition.y, 100, 100);

    }
    somethingIsBeingDraggedOver = false;
    repaint();
}

void MainContentComponent::timerCallback()
{
    
}
