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
#include "ConsoleComponent.h"

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
//        cout << "Disconnect all pressed" << endl;
        S::getMainComponent()->killAllCablesForOutlet(parent);
    } else if (button == &disconnectSingleButton) {
        cout << "Disconnect single pressed" << endl;
    }
    S::getMainComponent()->hideOptionsCallout();
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
    addAndMakeVisible(deleteSelectedButton);
    deleteButton.setButtonText("Delete Node");
    deleteButton.addListener(this);
    deleteSelectedButton.setButtonText("Delete Selected Nodes");
    deleteSelectedButton.addListener(this);
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
        S::getMainComponent()->removeAndDeleteNode(parent);
        
    } else if (button == &deleteSelectedButton) {
        cout << "Delete Selected Nodes pressed" << endl;
        S::getMainComponent()->removeAndDeleteSelectedNodes();
    }
    S::getMainComponent()->hideOptionsCallout();
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
    left = area.removeFromLeft(100);
    deleteSelectedButton.setBounds(left);
}

//==============================================================================
#pragma mark MainContentComponent

MainContentComponent::MainContentComponent()
: optionsCallout(nullptr), selectedOutletA(nullptr), selectedOutletB(nullptr), somethingIsBeingDraggedOver(false), forceRepaint(false)
{
    // Sanity check
    S::getInstance().mainComponent = this;
    assert(S::getMainComponent() == this);

    // Must instantiate this first
    console = new ConsoleComponent();
    addAndMakeVisible(console);

    // Present layout component
    layout = new LayoutComponent();
//    addAndMakeVisible(layout);
    viewport.setViewedComponent(layout, false);
    addAndMakeVisible(viewport);

    
    // Present librarian toolbox
    librarian = new LibrarianComponent();
//    addAndMakeVisible(librarian);
    viewportLibrarian.setViewedComponent(librarian);
    addAndMakeVisible(viewportLibrarian);
    
    // Main update timer used for animating elements on screen
    startTimerHz(50);
    
    auto screenrect = Desktop::getInstance().getDisplays().getMainDisplay().totalArea;
    setSize(screenrect.getWidth()*0.75, screenrect.getHeight()*0.75);
}

MainContentComponent::~MainContentComponent()
{
    resetLayout();
}


MainContentComponent::ValidationResult MainContentComponent::validateConnection(OutletComponent *a, OutletComponent *b)
{
    // Rules:
    // a) type must match
    // b) direction must be compatible (SOURCE/SINK)
    cout << "Validate connection.. " << endl;
    if (a == b) {
        cout << "-- warning: a == b" << endl;
        return {1, "Can't connect an outlet to itself."};
    }
    // Connection between same two outlets already exists?
    if (getConnectionByOutlets(a, b)) {
        cout << "-- warning: a/b cable already exists" << endl;
        return {2, "The two outlets are already connected."};
    }
    if (a->getDirection() == b->getDirection()) {
        cout << "-- warning: a and b are of same direction" << endl;
        return {3, "Connected outlets can't have same direction."};
    }
    if (a->getType() != b->getType()) {
        cout << "-- warning: a and b are not of same type" << endl;
        return {4, "Connected outlets must be of the same type."};
    }
    // Does SOURCE outlet already has a connection?
    if (a->isSource() && getConnectionsLinkedToOutlet(a).size() > 0) {
        cout << "-- warning: a is SOURCE and already has a cable attached " << endl;
        return {5, "Source outlet can have only ONE incoming connection."};
    }
    if (b->isSource() && getConnectionsLinkedToOutlet(b).size() > 0) {
        cout << "-- warning: b is SOURCE and already has a cable attached " << endl;
        return {5, "Source outlet can have only ONE incoming connection."};
    }
    // Is power rating acceptable?
    if (!a->isRatingCompatible(b)) {
        cout << "-- warning: incompatible rating" << endl;
        return {6, "Power ratings are incompatible. Check the supply voltage."};
    }
    return {0, ""};
}

bool MainContentComponent::createConnection(OutletComponent *a, OutletComponent *b, Uuid uuid)
{
    cout << "Creating new connection" << endl;
    Cable *conn = new Cable(a, b, uuid);
    cables.push_back(conn);
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
    selectedNodes.push_back(node);
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
        MainContentComponent::ValidationResult result;
        result = validateConnection(selectedOutletA, selectedOutletB);
        if (result.code == 0) {
            createConnection(selectedOutletA, selectedOutletB);
        } else {
            AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                              "Connection Warning",
                                              result.message,
                                              "OK");
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

void MainContentComponent::deselectAll()
{
    cout << "Deselect all" << endl;
    // Deselect outlets
    if (selectedOutletA){
        selectedOutletA->signalize(Colours::white, false);
        selectedOutletA = nullptr;
    }
    if (selectedOutletB){
        selectedOutletB->signalize(Colours::white, false);
        selectedOutletB = nullptr;
    }
    
    // Deselect nodes
    // TODO
    for (auto it=selectedNodes.begin(); it!=selectedNodes.end(); it++) {
        NodeComponent *c = *it;
        c->deselect();
    }
    selectedNodes.clear();
}

Rectangle<int> MainContentComponent::getAreaOfSelectedNodes()
{
    if (selectedNodes.size() == 0)
        return Rectangle<int>(0, 0, 0, 0);
    
    Rectangle<int> a = selectedNodes[0]->getBounds();
    
    for (auto comp: selectedNodes)
        if (comp)
            a = a.getUnion(comp->getBounds());
    
    return a;
}

void MainContentComponent::moveSelectedNodes(NodeComponent* chief, Point<int> delta)
{
    // TODO
    // Multi select drag is disabled becuase it's too damn buggy.
    
    return;
    for (auto c: selectedNodes) {
        if (c == chief)
            continue;
        
        // Constrain to limits
        auto targetArea = getAreaOfSelectedNodes() + delta;
        auto limit = chief->getParentComponent()->getBounds();
        

        if (targetArea.getX() < 0)
            delta.x -= targetArea.getX();
        
        if (targetArea.getY() < 0)
            delta.y -= targetArea.getY();
        
        if (targetArea.getBottom() > limit.getBottom())
            delta.y -= targetArea.getBottom() - limit.getBottom();
        
        if (targetArea.getRight() > limit.getRight())
            delta.x -= targetArea.getRight() - limit.getRight();
        
        Rectangle<int> bounds (c->getBounds());
        bounds += delta;
        c->setBounds(bounds);
    }
}

std::vector<Cable *> MainContentComponent::getConnectionsLinkedToOutlet(OutletComponent *outlet)
{
    std::vector<Cable *> list;
    for (auto it = cables.begin(); it != cables.end(); it++) {
        Cable *c = *it;
        if (!c->linksToOutlet(outlet))
            continue;
        list.push_back(c);
    }
    return list;
}

bool MainContentComponent::removeAndDeleteConnection(Cable *cable)
{
    cout << "Remove and delete a connection" << endl;
    for (auto it = cables.begin(); it != cables.end(); it++) {
        Cable *c = *it;
        if (c != cable)
            continue;
        delete(c);
        cables.erase(it);
        cout << "-- erased connection" << endl;
        return true;
    }
    return false;
}

void MainContentComponent::killAllCablesForOutlet(OutletComponent *outlet)
{
    console->print("Kill all cables for outlet " + outlet->getName());
    auto list = getConnectionsLinkedToOutlet(outlet);
    static int i = 0;
    for (auto it = list.begin(); it != list.end(); it++, i++){
        Cable *c = *it;
        removeAndDeleteConnection(c);
        console->print("-- killed cable " + String(i));
    }
    
    // For redraw, theater has changed.
    repaint();
}

void MainContentComponent::killAllCablesForNode(NodeComponent *node)
{
    console->print("Kill all cables for node " + node->getName());
    const OwnedArray<OutletComponent> &outlets = node->getOutlets();
    for (int i=0; i<outlets.size(); i++) {
        killAllCablesForOutlet(outlets[i]);
    }
}

void MainContentComponent::removeAndDeleteNode(NodeComponent *node)
{
    console->print("Deleting node: " + node->getName());
    for (int i=0; i<nodes.size(); i++) {
        if(nodes[i] != node)
            continue;
        // Detach all cables from this node.
        killAllCablesForNode(node);
        // note that remove() will call object's destructor.
        nodes.remove(i);
    }
}

void MainContentComponent::removeAndDeleteSelectedNodes()
{
    for (auto node: selectedNodes) {
        removeAndDeleteNode(node);
    }
    // If you call deselectAll at this point, it may very likely crash
    // because it will try to call methods on destructed objects.
    // So just empty the selection queue.
    selectedNodes.clear();
}

Cable *MainContentComponent::getConnectionByOutlets(OutletComponent *a, OutletComponent *b)
{
    for (auto it = cables.begin(); it != cables.end(); it++) {
        Cable *c = *it;
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
    
    auto r = getLocalBounds();
    
    // Update Layout and Librarian positions
    auto lr = r.removeFromRight(128);
//    librarian->setBounds(lr);
    viewportLibrarian.setBounds(lr);
    // TODO
    // This needs to be set dynamically by the Librarian?
    librarian->setBounds(0, 0, lr.getWidth()-10, lr.getHeight());

    auto cr = r.removeFromBottom(100);
    console->setBounds(cr);

//    layout->setBounds(r);
    viewport.setBounds(r);
    // TODO provisionally lets set this to some insane size
    layout->setBounds(0,0,2048,2048);
}

void MainContentComponent::timerCallback()
{
    if (forceRepaint){
        forceRepaint = false;
        repaint();
    }
}

void MainContentComponent::mouseDown (const MouseEvent& e)
{
}

bool MainContentComponent::saveLayoutToFile(String xmlFileName)
{
    // TODO
    // data serialization test
    ValueTree layoutTree = ValueTree("layout");
    
    // Serialize all nodes
    ValueTree nodesTree = ValueTree("nodes");
    for (auto node: nodes){
        ValueTree child = node->serialize();
        nodesTree.addChild(child, -1, nullptr);
    }
    layoutTree.addChild(nodesTree, 0, nullptr);
    
    // Serialize cables
    ValueTree cablesTree = ValueTree("cables");
    for (auto cable: cables)  {
        ValueTree child = cable->serialize();
        cablesTree.addChild(child, -1, nullptr);
    }
    layoutTree.addChild(cablesTree, 0, nullptr);
    
    ScopedPointer<XmlElement> xml = layoutTree.createXml();
    bool res = xml->writeToFile(xmlFileName, String::empty);
    console->print("Layout save to file " + xmlFileName + " result: " + (res ? "OK" : "FAIL"));
    return res;
}

bool MainContentComponent::loadLayoutFromFile(String xmlFileName)
{
    console->print("Loading layout from file " + xmlFileName);
    // Read from xml file
    File file = File(xmlFileName);
    XmlDocument xmlDoc(file);
    ScopedPointer<XmlElement> xml = xmlDoc.getDocumentElement();
    if (xml == nullptr) {
        console->print("\tError: can't load xml document");
        return false;
    }
    
    // Evict existing layout
    resetLayout();

    ValueTree _layout = ValueTree(ValueTree::fromXml(*xml));
    String tmp = _layout.toXmlString();
    console->print("XML: " + tmp.toStdString());
    
    ValueTree _nodes = _layout.getChildWithName("nodes");
    for (int i=0; i<_nodes.getNumChildren(); i++) {
        ValueTree _node = _nodes.getChild(i);
        String uuid = _node.getProperty("uuid").toString();
        String name = _node.getProperty("name").toString();
        String libid = _node.getProperty("librarian-id").toString();
        var x = _node.getPropertyAsValue("x", nullptr);
        var y = _node.getPropertyAsValue("y", nullptr);
        console->print("\tLoaded node: "
                       + String(i) + " " + uuid + " "
                       + name + " " + libid + " at pos: "
                       + String((int)x) + ", " + String((int)y));

        // Create node in the scene
        ComponentDesc *desc = getLibrarian()->getComponentById(libid);
        if (desc == nullptr){
            // TODO
            // add user friendly and understandable alert here
            console->print("Error: unknown component encountered");
            return false;
        }
        
        NodeComponent *node = layout->createNode(desc, uuid);
        assert(node);
        layout->setNodePosition(node, x, y);
    }

    // Load cables and attach them
    ValueTree _cables = _layout.getChildWithName("cables");

    for (int i=0; i<_cables.getNumChildren(); i++) {
        ValueTree  _cable = _cables.getChild(i);
        String nodeA = _cable.getProperty("nodeA").toString();
        String nodeB = _cable.getProperty("nodeB").toString();
        String outletA = _cable.getProperty("outletA").toString();
        String outletB = _cable.getProperty("outletB").toString();
        String uuid = _cable.getProperty("uuid").toString();

        // Identify outlets and recreate connections
        NodeComponent *node = findNodeByUuid(nodeA);
        assert(node);
        OutletComponent *outlet1 = node->getOutletByName(outletA);
        assert(outlet1);
        node = findNodeByUuid(nodeB);
        assert(node);
        OutletComponent *outlet2 = node->getOutletByName(outletB);
        assert(outlet2);
        
        createConnection(outlet1, outlet2);
    }
    
    forceRepaint = true;
    return true;
}

NodeComponent *MainContentComponent::findNodeByUuid(Uuid uuid)
{
    for (auto node: nodes) {
        if (node->getUuid() == uuid)
            return node;
    }
    return nullptr;
}

void MainContentComponent::resetLayout()
{
    console->print("Reset layout");
    // Delete all cables
    for (auto cable: cables) {
        delete cable;
    }
    cables.clear();
    nodes.clearQuick(true);
    forceRepaint = true;dw
}
