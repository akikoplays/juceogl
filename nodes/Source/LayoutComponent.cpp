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
    scale = 1.0f;
    addKeyListener(this);
    setWantsKeyboardFocus(true);
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

    if (areaSelecting) {
//        cout << "Selecting " << areaSelectionCorner1.x << ", " << areaSelectionCorner1.y << " to " << areaSelectionCorner2.x << ", " << areaSelectionCorner2.y << endl;
        g.setColour(Colours::grey);
        int x1 = min(areaSelectionCorner1.x, areaSelectionCorner2.x);
        int x2 = max(areaSelectionCorner1.x, areaSelectionCorner2.x);
        int y1 = min(areaSelectionCorner1.y, areaSelectionCorner2.y);
        int y2 = max(areaSelectionCorner1.y, areaSelectionCorner2.y);
        g.drawRect(x1, y1, x2-x1, y2-y1);
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
    node->setBounds(x / S::getGridSize() * S::getGridSize(),
                    y / S::getGridSize() * S::getGridSize(),
                    size.x, size.y);
    repaint();
}

void LayoutComponent::mouseDown(const MouseEvent& e)
{
    if (e.mods.isRightButtonDown()){
        if (popupMenu.getNumItems() == 0) {
            popupMenu.addSeparator();
            popupMenu.addItem(1, "Load layout");
            popupMenu.addItem(2, "Save layout");
            popupMenu.addItem(3, "Reset layout");
            popupMenu.addItem(4, "Save layout to image");
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
        } else if (result == 4) {
            // Code taken from JUCE forum:
            // https://forum.juce.com/t/rendering-window-contents-to-image-file/12820/7
            
            Component *comp = this;
            File thumbFile = File::getSpecialLocation (File::userDesktopDirectory).getChildFile ("snapshot.png");
            Image snapShot = comp->createComponentSnapshot (comp->getLocalBounds());
//            Image thumbnail = snapShot.rescaled (2048, 2048 * comp->getHeight() / comp->getWidth());
            
            // copied from jules' posting above:
            if (ImageFileFormat* format = ImageFileFormat::findImageFormatForFileExtension (thumbFile))
            {
                FileOutputStream out (thumbFile);
                if (out.openedOk())
                    format->writeImageToStream (snapShot, out);
            }
        }
            
    } else {
        popupMenu.clear();
        popupMenu.dismissAllActiveMenus();
        S::getMainComponent()->deselectAll();
        
//        cout << "Area Selecting true" << endl;
        areaSelecting = true;
        areaSelectionCorner1 = e.getMouseDownPosition();
    }
}

void LayoutComponent::mouseUp(const MouseEvent& e)
{
//    cout << "Area Selecting false" << endl;
    areaSelecting = false;
    repaint();
}

void LayoutComponent::mouseDrag(const MouseEvent &e)
{
    areaSelectionCorner2 = e.getPosition();

    // Selected area, sorted
    int x1 = min(areaSelectionCorner1.x, areaSelectionCorner2.x);
    int x2 = max(areaSelectionCorner1.x, areaSelectionCorner2.x);
    int y1 = min(areaSelectionCorner1.y, areaSelectionCorner2.y);
    int y2 = max(areaSelectionCorner1.y, areaSelectionCorner2.y);
    int w = x2-x1;
    int h = y2-y1;
    Rectangle<int> selection(x1, y1, w, h);
    
    cout << "---" << endl;
    cout << selection.toString() << endl;
    const OwnedArray<NodeComponent> &nodes = S::getMainComponent()->getNodes();
    for (int i=0; i<nodes.size(); i++) {
        auto rect = nodes[i]->getBoundsInParent();
        
        bool overlap = false;
        if (x1 < rect.getX()+rect.getWidth() && x1+w > rect.getX() &&
            y1 < rect.getY()+rect.getHeight() && y1+h > rect.getY())
            overlap = true;
        cout << "Node: " << nodes[i]->getName() << " overlaps: " << overlap << endl;
        cout << rect.toString() << endl;
        if (overlap)
            S::getMainComponent()->selectNode(nodes[i]);
        else
            S::getMainComponent()->deselectNode(nodes[i]);
    }
    repaint();
}

void LayoutComponent::mouseDoubleClick(const MouseEvent &event)
{
    // TODO:
    // current zoom out zoom in, still WIP
    if (getTransform().isIdentity()) {
        setTransform(AffineTransform::scale(0.5, 0.5));
    } else {
        setTransform(AffineTransform::identity);
    }
    repaint();
}

bool LayoutComponent::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    if (key == KeyPress ('-', ModifierKeys::ctrlModifier, 0))
    {
        scale -= 0.1f;
        scale = scale > 2.0f ? 2.0f : scale < 0.2f ? 0.2f : scale;
        setTransform(AffineTransform::scale(scale, scale));
        return true;
    }
    if (key == KeyPress ('=', ModifierKeys::ctrlModifier, 0))
    {
        scale += 0.1f;
        scale = scale > 2.0f ? 2.0f : scale < 0.2f ? 0.2f : scale;
        setTransform(AffineTransform::scale(scale, scale));
        return true;
    }

    return false;
}

#ifdef USE_MOUSEWHEEL_TO_ZOOM
void LayoutComponent::mouseWheelMove (const MouseEvent &event, const MouseWheelDetails &details)
{
    if (!event.mods.isCtrlDown() && !event.mods.isAltDown()) {
        int curX = S::getMainComponent()->getViewport().getViewPositionX();
        int curY = S::getMainComponent()->getViewport().getViewPositionY();
        S::getMainComponent()->getViewport().setViewPosition(curX - details.deltaX*200.0f, curY - details.deltaY*200.0f);
        return;
    }
    static float scale = 1.0f;
    scale -= details.deltaY*0.5;
    scale = scale > 2.0f ? 2.0f : scale < 0.2f ? 0.2f : scale;
    setTransform(AffineTransform::scale(scale, scale));
}
#endif
