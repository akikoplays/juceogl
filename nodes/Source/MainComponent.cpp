/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "NodeComponent.h"
#include "OutletComponent.h"
#include "OntopComponent.h"

using namespace std;

//==============================================================================
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
MainContentComponent::MainContentComponent()
: selectedOutletA(nullptr), selectedOutletB(nullptr)
{
    setSize (600, 400);

    NodeComponent *node = new NodeComponent();
    addAndMakeVisible(node);
    nodes.add(node);
    node->setBounds(100, 100, 100, 100);

    node = new NodeComponent();
    addAndMakeVisible(node);
    nodes.add(node);
    node->setBounds(300, 300, 100, 100);
    
//    addAndMakeVisible(&ontop);
//    ontop.setBounds(getLocalBounds());
//    ontop.setInterceptsMouseClicks(false, false);
    
    S::getInstance().mainComponent = this;
    assert(S::getInstance().mainComponent == this);
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
        cout << "-- warning: a and b are of same type" << endl;
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
    CallOutBox::launchAsynchronously(options, rect, nullptr);
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
    }
    else if (selectedOutletB == nullptr) {
        selectedOutletB = outlet;
        
        // Validate connection
        if (validateConnection(selectedOutletA, selectedOutletB)) {
            createConnection(selectedOutletA, selectedOutletB);
        }
        
        // If connection successful, release selectedOutlets
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
    cout << "Kill all cables in outlet" << endl;
    auto list = getConnectionsLinkedToOutlet(outlet);
    for (auto it = list.begin(); it != list.end(); it++){
        Connection *c = *it;
        removeAndDeleteConnection(c);
    }
    
    // For redraw, theater has changed.
    repaint();
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
