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
    
    // TODO
    // test absolute coords in Layout component when operating within viewport
    
    // Draw connections
    std::vector<Connection*> connections = S::getInstance().mainComponent->getConnections();
    auto it = connections.begin();
    int i = 0;
    while (it != connections.end()){
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
        // TODO optimize this!!
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
        
//        // Create node based on what's dragged into the screen
//        ComponentDesc *desc = S::getInstance().mainComponent->getLibrarian()->getComponentById(dragSourceDetails.description.toString());
//        if (desc == nullptr){
//            cout << "Error: unknown component dragged over" << endl;
//            return;
//        }
//
//        NodeComponent *node = createNode(desc);
        
        // Create node based on Component ID as stored in librarian lib file (that's what's passed over during drag and drop from
        // librarian onto the layout component
        NodeComponent *node = createNode(dragSourceDetails.description.toString());
        
        Point<int> size = NODE_SIZE;
//        node->setBounds(dragSourceDetails.localPosition.x - size.x/2, dragSourceDetails.localPosition.y - size.y/2,
//                        size.x, size.y);
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
        cout << "Error: unknown component dragged over" << endl;
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
}

void LayoutComponent::mouseDown(const MouseEvent& e)
{
    if (e.mods.isRightButtonDown()){
        if (popupMenu.getNumItems() == 0) {
            popupMenu.addItem (1, "Load layout");
            popupMenu.addItem (2, "Save layout");
        }
        const int result = popupMenu.show();
        if (result == 0)
        {
            // user dismissed the menu without picking anything
        }
        else if (result == 1)
        {
            // user picked item 1
            cout << "popupmenu: Load layout" << endl;
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
            cout << "popupmenu: Save layout" << endl;
            FileChooser myChooser ("Please select where to save the layout",
                                   File::getSpecialLocation (File::userDesktopDirectory),
                                   "*.xml");
            if (myChooser.browseForFileToSave(true))
            {
                File xmlFile (myChooser.getResult());
                S::getMainComponent()->saveLayoutToFile(xmlFile.getFullPathName());
            }
        }
    } else {
        popupMenu.clear();
        popupMenu.dismissAllActiveMenus();
        S::getMainComponent()->deselectAll();
    }
}
