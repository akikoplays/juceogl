/*
  ==============================================================================

    MainWindow.h
    Created: 24 Oct 2017 11:49:00am
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MainContentComponent;

//==============================================================================
/*
*/
class MainWindow    : public DocumentWindow
{
public:
    MainWindow(String name);
    ~MainWindow();

    void paint (Graphics&) override;
    void resized() override;
    void closeButtonPressed() override;

    static MainWindow* getMainAppWindow(); // returns the MainWindow if it exists.
    StringArray getRenderingEngines() const;
    void setRenderingEngine (int index);
    void setOpenGLRenderingEngine();
    int getActiveRenderingEngine() const;

private:
#if JUCE_OPENGL
    OpenGLContext openGLContext;
#endif
    ScopedPointer<MainContentComponent> contentComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};
