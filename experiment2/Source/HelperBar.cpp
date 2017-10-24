/*
  ==============================================================================

    HelperBar.cpp
    Created: 23 Oct 2017 11:47:28am
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "HelperBar.h"

using namespace std;


//==============================================================================
HelperBar::HelperBar()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setInterceptsMouseClicks(true, false);
    lockCnt = 0;
    addAndMakeVisible(buttons);
    hideMe();
}

HelperBar::~HelperBar()
{
}

void HelperBar::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//    g.fillAll (Colours::blue);   // clear the background
//
//    g.setColour (Colours::blue);
//    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
//
//    g.setColour (Colours::white);
//    g.setFont (10);
//    g.drawText ("HelperBar", getLocalBounds(),
//                Justification::centred, true);   // draw some placeholder text
}

void HelperBar::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    auto r = getLocalBounds();
    r.reduce(getWidth()*0.1, getHeight()*0.2);
    buttons.setBounds(r);
}

void HelperBar::mouseDown (const MouseEvent& e)
{
    cout << "Mouse down" << endl;
}

void HelperBar::mouseEnter (const MouseEvent& e)
{
    cout << "Mouse enter" << endl;
    showMe();
}

void HelperBar::mouseExit (const MouseEvent& e)
{
    cout << "Mouse exit" << endl;
    hideMe();
}

void HelperBar::lockMe()
{
    lockCnt++;
    cout << "lockCnt: " << lockCnt << endl;
}

void HelperBar::unlockMe()
{
    lockCnt--;
    cout << "lockCnt: " << lockCnt << endl;
}

void HelperBar::hideMe()
{
    anim.fadeOut(&buttons, 1500);
}

void HelperBar::showMe()
{
    anim.fadeIn(&buttons, 100);
}

bool HelperBar::isInterestedInFileDrag (const StringArray&)
{
    return true;
}

void HelperBar::filesDropped (const StringArray& filenames, int /* x */, int /* y */)
{
    Array<File> files;
    
    for (int i = 0; i < filenames.size(); ++i)
        files.add (File (filenames[i]));    
}
