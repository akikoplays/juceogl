/*
  ==============================================================================

    LibrarianComponent.cpp
    Created: 16 Jan 2018 11:45:20pm
    Author:  Akiko

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LibrarianComponent.h"

using namespace std;

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

    // Load library from json file
    String jsonStr;
    String path = getAppFolder() + "mainlib.json";
    File file(path);
    if (!file.exists()) {
        cout << "-- can't open main library" << endl;
    } else {
        jsonStr = file.loadFileAsString();

        // Test json
        //String jsonstr = "{ \"foo\" : \"bar\", \"num\" : 123, \"nested\" : { \"inner\": \"value\" } }";
        var json = JSON::fromString(jsonStr);

        if (DynamicObject* obj = json.getDynamicObject()) {
            
            NamedValueSet& props (obj->getProperties());
            
            for (int i = 0; i < props.size(); ++i)
            {
                cout << i << ": Component " << props.getName(i).toString().toStdString() << endl;
                const Identifier id (props.getName (i));
                var child (props[id]);
                jassert (! child.isVoid());

                ComponentDesc cdesc;
                cdesc.cid = props.getName(i).toString();
                
                NamedValueSet& subprops (child.getDynamicObject()->getProperties());
                for (int i = 0; i < subprops.size(); ++i)
                {
                    if (subprops.getName(i).toString() == "name") {
                        cdesc.name = props.getName(i).toString();
                    }
                    
                    cout << "-- " << subprops.getName(i).toString() << ": " << subprops.getValueAt(i).toString() << endl;

                    const Identifier id (subprops.getName (i));
                    var child (subprops[id]);
                    jassert (! child.isVoid());
                    
                    // Extract outlets
                    if (subprops.getName(i).toString() == "outlets") {

                        NamedValueSet& subprops (child.getDynamicObject()->getProperties());
                        for (int i = 0; i < subprops.size(); ++i)
                        {
                            cout << "\t-- outlet " << i << ": " << subprops.getName(i).toString() << endl;
                            const Identifier id (subprops.getName (i));
                            var child (subprops[id]);
                            jassert (! child.isVoid());

                            _OutletDesc odesc;
                            odesc.name = subprops.getName(i).toString();

                            // Parse outlet properties
                            NamedValueSet& subprops(child.getDynamicObject()->getProperties());
                            for (int i = 0; i < subprops.size(); ++i)
                            {
                                cout << "\t\t-- " << subprops.getName(i).toString().toStdString();
                                cout << ": " << subprops.getValueAt(i).toString() << endl;
                                
                                // Should strongly typed conversion be done here? hmm..
                                // or just map all the values, and then convert else where.
                                if (subprops.getName(i).toString() == "type")
                                    odesc.type = subprops.getValueAt(i).toString();
                                else if (subprops.getName(i).toString() == "dir")
                                    odesc.direction = subprops.getValueAt(i).toString();
                                else if (subprops.getName(i).toString() == "max-receivers")
                                    odesc.maxReceivers = subprops.getValueAt(i);
                            }
                            cdesc.outlets.add(odesc);
                        }
                    }
                }
                components.add(cdesc);
            }
        }
    }
    
    // Debug print collected component descriptors
    for (int i=0; i<components.size(); i++) {
        cout << i << ": " << components[i].name << endl;
        for (int j=0; j<components[i].outlets.size(); j++) {
            cout << "\t" << j << ": " << components[i].outlets[j].name << endl;
            cout << "\t" << "-- type: " << components[i].outlets[j].type << endl;
            cout << "\t" << "-- dir: " << components[i].outlets[j].direction << endl;
            cout << "\t" << "-- max-receivers: " << components[i].outlets[j].maxReceivers << endl;
        }
    }
    return;
}

LibrarianComponent::~LibrarianComponent()
{
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

}

void LibrarianComponent::addButton()
{
    
}
