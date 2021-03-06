/*
  ==============================================================================

    LibrarianComponent.cpp
    Created: 16 Jan 2018 11:45:20pm
    Author:  Akiko

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LibrarianComponent.h"
#include "MainComponent.h"
#include "ConsoleComponent.h"

using namespace std;

#define BUTTON_SIZE Point<int>(64, 64)

//==============================================================================

String LibrarianComponent::getAppFolder()
{
    String path = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName();
    path += File::getSeparatorString();
    return path;
}

LibrarianComponent::LibrarianComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    S::getConsole()->print("Initializing Librarian");

    // Load library from json file
    String jsonStr;
    String path = LibrarianComponent::getAppFolder() + "mainlib.json";
    File file(path);
    if (!file.exists()) {
        S::getConsole()->print("Warning: can't open main library (mainlib.json) file");
    } else {
        jsonStr = file.loadFileAsString();

        // Test json
        //String jsonstr = "{ \"foo\" : \"bar\", \"num\" : 123, \"nested\" : { \"inner\": \"value\" } }";
        var json = JSON::fromString(jsonStr);

        if (DynamicObject* obj = json.getDynamicObject()) {
            
            NamedValueSet& props (obj->getProperties());
            
            for (int i = 0; i < props.size(); ++i)
            {
                S::getConsole()->print(String(i) + ": Component " + props.getName(i).toString().toStdString());
                const Identifier id (props.getName (i));
                var child (props[id]);
                jassert (! child.isVoid());

                ComponentDesc *cdesc = new ComponentDesc();
                cdesc->cid = props.getName(i).toString();
                
                NamedValueSet& subprops (child.getDynamicObject()->getProperties());
                for (int i = 0; i < subprops.size(); ++i)
                {
                    if (subprops.getName(i).toString() == "name") {
                        cdesc->name = subprops.getValueAt(i).toString();
                    }
                    
                    S::getConsole()->print("-- " + subprops.getName(i).toString() + ": " + subprops.getValueAt(i).toString());
                    
                    const Identifier id (subprops.getName (i));
                    var child (subprops[id]);
                    jassert (! child.isVoid());
                    
                    // Extract outlets
                    if (subprops.getName(i).toString() == "outlets") {

                        NamedValueSet& subprops (child.getDynamicObject()->getProperties());
                        for (int i = 0; i < subprops.size(); ++i)
                        {
                            S::getConsole()->print("\t-- outlet " + String(i) + ": " + subprops.getName(i).toString());
                            const Identifier id (subprops.getName (i));
                            var child (subprops[id]);
                            jassert (! child.isVoid());

                            OutletDesc *odesc = new OutletDesc();
                            odesc->name = subprops.getName(i).toString();

                            // Parse outlet properties
                            NamedValueSet& subprops(child.getDynamicObject()->getProperties());
                            for (int i = 0; i < subprops.size(); ++i)
                            {
                                S::getConsole()->print("\t\t-- "
                                                       + subprops.getName(i).toString().toStdString()
                                                       + ": " + subprops.getValueAt(i).toString());
                                
                                // Should strongly typed conversion be done here? hmm..
                                // or just map all the values, and then convert else where.
                                if (subprops.getName(i).toString() == "type") {
                                    if (subprops.getValueAt(i).toString() == "bus")
                                        odesc->type = OutletParamBlock::Type::POWER_BUS;
                                    else if (subprops.getValueAt(i).toString() == "dcbus")
                                        odesc->type = OutletParamBlock::Type::POWER_DCBUS;
                                    else if (subprops.getValueAt(i).toString() == "binary")
                                        odesc->type = OutletParamBlock::Type::BINARY;
                                    else if (subprops.getValueAt(i).toString() == "comm")
                                        odesc->type = OutletParamBlock::Type::COMM;
                                }
                                else if (subprops.getName(i).toString() == "dir"){
                                    if (subprops.getValueAt(i).toString() == "in")
                                        odesc->direction = OutletParamBlock::Direction::SOURCE;
                                    else if (subprops.getValueAt(i).toString() == "out")
                                        odesc->direction = OutletParamBlock::Direction::SINK;
                                }
                                else if (subprops.getName(i).toString() == "max-receivers")
                                    odesc->maxReceivers = subprops.getValueAt(i);
                                else if (subprops.getName(i).toString() == "rating") {
                                    var rating = subprops.getValueAt(i);
                                    if (rating.isArray()) {
                                        if (rating.size() != 2){
                                            S::getConsole()->print("\t\t-- error: if rating is range, then it must have two values, ie [220,500]");
                                            
                                            continue;
                                        }
                                        S::getConsole()->print("\t\t-- rating is range: " + rating[0].toString() + " to " + rating[1].toString());
                                        odesc->rating[0] = rating[0];
                                        odesc->rating[1] = rating[1];
                                    } else {
                                        S::getConsole()->print("\t\t-- rating is fixed value: " + rating.toString());
                                        odesc->rating[0] = rating;
                                        odesc->rating[1] = rating;
                                    }
                                }

                            }
                            cdesc->outlets.add(odesc);
                        }
                    }
                }
                components.add(cdesc);
            }
        }
    }
    
    // Print collected component descriptors and create buttons for each
    for (int i=0; i<components.size(); i++) {
        cout << i << ": " << components[i]->name << endl;
        for (int j=0; j<components[i]->outlets.size(); j++) {
            cout << "\t" << j << ": " << components[i]->outlets[j]->name << endl;
            cout << "\t" << "-- type: " << components[i]->outlets[j]->type << endl;
            cout << "\t" << "-- dir: " << components[i]->outlets[j]->direction << endl;
            cout << "\t" << "-- max-receivers: " << components[i]->outlets[j]->maxReceivers << endl;
        }
        addButton(components[i]);
    }
    return;
}

LibrarianComponent::~LibrarianComponent()
{
    // Deallocate all descriptors
    for (int i=0; i<components.size(); i++) {
        ComponentDesc *cd = components[i];
        delete(cd);
    }
}

void LibrarianComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("LibrarianComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void LibrarianComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    Point<int>size = BUTTON_SIZE;

    setBounds(0, 0, getWidth(), (buttons.size() + 5) * size.y);

    auto r = getLocalBounds();
    for (int i=0; i<buttons.size(); i++) {
        TextButton *b = buttons[i];
        auto p = r.removeFromTop(size.y);
        b->setBounds(p);
        addAndMakeVisible(b);
    }
    
}

void LibrarianComponent::addButton(ComponentDesc *cdesc)
{
    ComponentButton *b = new ComponentButton();
    b->setButtonText(cdesc->name);
    b->setName(cdesc->cid);
    buttons.add(b);
}

ComponentDesc *LibrarianComponent::getComponentById(const String cid)
{
    for (int i=0; i<components.size(); i++) {
        ComponentDesc *cd = components[i];
        if (cd->cid == cid)
            return cd;
    }
    return nullptr;
}

