/*
  ==============================================================================

    HelperBar.h
    Created: 23 Oct 2017 11:47:28am
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ButtonBar.h"

//==============================================================================
/*
*/
class HelperBar    :    public Component,
                        public FileDragAndDropTarget

{
public:
    HelperBar();
    ~HelperBar();

    void lockMe();
    void unlockMe();
    void hideMe();
    void showMe();
    
    void paint (Graphics&) override;
    void resized() override;
    void mouseDown(const MouseEvent & e) override;
    void mouseExit(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void filesDropped (const StringArray& filenames, int /* x */, int /* y */) override;
    bool isInterestedInFileDrag (const StringArray&) override;
    
private:
    int lockCnt;
    ButtonBar buttons;
    ComponentAnimator anim;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelperBar)
};
