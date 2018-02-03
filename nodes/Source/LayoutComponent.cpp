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

#define NODE_SIZE Point<int>(80, 80)

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
    
    // Draw grid
    int gridsize = S::getGridSize();
    g.setColour(Colours::white);
    g.setOpacity(0.12f);
    for (int x=0; x<getWidth(); x+=gridsize){
        g.drawLine(x, 0, x, getHeight());
    }
    for (int y=0; y<getHeight();y+=gridsize){
        g.drawLine(0, y, getWidth(),y);
    }

    // Draw connections
    std::vector<Cable*> connections = S::getInstance().mainComponent->getConnections();
    auto it = connections.begin();
    g.setColour(Colours::white);
    g.setOpacity(1.0f);

    while (it != connections.end()){
        Cable *conn = *it++;
        assert(conn);
        OutletComponent *a = conn->a;
        OutletComponent *b = conn->b;
        
        if (a->isSink()) {
            OutletComponent *tmp = b;
            b = a;
            a = tmp;
        }
            
        Point<int>apos = a->getWindowPos();
        Point<int>bpos = b->getWindowPos();
        
        // New way of drawing cables - snap cables to grid
        int sign = apos.x < bpos.x ? 1 : -1;
        if (apos.y == bpos.y) {
            g.drawLine(apos.x, apos.y, bpos.x, bpos.y);
        } else {
        int halfx = (abs(apos.x - bpos.x) / 2) * sign;
            g.drawLine(apos.x, apos.y, apos.x + halfx, apos.y);
            g.drawLine(apos.x+halfx, apos.y, apos.x + halfx, bpos.y);
            g.drawLine(apos.x+halfx, bpos.y, bpos.x, bpos.y);
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
        
        // Create node based on Component ID as stored in librarian lib file (that's what's passed over during drag and drop from
        // librarian onto the layout component
        NodeComponent *node = createNode(dragSourceDetails.description.toString());
        
        Point<int> size = NODE_SIZE;
        setNodePosition(node, dragSourceDetails.localPosition.x - size.x/2, dragSourceDetails.localPosition.y - size.y/2);
    }
    somethingIsBeingDraggedOver = false;
    repaint();
}

NodeComponent *LayoutComponent::createNode(ComponentDesc *cdesc, Uuid _uuid)
{
    NodeComponent *node = new NodeComponent(cdesc, _uuid);
    addAndMakeVisible(node);
    S::getInstance().mainComponent->addNode(node);
    return node;
}

NodeComponent *LayoutComponent::createNode(String cdescstr, Uuid _uuid)
{
    ComponentDesc *desc = S::getInstance().mainComponent->getLibrarian()->getComponentById(cdescstr);
    if (desc == nullptr){
        S::getConsole()->print("Error: unknown component dragged over: " + cdescstr);
        return nullptr;
    }
    
    NodeComponent *node = createNode(desc, _uuid);
    return node;
}

void LayoutComponent::setNodePosition(NodeComponent *node, int x, int y)
{
    Point<int> size = NODE_SIZE;
    node->setBounds(x, y,
                    size.x, size.y);
    repaint();
}

void LayoutComponent::mouseDown(const MouseEvent& e)
{
    if (e.mods.isRightButtonDown()){
        if (popupMenu.getNumItems() == 0) {
            popupMenu.addSeparator();
            popupMenu.addItem (1, "Load layout");
            popupMenu.addItem (2, "Save layout");
            popupMenu.addItem (3, "Reset layout");
            popupMenu.addSeparator();
        }
        const int result = popupMenu.show();
        if (result == 0)
        {
            // user dismissed the menu without picking anything
        }
        else if (result == 1)
        {
            // user picked item 1
            S::getConsole()->print("Popupmenu: Load layout form file");
            FileChooser myChooser ("Please select layout file to load",
                                   File::getSpecialLocation (File::userDesktopDirectory),
                                   "*.xml");
            if (myChooser.browseForFileToOpen())
            {
                File xmlFile (myChooser.getResult());
                S::getMainComponent()->loadLayoutFromFile(xmlFile.getFullPathName());
            }
        }
        else if (result == 2)
        {
            // user picked item 2
            S::getConsole()->print("Popupmenu: Save layout");
            FileChooser myChooser ("Please select where to save the layout",
                                   File::getSpecialLocation (File::userDesktopDirectory),
                                   "*.xml");
            if (myChooser.browseForFileToSave(true))
            {
                File xmlFile (myChooser.getResult());
                S::getMainComponent()->saveLayoutToFile(xmlFile.getFullPathName());
            }
        }
        else if (result == 3) {
            S::getConsole()->print("Popupmenu: Reset layout");
            S::getMainComponent()->resetLayout();
        }
            
    } else {
        popupMenu.clear();
        popupMenu.dismissAllActiveMenus();
        S::getMainComponent()->deselectAll();
    }
}
