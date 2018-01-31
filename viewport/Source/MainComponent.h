/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;
class MainContentComponent;

class SnapConstraint : public ComponentBoundsConstrainer
{
public:
    void applyBoundsToComponent (Component &, Rectangle<int> bounds) override;
};

class NodeComponent : public Component
{
public:
    NodeComponent()
    {
        
    }
    
    ~NodeComponent()
    {
        
    }

    void paint (Graphics& g)
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::slategrey);
        
        g.setFont (Font (16.0f));
        g.setColour (Colours::white);
        g.drawText ("Node", getLocalBounds(), Justification::centred, true);
    }
    
    void resized()
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
    }
    
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent &e) override;

private:
    ComponentDragger dragger;

    // TODO my snap to grid movement constrainer
//    ComponentBoundsConstrainer constrainer;
    SnapConstraint constrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeComponent)
};

class LayoutComponent : public Component
{
public:
    LayoutComponent()
    {
        addAndMakeVisible(&node);
        cout << "Layout added" << endl;
    }
    
    ~LayoutComponent()
    {
        cout << "Layout destroyed" << endl;
    }
    
    void setMain(MainContentComponent *_main)
    {
        main = _main;
    }
    
    MainContentComponent *getMain()
    {
        return main;
    }
    
    void paint (Graphics& g)
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::blueviolet);
        
        g.setFont (Font (16.0f));
        g.setColour (Colours::white);
        g.drawText ("Layout", getLocalBounds(), Justification::centred, true);
    }
    
    void resized()
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
        Rectangle<int> r(0,0,100,100);
        node.setBounds(r);
    }
    
private:
    MainContentComponent *main;
    NodeComponent node;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutComponent)
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;

    Viewport &getViewport()
    {
        return view;
    }

private:
    Viewport view;
    LayoutComponent layout;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
