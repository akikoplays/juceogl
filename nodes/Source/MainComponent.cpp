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

void MainContentComponent::selectOutlet(OutletComponent *outlet)
{
    // Determine whether this is beginning or end of selection
    if (selectedOutletA == nullptr) {
        // Ok, wait for endpoint 2 now ..
        selectedOutletA = outlet;
        selectedOutletA->lock();
    }
    else if (selectedOutletB == nullptr) {
        selectedOutletB = outlet;
        selectedOutletB->lock();
        
        // Validate connection
        Connection *conn = new Connection(selectedOutletA, selectedOutletB);
        connections.push_back(conn);
        
        // If connection successful, release selectedOutlets
        selectedOutletA = nullptr;
        selectedOutletB = nullptr;
    }
    
    // todo:
    // Connection validation logic missing!
    
    // Force repaint
    repaint();
}

void MainContentComponent::mouseMove (const MouseEvent& e)
{
    cout << "mouse moves " << e.getPosition().x << ", " << e.getPosition().y << endl;
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
