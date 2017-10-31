/*
  ==============================================================================

    BallMenu.cpp
    Created: 25 Oct 2017 4:08:37pm
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "BallMenu.h"
#include "Renderer.h"

using namespace std;

//==============================================================================
BallMenu::BallMenu()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    numIcons = 6;
}

void BallMenu::launch()
{
    for (int i = numIcons; --i >= 0;)
    {
        Button* b = createButton();
        animations.add (b);
        addAndMakeVisible (b);
        b->addListener (this);
    }
    
    
    Rectangle<int> bounds = getLocalBounds();
    for (int i = 0; i < animations.size(); ++i)
        animations.getUnchecked(i)->setBounds(bounds.reduced (150, 150));
    
    for (int i = 0; i < animations.size(); ++i)
    {
        const int w = bounds.getWidth();
        const int h = bounds.getHeight();
        const int newIndex = (i + 3) % animations.size();
        const float angle = newIndex * 1.7f * float_Pi / animations.size();
        const float radius = w * 0.15f;
        
        Rectangle<int> r (w  / 2 + (int) (radius * std::sin (angle) - 50),
                          h / 2 + (int) (radius * std::cos (angle) - 50),
                          100, 100);
        
        animator.animateComponent (animations.getUnchecked(i),
                                   r.reduced (10),
                                   1.0f,
                                   300 + i * 50,
                                   false,
                                   0.0,
                                   0.0);
    }
}

void BallMenu::hide()
{
    
}

BallMenu::~BallMenu()
{
    cout << "BallMenu:: deleting" << endl;
}

void BallMenu::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
//
//    g.setColour (Colours::grey);
//    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
//
//    g.setColour (Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("BallMenu", getLocalBounds(),
//                Justification::centred, true);   // draw some placeholder text
}

void BallMenu::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
}

void BallMenu::buttonClicked (Button*)
{
    // todo
    Renderer *parent = dynamic_cast<Renderer*>(getParentComponent());
    jassert(parent);
    if (parent)
        parent->hideBallMenu();
}

void BallMenu::mouseDown (const MouseEvent& e)
{
    cout << "BallMenu:: mouse clicked" << endl;
    Renderer *parent = dynamic_cast<Renderer*>(getParentComponent());
    jassert(parent);
    if (parent)
        parent->hideBallMenu();
}

Button* BallMenu::createButton()
{
    Image juceIcon = ImageCache::getFromMemory (BinaryData::juce_icon_png,
                                                BinaryData::juce_icon_pngSize)
    .rescaled (128, 128);
    
    ImageButton* b = new ImageButton ("ImageButton");
    
    b->setImages (true, true, true,
                  juceIcon, 1.0f, Colours::transparentBlack,
                  juceIcon, 1.0f, Colours::white,
                  juceIcon, 1.0f, Colours::white,
                  0.5f);
    
    return b;
}
