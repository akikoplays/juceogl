//
//  DemoControlsOverlay.hpp
//  OGL - App
//
//  Created by BorisP on 10/18/17.
//

#ifndef DemoControlsOverlay_hpp
#define DemoControlsOverlay_hpp

#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

//==============================================================================
/**
 This component sits on top of the main GL demo, and contains all the sliders
 and widgets that control things.
 */
//class MainContentComponent;
class DemoControlsOverlay  : public Component
{
public:
    DemoControlsOverlay ()
    {
        lookAndFeelChanged();
    }
    
    void initialise()
    {
    }
    
    void resized() override
    {
        //        Rectangle<int> area (getLocalBounds().reduced (4));
        //
        //        Rectangle<int> top (area.removeFromTop (75));
        //
        //        Rectangle<int> sliders (top.removeFromRight (area.getWidth() / 2));
        //        showBackgroundToggle.setBounds (sliders.removeFromBottom (25));
        //        speedSlider.setBounds (sliders.removeFromBottom (25));
        //        sizeSlider.setBounds (sliders.removeFromBottom (25));
        //
        //        top.removeFromRight (70);
        //        statusLabel.setBounds (top);
        //
        //        Rectangle<int> shaderArea (area.removeFromBottom (area.getHeight() / 2));
        //
        //        Rectangle<int> presets (shaderArea.removeFromTop (25));
        //        presets.removeFromLeft (100);
        //        presetBox.setBounds (presets.removeFromLeft (150));
        //        presets.removeFromLeft (100);
        //        textureBox.setBounds (presets);
        //
        //        shaderArea.removeFromTop (4);
        //        tabbedComp.setBounds (shaderArea);
    }
    
    void mouseDown (const MouseEvent& e) override
    {
//        demo.draggableOrientation.mouseDown (e.getPosition());
        cout << "Mouse down event" << endl;
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
//        demo.draggableOrientation.mouseDrag (e.getPosition());
    }
    
    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& d) override
    {
        //        sizeSlider.setValue (sizeSlider.getValue() + d.deltaY);
    }
    
    void mouseMagnify (const MouseEvent&, float magnifyAmmount) override
    {
        //        sizeSlider.setValue (sizeSlider.getValue() + magnifyAmmount - 1.0f);
    }
    
    
private:
    
//    MainContentComponent& demo;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoControlsOverlay)
};

#endif /* DemoControlsOverlay_hpp */
