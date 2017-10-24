/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include <algorithm>
#include "../JuceLibraryCode/JuceHeader.h"
#include "Resources/WavefrontObjParser.h"

using namespace std;

class ToolbarComponent : public Component
{
public:
    ComponentAnimator animator;
    bool hidden;
    Image toolsIcon;
    
    ToolbarComponent()
    {
        setBounds(0, 0, 800, 128);
        toolsIcon = ImageCache::getFromMemory (BinaryData::toolbar_png,
                                               BinaryData::toolbar_pngSize).rescaled(800, 120);

    }
    
    bool isBusy()
    {
        cout << "toolbar is busy: " << animator.isAnimating() << endl;
        return animator.isAnimating();
    }
    
    void hide()
    {
        Rectangle<int>r1 = getLocalBounds();
        Rectangle<int>r2 = getBounds();
        Rectangle<int> r(0, getParentComponent()->getHeight() - 20,
                            getWidth(), getHeight());
        animator.animateComponent (this,
                                   r,
                                   1.0f,
                                   100,
                                   false,
                                   0.0,
                                   0.0);
        hidden = true;
    }
    
    void show()
    {
        Rectangle<int> r(0, getParentComponent()->getHeight() - 120,
                            getWidth(), getHeight());
        animator.animateComponent (this,
                                   r,
                                   1.0f,
                                   100,
                                   false,
                                   0.0,
                                   0.0);
        hidden = false;
    }
    
    void paint (Graphics& g) override
    {
//        Rectangle<float> area (getLocalBounds().toFloat().reduced (2.0f));
//        g.setColour (Colours::orange);
//        g.drawRoundedRectangle (area, 10.0f, 2.0f);
        Rectangle<int> area (getLocalBounds());
        g.drawImageAt(toolsIcon, area.getX(), area.getY());
    }
    
    void resized() override
    {
        // Just set the limits of our constrainer so that we don't drag ourselves off the screen
        
    }
    
    void mouseEnter (const MouseEvent& e) override
    {
        cout << "Mouse entered toolbar zone" << endl;
        show();
    }
    
    void mouseExit (const MouseEvent& e) override
    {
        cout << "Mouse exited toolbar zone" << endl;
        hide();
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        // Prepares our dragger to drag this Component
        cout << "Mouse down!" << endl;
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        // Moves this Component according to the mouse drag event and applies our constraints to it
    }

    Button* createButton()
    {
        Image toolsIcon = ImageCache::getFromMemory (BinaryData::tools_png,
                                                    BinaryData::tools_pngSize)
        .rescaled (800, 128);
        
        ImageButton* b = new ImageButton ("ImageButton");
        
        b->setImages (true, true, true,
                      toolsIcon, 1.0f, Colours::transparentBlack,
                      toolsIcon, 1.0f, Colours::white,
                      toolsIcon, 1.0f, Colours::white,
                      0.5f);
        
        return b;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolbarComponent)

};

//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class MainContentComponent : public OpenGLAppComponent,
                            private ComboBox::Listener,
                            private Button::Listener,
                            private Timer
{
public:
    
    //==============================================================================
    MainContentComponent()
    {
        setSize (800, 600);
        
        textures.add (new BuiltInTexture ("Checker", BinaryData::checker_jpg, BinaryData::checker_jpgSize));

        addAndMakeVisible (statusLabel);
        statusLabel.setText("Render options:", dontSendNotification);
        statusLabel.setJustificationType (Justification::topLeft);
        statusLabel.setFont (Font (14.0f));

        addAndMakeVisible (textureBox);
        textureBox.addListener (this);
        updateTexturesList();
        
        addAndMakeVisible (presetBox);
        presetBox.addListener (this);

        // add shaders to user selection droplist
        Array<ShaderPreset> presets (getPresets());
        StringArray presetNames;
        for (int i = 0; i < presets.size(); ++i){
            presetBox.addItem (presets[i].name, i + 1);
            cout << "Added shader " << presets[i].name << endl;
        }
        
        addAndMakeVisible (presetLabel);
        presetLabel.setText ("Shader Preset:", dontSendNotification);
        presetLabel.attachToComponent (&presetBox, true);

        // texture manager
        addAndMakeVisible (textureLabel);
        textureLabel.setText ("Texture:", dontSendNotification);
        textureLabel.attachToComponent (&textureBox, true);
        
        // toolbar appears on bottom
        addAndMakeVisible(toolbar);
        toolbar.setBounds(getLocalBounds().getWidth()/2 - toolbar.getWidth()/2, getLocalBounds().getHeight() - toolbar.getHeight(),
                          toolbar.getWidth(), toolbar.getHeight());
        lookAndFeelChanged();
        
        rotation = 0.0f;
        zoom = 0.4f;
        startTimer(2000);
    }
    
    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
        Rectangle<int> area (getLocalBounds().reduced (4));
        Rectangle<int> top (area.removeFromBottom(40));
        statusLabel.setBounds (top);

        Rectangle<int> shaderArea (area.removeFromBottom (64));
        Rectangle<int> presets (shaderArea.removeFromTop (25));
        presets.removeFromLeft (100);
        presetBox.setBounds (presets.removeFromLeft (150));
        presets.removeFromLeft (100);
        textureBox.setBounds (presets);
        
        toolbar.setBounds(getLocalBounds().getWidth()/2 - toolbar.getWidth()/2, getLocalBounds().getHeight() + toolbar.getHeight(),
                          toolbar.getWidth(), 128);
        
        // Must set for mouse interactions
        draggableOrientation.setViewport (getLocalBounds());
    }
    

    ~MainContentComponent()
    {
        shutdownOpenGL();
    }
    
    void timerCallback() override
    {
        stopTimer();
        toolbar.hide();
    }

    void newOpenGLContextCreated() override
    {
        // nothing to do in this case - we'll initialise our shaders + textures
        // on demand, during the render callback.
        presetBox.setSelectedItemIndex(0);
        textureBox.setSelectedItemIndex(0);
    }
    
    void openGLContextClosing() override
    {
        // When the context is about to close, you must use this callback to delete
        // any GPU resources while the context is still current.
        freeAllContextObjects();
    }
    
    void freeAllContextObjects()
    {
        shape = nullptr;
        shader = nullptr;
        attributes = nullptr;
        uniforms = nullptr;
        texture.release();
    }

    void comboBoxChanged (ComboBox* box) override
    {
        if (box == &presetBox)
            selectPreset (presetBox.getSelectedItemIndex());
        else if (box == &textureBox)
            selectTexture (textureBox.getSelectedId());
    }

    void selectPreset (int preset)
    {
        const ShaderPreset& p = getPresets()[preset];
        setShaderProgram(p.vertexShader, p.fragmentShader);
    }

    void selectTexture (int itemID)
    {
#if JUCE_MODAL_LOOPS_PERMITTED
        if (itemID == 1000)
        {
            static File lastLocation = File::getSpecialLocation (File::userPicturesDirectory);
            
            FileChooser fc ("Choose an image to open...", lastLocation, "*.jpg;*.jpeg;*.png;*.gif");
            
            if (fc.browseForFileToOpen())
            {
                lastLocation = fc.getResult();
                
                textures.add (new TextureFromFile (fc.getResult()));
                updateTexturesList();
                
                textureBox.setSelectedId(textures.size());
            }
        }
        else
#endif
        {
            if (DemoTexture* t = textures[itemID - 1])
                setTexture(t);
        }
    }

    void buttonClicked (Button*) override
    {
        
    }

    Matrix3D<float> getProjectionMatrix() const
    {
        float w = 1.0f / (zoom + 0.1f);
        float h = w * getLocalBounds().toFloat().getAspectRatio (false);
        return Matrix3D<float>::fromFrustum (-w, w, -h, h, 4.0f, 30.0f);
    }
    
    Matrix3D<float> getViewMatrix() const
    {
        Matrix3D<float> viewMatrix = draggableOrientation.getRotationMatrix() * Vector3D<float> (0.0f, 0.0f, -10.0f);
        Matrix3D<float> rotationMatrix = viewMatrix.rotated (Vector3D<float> (rotation, rotation, -0.3f));
        return rotationMatrix * viewMatrix;
    }
    
    void render() override
    {
        jassert (OpenGLHelpers::isContextActive());
        
        const float desktopScale = (float) openGLContext.getRenderingScale();
        OpenGLHelpers::clear (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        
        if (textureToUse != nullptr)
            if (! textureToUse->applyTo (texture))
                textureToUse = nullptr;

        updateShader();   // Check whether we need to compile a new shader
        
        if (shader == nullptr)
            return;

        // Having used the juce 2D renderer, it will have messed-up a whole load of GL state, so
        // we need to initialise some important settings before doing our normal GL 3D drawing..
        glEnable (GL_DEPTH_TEST);
        glDepthFunc (GL_LESS);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        openGLContext.extensions.glActiveTexture (GL_TEXTURE0);
        glEnable (GL_TEXTURE_2D);
        
        glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));
        
        texture.bind();
        
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        shader->use();
        
        if (uniforms->projectionMatrix != nullptr)
            uniforms->projectionMatrix->setMatrix4 (getProjectionMatrix().mat, 1, false);
        
        if (uniforms->viewMatrix != nullptr)
            uniforms->viewMatrix->setMatrix4 (getViewMatrix().mat, 1, false);
        
        if (uniforms->texture != nullptr)
            uniforms->texture->set ((GLint) 0);
        
        if (uniforms->lightPosition != nullptr)
            uniforms->lightPosition->set (-15.0f, 10.0f, 15.0f, 0.0f);
        
        shape->draw (openGLContext, *attributes);

        // Reset the element buffers so child Components draw correctly
        openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);
        openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        
        drawBackground2DStuff(desktopScale);
    }
    
    void paint (Graphics& g) override
    {
        // You can add your component specific drawing code here!
        // This will draw over the top of the openGL background.
        
        g.setColour (getLookAndFeel().findColour (Label::textColourId));
        g.setFont (20);
        g.drawText ("Unwrella Juced Demo", 25, 20, 300, 30, Justification::left);
        g.drawLine (20, 20, 170, 20);
        g.drawLine (20, 50, 170, 50);
    }
    
    void initialise() override
    {
        
    }
    
    void shutdown() override
    {
        
    }
    
    void mouseMove (const MouseEvent& e) override
    {
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        draggableOrientation.mouseDown (e.getPosition());
        cout << "Mouse down event" << endl;
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        draggableOrientation.mouseDrag (e.getPosition());
    }
    
    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& d) override
    {
        zoom += d.deltaY;
        zoom = max(0.0f, min(1000.0f, zoom));
    }
    
    void mouseMagnify (const MouseEvent&, float magnifyAmmount) override
    {
        zoom += magnifyAmmount - 1.0f;
    }
    
    void updateTexturesList()
    {
        textureBox.clear();
        
        for (int i = 0; i < textures.size(); ++i)
            textureBox.addItem (textures.getUnchecked(i)->name, i + 1);
        
#if JUCE_MODAL_LOOPS_PERMITTED
        textureBox.addSeparator();
        textureBox.addItem ("Load from a file...", 1000);
#endif
    }

    void drawBackground2DStuff (float desktopScale)
    {
        // Create an OpenGLGraphicsContext that will draw into this GL window..
        ScopedPointer<LowLevelGraphicsContext> glRenderer (createOpenGLGraphicsContext (openGLContext,
                                                                                        roundToInt (desktopScale * getWidth()),
                                                                                        roundToInt (desktopScale * getHeight())));
        
        if (glRenderer != nullptr)
        {
            Graphics g (*glRenderer);
            g.addTransform (AffineTransform::scale (desktopScale));
            
            float size = 0.25f;
            
            // This stuff just creates a spinning star shape and fills it..
            Path p;
            p.addStar (Point<float> (getWidth() * 0.5f,
                                     getHeight() * 0.5f), 7,
                       getHeight() * size * 0.5f,
                       getHeight() * size,
                       0.5f);
            
            float hue = 1.0f;
            
            g.setGradientFill (ColourGradient (Colours::green.withRotatedHue (hue).withAlpha (0.8f),
                                               0, 0,
                                               Colours::red.withRotatedHue (hue).withAlpha (0.5f),
                                               0, (float) getHeight(), false));
            g.fillPath (p);
        }
    }

private:
    
    //==============================================================================
    void setShaderProgram (const String& _vertexShader, const String& _fragmentShader)
    {
        vertexShader = _vertexShader;
        fragmentShader = _fragmentShader;
    }

    void updateShader()
    {
        if (vertexShader.isNotEmpty() || fragmentShader.isNotEmpty())
        {
            ScopedPointer<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));
            String statusText;
            
            if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
                && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
                && newShader->link())
            {
                shape = nullptr;
                attributes = nullptr;
                uniforms = nullptr;
                
                shader = newShader;
                shader->use();
                
                shape      = new Shape (openGLContext);
                attributes = new Attributes (openGLContext, *shader);
                uniforms   = new Uniforms (openGLContext, *shader);
                
                statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
            }
            else
            {
                statusText = newShader->getLastError();
            }
            
            vertexShader = String();
            fragmentShader = String();
        }
    }

    //==============================================================================
    struct ShaderPreset
    {
        const char* name;
        const char* vertexShader;
        const char* fragmentShader;
    };
    
    static Array<ShaderPreset> getPresets()
    {
#define SHADER_DEMO_HEADER \
"/*  This is a live OpenGL Shader demo.\n" \
"    Edit the shader program below and it will be \n" \
"    compiled and applied to the model above!\n" \
"*/\n\n"
        
        ShaderPreset presets[] =
        {
            {
                "Texture + Lighting",
                
                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 normal;\n"
                "attribute vec4 sourceColour;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "uniform vec4 lightPosition;\n"
                "\n"
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "varying float lightIntensity;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    destinationColour = sourceColour;\n"
                "    textureCoordOut = textureCoordIn;\n"
                "\n"
                "    vec4 light = viewMatrix * lightPosition;\n"
                "    lightIntensity = dot (light, normal);\n"
                "\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",
                
                SHADER_DEMO_HEADER
#if JUCE_OPENGL_ES
                "varying lowp vec4 destinationColour;\n"
                "varying lowp vec2 textureCoordOut;\n"
                "varying highp float lightIntensity;\n"
#else
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "varying float lightIntensity;\n"
#endif
                "\n"
                "uniform sampler2D demoTexture;\n"
                "\n"
                "void main()\n"
                "{\n"
#if JUCE_OPENGL_ES
                "   highp float l = max (0.3, lightIntensity * 0.3);\n"
                "   highp vec4 colour =  vec4 (l, l, l, 1.0);\n"
#else
                "   float l = max (0.3, lightIntensity * 0.3);\n"
                "   vec4 colour = destinationColour * vec4 (l, l, l, 1.0);\n"
#endif
                "    gl_FragColor = colour * texture2D (demoTexture, textureCoordOut);\n"
                "}\n"
            },
            
            {
                "Textured",
                
                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 sourceColour;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "\n"
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    destinationColour = sourceColour;\n"
                "    textureCoordOut = textureCoordIn;\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",
                
                SHADER_DEMO_HEADER
#if JUCE_OPENGL_ES
                "varying lowp vec4 destinationColour;\n"
                "varying lowp vec2 textureCoordOut;\n"
#else
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
#endif
                "\n"
                "uniform sampler2D demoTexture;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_FragColor = texture2D (demoTexture, textureCoordOut);\n"
                "}\n"
            },
            
            {
                "Flat Colour",
                
                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 sourceColour;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "\n"
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    destinationColour = sourceColour;\n"
                "    textureCoordOut = textureCoordIn;\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",
                
                SHADER_DEMO_HEADER
#if JUCE_OPENGL_ES
                "varying lowp vec4 destinationColour;\n"
                "varying lowp vec2 textureCoordOut;\n"
#else
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
#endif
                "\n"
                "void main()\n"
                "{\n"
                "    gl_FragColor = destinationColour;\n"
                "}\n"
            },
            
            {
                "Rainbow",
                
                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 sourceColour;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "\n"
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "\n"
                "varying float xPos;\n"
                "varying float yPos;\n"
                "varying float zPos;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 v = vec4 (position);\n"
                "    xPos = clamp (v.x, 0.0, 1.0);\n"
                "    yPos = clamp (v.y, 0.0, 1.0);\n"
                "    zPos = clamp (v.z, 0.0, 1.0);\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}",
                
                SHADER_DEMO_HEADER
#if JUCE_OPENGL_ES
                "varying lowp vec4 destinationColour;\n"
                "varying lowp vec2 textureCoordOut;\n"
                "varying lowp float xPos;\n"
                "varying lowp float yPos;\n"
                "varying lowp float zPos;\n"
#else
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "varying float xPos;\n"
                "varying float yPos;\n"
                "varying float zPos;\n"
#endif
                "\n"
                "void main()\n"
                "{\n"
                "    gl_FragColor = vec4 (xPos, yPos, zPos, 1.0);\n"
                "}"
            },
            
            {
                "Changing Colour",
                
                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "\n"
                "varying vec2 textureCoordOut;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    textureCoordOut = textureCoordIn;\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",
                
                SHADER_DEMO_HEADER
                "#define PI 3.1415926535897932384626433832795\n"
                "\n"
#if JUCE_OPENGL_ES
                "precision mediump float;\n"
                "varying lowp vec2 textureCoordOut;\n"
#else
                "varying vec2 textureCoordOut;\n"
#endif
                "uniform float bouncingNumber;\n"
                "\n"
                "void main()\n"
                "{\n"
                "   float b = bouncingNumber;\n"
                "   float n = b * PI * 2.0;\n"
                "   float sn = (sin (n * textureCoordOut.x) * 0.5) + 0.5;\n"
                "   float cn = (sin (n * textureCoordOut.y) * 0.5) + 0.5;\n"
                "\n"
                "   vec4 col = vec4 (b, sn, cn, 1.0);\n"
                "   gl_FragColor = col;\n"
                "}\n"
            },
            
            {
                "Simple Light",
                
                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 normal;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "uniform vec4 lightPosition;\n"
                "\n"
                "varying float lightIntensity;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 light = viewMatrix * lightPosition;\n"
                "    lightIntensity = dot (light, normal);\n"
                "\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",
                
                SHADER_DEMO_HEADER
#if JUCE_OPENGL_ES
                "varying highp float lightIntensity;\n"
#else
                "varying float lightIntensity;\n"
#endif
                "\n"
                "void main()\n"
                "{\n"
#if JUCE_OPENGL_ES
                "   highp float l = lightIntensity * 0.25;\n"
                "   highp vec4 colour = vec4 (l, l, l, 1.0);\n"
#else
                "   float l = lightIntensity * 0.25;\n"
                "   vec4 colour = vec4 (l, l, l, 1.0);\n"
#endif
                "\n"
                "    gl_FragColor = colour;\n"
                "}\n"
            },
            
            {
                "Flattened",
                
                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 normal;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "uniform vec4 lightPosition;\n"
                "\n"
                "varying float lightIntensity;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 light = viewMatrix * lightPosition;\n"
                "    lightIntensity = dot (light, normal);\n"
                "\n"
                "    vec4 v = vec4 (position);\n"
                "    v.z = v.z * 0.1;\n"
                "\n"
                "    gl_Position = projectionMatrix * viewMatrix * v;\n"
                "}\n",
                
                SHADER_DEMO_HEADER
#if JUCE_OPENGL_ES
                "varying highp float lightIntensity;\n"
#else
                "varying float lightIntensity;\n"
#endif
                "\n"
                "void main()\n"
                "{\n"
#if JUCE_OPENGL_ES
                "   highp float l = lightIntensity * 0.25;\n"
                "   highp vec4 colour = vec4 (l, l, l, 1.0);\n"
#else
                "   float l = lightIntensity * 0.25;\n"
                "   vec4 colour = vec4 (l, l, l, 1.0);\n"
#endif
                "\n"
                "    gl_FragColor = colour;\n"
                "}\n"
            },
            
            {
                "Toon Shader",
                
                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 normal;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "uniform vec4 lightPosition;\n"
                "\n"
                "varying float lightIntensity;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 light = viewMatrix * lightPosition;\n"
                "    lightIntensity = dot (light, normal);\n"
                "\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",
                
                SHADER_DEMO_HEADER
#if JUCE_OPENGL_ES
                "varying highp float lightIntensity;\n"
#else
                "varying float lightIntensity;\n"
#endif
                "\n"
                "void main()\n"
                "{\n"
#if JUCE_OPENGL_ES
                "    highp float intensity = lightIntensity * 0.5;\n"
                "    highp vec4 colour;\n"
#else
                "    float intensity = lightIntensity * 0.5;\n"
                "    vec4 colour;\n"
#endif
                "\n"
                "    if (intensity > 0.95)\n"
                "        colour = vec4 (1.0, 0.5, 0.5, 1.0);\n"
                "    else if (intensity > 0.5)\n"
                "        colour  = vec4 (0.6, 0.3, 0.3, 1.0);\n"
                "    else if (intensity > 0.25)\n"
                "        colour  = vec4 (0.4, 0.2, 0.2, 1.0);\n"
                "    else\n"
                "        colour  = vec4 (0.2, 0.1, 0.1, 1.0);\n"
                "\n"
                "    gl_FragColor = colour;\n"
                "}\n"
            }
        };
        
        return Array<ShaderPreset> (presets, numElementsInArray (presets));
    }

    
    struct Vertex
    {
        float position[3];
        float normal[3];
        float colour[4];
        float texCoord[2];
    };
    
    //==============================================================================
    // This class just manages the attributes that the shaders use.
    struct Attributes
    {
        Attributes (OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
        {
            position      = createAttribute (openGLContext, shaderProgram, "position");
            normal        = createAttribute (openGLContext, shaderProgram, "normal");
            sourceColour  = createAttribute (openGLContext, shaderProgram, "sourceColour");
            textureCoordIn = createAttribute (openGLContext, shaderProgram, "textureCoordIn");
        }
        
        void enable (OpenGLContext& openGLContext)
        {
            if (position != nullptr)
            {
                openGLContext.extensions.glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
                openGLContext.extensions.glEnableVertexAttribArray (position->attributeID);
            }
            
            if (normal != nullptr)
            {
                openGLContext.extensions.glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
                openGLContext.extensions.glEnableVertexAttribArray (normal->attributeID);
            }
            
            if (sourceColour != nullptr)
            {
                openGLContext.extensions.glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
                openGLContext.extensions.glEnableVertexAttribArray (sourceColour->attributeID);
            }
            
            if (textureCoordIn != nullptr)
            {
                openGLContext.extensions.glVertexAttribPointer (textureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
                openGLContext.extensions.glEnableVertexAttribArray (textureCoordIn->attributeID);
            }
        }
        
        void disable (OpenGLContext& openGLContext)
        {
            if (position != nullptr)       openGLContext.extensions.glDisableVertexAttribArray (position->attributeID);
            if (normal != nullptr)         openGLContext.extensions.glDisableVertexAttribArray (normal->attributeID);
            if (sourceColour != nullptr)   openGLContext.extensions.glDisableVertexAttribArray (sourceColour->attributeID);
            if (textureCoordIn != nullptr)  openGLContext.extensions.glDisableVertexAttribArray (textureCoordIn->attributeID);
        }
        
        ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;
        
    private:
        static OpenGLShaderProgram::Attribute* createAttribute (OpenGLContext& openGLContext,
                                                                OpenGLShaderProgram& shader,
                                                                const char* attributeName)
        {
            if (openGLContext.extensions.glGetAttribLocation (shader.getProgramID(), attributeName) < 0)
                return nullptr;
            
            return new OpenGLShaderProgram::Attribute (shader, attributeName);
        }
    };
    
    //==============================================================================
    // This class just manages the uniform values that the demo shaders use.
    struct Uniforms
    {
        Uniforms (OpenGLContext& openGLContext, OpenGLShaderProgram& shader)
        {
            projectionMatrix = createUniform (openGLContext, shader, "projectionMatrix");
            viewMatrix       = createUniform (openGLContext, shader, "viewMatrix");
            texture          = createUniform (openGLContext, shader, "demoTexture");
            lightPosition    = createUniform (openGLContext, shader, "lightPosition");
        }
        
        ScopedPointer<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix, texture, lightPosition;

    private:
        static OpenGLShaderProgram::Uniform* createUniform (OpenGLContext& openGLContext,
                                                            OpenGLShaderProgram& shader,
                                                            const char* uniformName)
        {
            if (openGLContext.extensions.glGetUniformLocation (shader.getProgramID(), uniformName) < 0)
                return nullptr;
            
            return new OpenGLShaderProgram::Uniform (shader, uniformName);
        }
    };
    
    //==============================================================================
    /** This loads a 3D model from an OBJ file and converts it into some vertex buffers
     that we can draw.
     */
    struct Shape
    {
        Shape (OpenGLContext& openGLContext)
        {
            if (shapeFile.load (BinaryData::teapot_obj).wasOk())
                for (int i = 0; i < shapeFile.shapes.size(); ++i)
                    vertexBuffers.add (new VertexBuffer (openGLContext, *shapeFile.shapes.getUnchecked(i)));
            
        }
        
        void draw (OpenGLContext& openGLContext, Attributes& attributes)
        {
            for (int i = 0; i < vertexBuffers.size(); ++i)
            {
                VertexBuffer& vertexBuffer = *vertexBuffers.getUnchecked (i);
                vertexBuffer.bind();
                
                attributes.enable (openGLContext);
                glDrawElements (GL_TRIANGLES, vertexBuffer.numIndices, GL_UNSIGNED_INT, 0);
                attributes.disable (openGLContext);
            }
        }
        
    private:
        struct VertexBuffer
        {
            VertexBuffer (OpenGLContext& context, WavefrontObjFile::Shape& shape) : openGLContext (context)
            {
                numIndices = shape.mesh.indices.size();
                
                openGLContext.extensions.glGenBuffers (1, &vertexBuffer);
                openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
                
                Array<Vertex> vertices;
                createVertexListFromMesh (shape.mesh, vertices, Colours::white);
                
                openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER, vertices.size() * (int) sizeof (Vertex),
                                                       vertices.getRawDataPointer(), GL_STATIC_DRAW);
                
                openGLContext.extensions.glGenBuffers (1, &indexBuffer);
                openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                openGLContext.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER, numIndices * (int) sizeof (juce::uint32),
                                                       shape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
            }
            
            ~VertexBuffer()
            {
                openGLContext.extensions.glDeleteBuffers (1, &vertexBuffer);
                openGLContext.extensions.glDeleteBuffers (1, &indexBuffer);
            }
            
            void bind()
            {
                openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
                openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            }
            
            GLuint vertexBuffer, indexBuffer;
            int numIndices;
            OpenGLContext& openGLContext;
            
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VertexBuffer)
        };
        
        WavefrontObjFile shapeFile;
        OwnedArray<VertexBuffer> vertexBuffers;
        
        static void createVertexListFromMesh (const WavefrontObjFile::Mesh& mesh, Array<Vertex>& list, Colour colour)
        {
            const float scale = 0.2f;
            WavefrontObjFile::TextureCoord defaultTexCoord = { 0.5f, 0.5f };
            WavefrontObjFile::Vertex defaultNormal = { 0.5f, 0.5f, 0.5f };
            
            for (int i = 0; i < mesh.vertices.size(); ++i)
            {
                const WavefrontObjFile::Vertex& v = mesh.vertices.getReference (i);
                
                const WavefrontObjFile::Vertex& n
                = i < mesh.normals.size() ? mesh.normals.getReference (i) : defaultNormal;
                
                const WavefrontObjFile::TextureCoord& tc
                = i < mesh.textureCoords.size() ? mesh.textureCoords.getReference (i) : defaultTexCoord;
                
                Vertex vert =
                {
                    { scale * v.x, scale * v.y, scale * v.z, },
                    { scale * n.x, scale * n.y, scale * n.z, },
                    { colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha() },
                    { tc.x, tc.y }
                };
                
                list.add (vert);
            }
        }
    };


#pragma mark Texture Handler
    //==============================================================================
    // These classes are used to load textures from the various sources that the demo uses..
    struct DemoTexture
    {
        virtual ~DemoTexture() {}
        virtual bool applyTo (OpenGLTexture&) = 0;
        
        String name;
    };
    
    struct BuiltInTexture   : public DemoTexture
    {
        BuiltInTexture (const char* nm, const void* imageData, size_t imageSize)
        : image (resizeImageToPowerOfTwo (ImageFileFormat::loadFrom (imageData, imageSize)))
        {
            name = nm;
        }
        
        Image image;
        
        bool applyTo (OpenGLTexture& texture) override
        {
            texture.loadImage (image);
            return false;
        }
    };
    
    struct TextureFromFile   : public DemoTexture
    {
        TextureFromFile (const File& file)
        {
            name = file.getFileName();
            image = resizeImageToPowerOfTwo (ImageFileFormat::loadFrom (file));
        }
        
        Image image;
        
        bool applyTo (OpenGLTexture& texture) override
        {
            texture.loadImage (image);
            return false;
        }
    };
    
    static Image resizeImageToPowerOfTwo (Image image)
    {
        if (! (isPowerOfTwo (image.getWidth()) && isPowerOfTwo (image.getHeight())))
            return image.rescaled (jmin (1024, nextPowerOfTwo (image.getWidth())),
                                   jmin (1024, nextPowerOfTwo (image.getHeight())));
        
        return image;
    }

    void setTexture (DemoTexture* t)
    {
        textureToUse = t;
    }

#pragma mark Private members
    String vertexShader, fragmentShader;
    ScopedPointer<OpenGLShaderProgram> shader;
    ScopedPointer<Shape> shape;
    ScopedPointer<Attributes> attributes;
    ScopedPointer<Uniforms> uniforms;
    Draggable3DOrientation draggableOrientation;
    float zoom;
    float rotation;
    ToolbarComponent toolbar;
    ComboBox presetBox, textureBox;
    Label presetLabel, textureLabel, statusLabel;
    OwnedArray<DemoTexture> textures;
    DemoTexture *textureToUse;
    OpenGLTexture texture;
    ComponentAnimator toolbarAnimator;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()    { return new MainContentComponent(); }
