/*
  ==============================================================================

    MainWindow.cpp
    Created: 24 Oct 2017 11:49:00am
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"
#include "MainComponent.h"
 
//==============================================================================
//MainWindow::MainWindow()
MainWindow::MainWindow (String name)  : DocumentWindow (name,
                                            Desktop::getInstance().getDefaultLookAndFeel()
                                                                  .findColour (ResizableWindow::backgroundColourId),
                                            DocumentWindow::allButtons)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setUsingNativeTitleBar (true);
    setResizable(true, false);
    setResizeLimits (640, 400, 2048, 2048);
//    centreWithSize (getWidth(), getHeight());


    contentComponent = new MainContentComponent();
    setContentOwned (contentComponent, true);
    setVisible (true);

}

MainWindow::~MainWindow()
{
}

MainWindow* MainWindow::getMainAppWindow()
{
    for (int i = TopLevelWindow::getNumTopLevelWindows(); --i >= 0;)
        if (auto* maw = dynamic_cast<MainWindow*> (TopLevelWindow::getTopLevelWindow (i)))
            return maw;

    return nullptr;
}


void MainWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

void MainWindow::paint (Graphics& g)
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
    g.drawText ("MainWindow", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void MainWindow::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    if (contentComponent != nullptr)
        contentComponent->setBounds(getLocalBounds());
}

static const char* openGLRendererName = "OpenGL Renderer";

StringArray MainWindow::getRenderingEngines() const
{
    StringArray renderingEngines;
    
    if (auto* peer = getPeer())
        renderingEngines = peer->getAvailableRenderingEngines();
    
#if JUCE_OPENGL
    renderingEngines.add (openGLRendererName);
#endif
    
    return renderingEngines;
}

void MainWindow::setRenderingEngine (int index)
{
    
#if JUCE_OPENGL
    if (getRenderingEngines()[index] == openGLRendererName
        //&& contentComponent != nullptr
        //&& ! contentComponent->isShowingOpenGLDemo()
        )
    {
        openGLContext.attachTo (*getTopLevelComponent());
        return;
    }
    
    openGLContext.detach();
#endif
    
    if (auto* peer = getPeer())
        peer->setCurrentRenderingEngine (index);
}

void MainWindow::setOpenGLRenderingEngine()
{
    setRenderingEngine (getRenderingEngines().indexOf (openGLRendererName));
}

int MainWindow::getActiveRenderingEngine() const
{
#if JUCE_OPENGL
    if (openGLContext.isAttached())
        return getRenderingEngines().indexOf (openGLRendererName);
#endif
    
    if (auto* peer = getPeer())
        return peer->getCurrentRenderingEngine();
    
    return 0;
}
