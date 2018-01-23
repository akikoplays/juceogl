/*
  ==============================================================================

    LayoutComponent.cpp
    Created: 21 Jan 2018 6:10:20pm
    Author:  Akiko

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LayoutComponent.h"
#include "MainComponent.h"
#include "OutletComponent.h"
#include "NodeComponent.h"

using namespace std;

#define NODE_SIZE Point<int>(100, 100)

//==============================================================================
LayoutComponent::LayoutComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

LayoutComponent::~LayoutComponent()
{
}

void LayoutComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colours::black);   // clear the background
    
    // Draw connections
    std::vector<Connection*> connections = S::getInstance().mainComponent->getConnections();
    auto it = connections.begin();
    int i = 0;
    while (it != connections.end()){
        cout << "Painting connection " << i++ << endl;
        Connection *conn = *it++;
        assert(conn);
        g.setColour(Colours::white);
        g.setOpacity(1.0f);
        float width = 1.0f;
        OutletComponent *a = conn->a;
        OutletComponent *b = conn->b;
        
        Point<int>apos = a->getWindowPos();
        Point<int>bpos = b->getWindowPos();
        
        // Draw strain reliefs
        g.drawLine(apos.x, apos.y, a->getStrainReliefPos().x, a->getStrainReliefPos().y);
        g.drawLine(bpos.x, bpos.y, b->getStrainReliefPos().x,  b->getStrainReliefPos().y);
        
        // Manhattan
        apos = a->getStrainReliefPos();
        bpos = b->getStrainReliefPos();
        if ((apos.x == bpos.x) || (apos.y == bpos.y)) // Little speed up
            g.drawLine(apos.x, apos.y, bpos.x, bpos.y, width);
        else {
            g.drawLine(apos.x, apos.y, bpos.x, apos.y, width);
            g.drawLine(bpos.x, apos.y, bpos.x, bpos.y, width);
        }
    }

}

void LayoutComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
}

//==============================================================================
// These methods implement the DragAndDropTarget interface, and allow our component
// to accept drag-and-drop of objects from other Juce components..

bool LayoutComponent::isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/)
{
    // normally you'd check the sourceDescription value to see if it's the
    // sort of object that you're interested in before returning true, but for
    // the demo, we'll say yes to anything..
    return true;
}

void LayoutComponent::itemDragEnter (const SourceDetails& /*dragSourceDetails*/)
{
    somethingIsBeingDraggedOver = true;
    repaint();
}

void LayoutComponent::itemDragMove (const SourceDetails& /*dragSourceDetails*/)
{
}

void LayoutComponent::itemDragExit (const SourceDetails& /*dragSourceDetails*/)
{
    somethingIsBeingDraggedOver = false;
    repaint();
}

void LayoutComponent::itemDropped (const SourceDetails& dragSourceDetails)
{
    if (somethingIsBeingDraggedOver) {
        cout << "Items dropped: " << dragSourceDetails.description.toString() << endl;
        cout << "drag state: " << somethingIsBeingDraggedOver << endl;
        cout << "pos: " << dragSourceDetails.localPosition.x << ", " << dragSourceDetails.localPosition.y << endl;
        
        // Create node based on what's dragged into the screen
        ComponentDesc *desc = S::getInstance().mainComponent->getLibrarian()->getComponentById(dragSourceDetails.description.toString());
        if (desc == nullptr){
            cout << "Error: unknown component dragged over" << endl;
            return;
        }
        
        NodeComponent *node = new NodeComponent(desc);
        addAndMakeVisible(node);
        S::getInstance().mainComponent->addNode(node);
        
        Point<int> size = NODE_SIZE;
        node->setBounds(dragSourceDetails.localPosition.x - size.x/2, dragSourceDetails.localPosition.y - size.y/2,
                        size.x, size.y);
        
    }
    somethingIsBeingDraggedOver = false;
    repaint();
}

void LayoutComponent::mouseDown(const MouseEvent& e)
{
    S::getMainComponent()->deselectAll();
}
