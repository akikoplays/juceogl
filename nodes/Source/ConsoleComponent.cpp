/*
  ==============================================================================

    ConsoleComponent.cpp
    Created: 2 Feb 2018 1:52:21pm
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ConsoleComponent.h"

using namespace std;

//==============================================================================
ConsoleComponent::ConsoleComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(&editor);
    editor.setMultiLine(true);
    editor.setColour(TextEditor::backgroundColourId, Colour(0xff111111));
    editor.setColour(TextEditor::textColourId, Colour(0xff777777));
    editor.insertTextAtCaret("Hello world :)\n");
    editor.setScrollToShowCursor(true);
}

ConsoleComponent::~ConsoleComponent()
{
}

void ConsoleComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
}

void ConsoleComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    auto r = getLocalBounds();
    editor.setBounds(r);
}

void ConsoleComponent::print(String string)
{
    editor.insertTextAtCaret(string + "\n");
//    editor.setText(editor.getText() + string + "\n");
}
