/*
  ==============================================================================

    Renderer.cpp
    Created: 24 Oct 2017 11:18:36am
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "BallMenu.h"
#include "MainWindow.h"
#include "Renderer.h"

using namespace std;

#define USE_RTT
#define RTT_WIDTH 1024
#define RTT_HEIGHT 1024

//==============================================================================
Renderer::Renderer()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setOpaque (true);
    if (MainWindow* mw = MainWindow::getMainAppWindow())
        mw->setRenderingEngine (0);

    openGLContext.setRenderer (this);
    openGLContext.attachTo (*this);
    openGLContext.setContinuousRepainting (true);
    
    // Load default texture, configure camera and renderer
    textures.add(new BuiltInTexture("Checker", BinaryData::checker_jpg, BinaryData::checker_jpgSize));
    rotation = 0.0f;
    zoom = 0.4f;

    // Load gradient texture
    textures.add(new BuiltInTexture("Background", BinaryData::lightbluebackground_jpg, BinaryData::lightbluebackground_jpgSize));

    // Use this texture for object mapping
    selectTexture(0);
    
    mainCaption = "3D Viewer";
}

Renderer::~Renderer()
{
    openGLContext.detach();
}

void Renderer::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//    g.setColour (Colours::darkgrey);
//    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
//    g.setColour (Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("Renderer", getLocalBounds(),
//                Justification::centred, true);   // draw some placeholder text
    
    g.setColour (getLookAndFeel().findColour (Label::textColourId));
    g.setFont (20);
    g.drawText (mainCaption, 25, 20, 300, 30, Justification::left);
    g.drawLine (20, 20, 170, 20);
    g.drawLine (20, 50, 170, 50);
}

void Renderer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    // Must set for mouse interactions
    draggableOrientation.setViewport (getLocalBounds());
    if (ballMenu != nullptr) {
        Rectangle<int> bounds = getLocalBounds();
        int w = 600;
        int h = 400;
        bounds.setWidth(w);
        bounds.setHeight(h);
        bounds.setX(getLocalBounds().getWidth()/2 - w/2);
        bounds.setY(getLocalBounds().getHeight()/2 - h/2);
        ballMenu->setBounds(bounds);
    }
}

void Renderer::newOpenGLContextCreated()
{
    // nothing to do in this case - we'll initialise our shaders + textures
    // on demand, during the render callback.
    
#ifdef USE_RTT
    // test off screen render
    // todo: reinitialise FBO with current window size during resize()
    fbo.initialise(openGLContext, RTT_WIDTH, RTT_HEIGHT);
#endif
    
    // Load plane obj used to for RTT processing
    shape = new Shape(openGLContext, "../../../../Source/Resources/teapot.obj");
    planeShape = new Shape(openGLContext, "../../../../Source/Resources/plane.obj");

    // Manually load some shaders
    Shader *shader = new Shader(openGLContext, "fsblitter", "../../../../Source/Resources/fsblit", "#define LUMA 0\n#define JUCE_OPENGL_ES 0\n");
    shaders.add(shader);
    shader = new Shader(openGLContext, "fsblitterluma", "../../../../Source/Resources/fsblit", "#define LUMA 1\n#define JUCE_OPENGL_ES 0\n");
    shaders.add(shader);
    shader = new Shader(openGLContext, "tex_light", "../../../../Source/Resources/tex_light", "#define JUCE_OPENGL_ES 0\n");
    shaders.add(shader);

    // Load backgdround gradient image and create OGL texture
    Texture *t = getTextureByName("Background");
    jassert(t);
    t->applyTo(backgroundTexture);
}

void Renderer::openGLContextClosing()
{
    // When the context is about to close, you must use this callback to delete
    // any GPU resources while the context is still current.
    shape = nullptr;
    planeShape = nullptr;
    texture.release();
    backgroundTexture.release();
#ifdef USE_RTT
    fbo.release();
#endif
}

// This is a virtual method in OpenGLRenderer, and is called when it's time
// to do your GL rendering.
void Renderer::renderOpenGL()
{
    // todo: this is a bit bruteforced
    repaint();
    
    jassert (OpenGLHelpers::isContextActive());
    const float desktopScale = (float) openGLContext.getRenderingScale();
    OpenGLHelpers::clear (Colours::darkblue);
    
#ifdef USE_RTT
    glViewport (0, 0, roundToInt(fbo.getWidth()), roundToInt(fbo.getHeight()));
#else
    glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));
#endif

    
#ifdef USE_RTT
    fbo.makeCurrentAndClear();
#endif

    // Having used the juce 2D renderer, it will have messed-up a whole load of GL state, so
    // we need to initialise some important settings before doing our normal GL 3D drawing..
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LESS);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    openGLContext.extensions.glActiveTexture (GL_TEXTURE0);
    glEnable (GL_TEXTURE_2D);

    // Render background gradient
    backgroundTexture.bind();
    
    // -------------------------------------------------------
    // FS blitter
    // -------------------------------------------------------
    const Shader *shader;
    Attributes *attr;
    
    shader = getShaderByName("fsblitter");
    jassert(shader);
    shader->shader->use();
    
    if (shader->uniforms->texture != nullptr)
        shader->uniforms->texture->set ((GLint) 0);
    attr = shader->attributes;
    planeShape->draw(openGLContext, *attr);

    
    // -------------------------------------------------------
    // Render object:
    // -------------------------------------------------------
    if (textureToUse != nullptr)
        if (! textureToUse->applyTo (texture))
            textureToUse = nullptr;
    
    texture.bind();
    
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    shader = getShaderByName("tex_light");
    jassert(shader);
    shader->shader->use();
    
    if (shader->uniforms->projectionMatrix != nullptr)
        shader->uniforms->projectionMatrix->setMatrix4 (getProjectionMatrix().mat, 1, false);
    
    if (shader->uniforms->viewMatrix != nullptr)
        shader->uniforms->viewMatrix->setMatrix4 (getViewMatrix().mat, 1, false);
    
    if (shader->uniforms->texture != nullptr)
        shader->uniforms->texture->set ((GLint) 0);
    
    if (shader->uniforms->lightPosition != nullptr)
        shader->uniforms->lightPosition->set (-15.0f, 10.0f, 15.0f, 0.0f);
    
    attr = shader->attributes;
    shape->draw(openGLContext, *attr);
    
    // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    
#ifdef USE_RTT
    // -------------------------------------------------------
    // Post fx RTT blitter
    // -------------------------------------------------------

    fbo.releaseAsRenderingTarget();
    
    openGLContext.extensions.glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, fbo.getTextureID());
    glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));

    if (ballMenu)
        shader = getShaderByName("fsblitterluma");
    else
        shader = getShaderByName("fsblitter");
    jassert(shader);
    shader->shader->use();

    if (shader->uniforms->texture != nullptr)
        shader->uniforms->texture->set ((GLint) 0);
    attr = shader->attributes;
    planeShape->draw(openGLContext, *attr);
#endif

}

void Renderer::selectTexture (int itemID)
{
    if (Texture* t = textures[itemID])
        setTexture(t);
}

Matrix3D<float> Renderer::getProjectionMatrix() const
{
    float w = 1.0f / (zoom + 0.1f);
    float h = w * getLocalBounds().toFloat().getAspectRatio (false);
    return Matrix3D<float>::fromFrustum (-w, w, -h, h, 4.0f, 30.0f);
}

Matrix3D<float> Renderer::getViewMatrix() const
{
    Matrix3D<float> viewMatrix = draggableOrientation.getRotationMatrix() * Vector3D<float> (0.0f, 0.0f, -10.0f);
    Matrix3D<float> rotationMatrix = viewMatrix.rotated (Vector3D<float> (rotation, rotation, -0.3f));
    return rotationMatrix * viewMatrix;
}

void Renderer::setTexture (Texture* t)
{
    textureToUse = t;
}

void Renderer::mouseMove (const MouseEvent& e)
{
}

void Renderer::mouseDown (const MouseEvent& e)
{
    draggableOrientation.mouseDown (e.getPosition());
    clickStart = Time::currentTimeMillis();
    wasDrag = false;
}

void Renderer::mouseUp (const MouseEvent& e)
{
    cout << "Mouse down event" << endl;
    if (Time::currentTimeMillis() - clickStart > 50 && !wasDrag) {
        cout << "-- >50ms" << endl;
        if (ballMenu == nullptr){
            ballMenu = new BallMenu();
            addAndMakeVisible(ballMenu);
            Rectangle<int> bounds = getLocalBounds();
            int w = 600;
            int h = 400;
            bounds.setWidth(w);
            bounds.setHeight(h);
            bounds.setX(getLocalBounds().getWidth()/2 - w/2);
            bounds.setY(getLocalBounds().getHeight()/2 - h/2);
            ballMenu->setBounds(bounds);
            ballMenu->launch();
            mainCaption = "Apply effect";
        }
    } else {
        cout << "-- <50ms" << endl;
        draggableOrientation.mouseDown (e.getPosition());
    }
}

void Renderer::mouseDrag (const MouseEvent& e)
{
    draggableOrientation.mouseDrag (e.getPosition());
    if (Time::currentTimeMillis() - clickStart > 100)
        wasDrag = true;
}

void Renderer::mouseWheelMove (const MouseEvent&, const MouseWheelDetails& d)
{
    zoom += d.deltaY;
    zoom = max(0.0f, min(1000.0f, zoom));
}

void Renderer::mouseMagnify (const MouseEvent&, float magnifyAmmount)
{
    zoom += magnifyAmmount - 1.0f;
}

bool Renderer::isInterestedInFileDrag (const StringArray&)
{
    return true;
}

void Renderer::filesDropped (const StringArray& filenames, int /* x */, int /* y */)
{
    String filename = filenames[0];
    StringArray tokens;
    tokens.addTokens(filename, ".", "\"");
    String ext = tokens[tokens.size()-1];
    ext.toLowerCase();
    if (ext == "jpg" || ext == "png" || ext == "jpeg" || ext == "gif") {
        TextureFromFile *tex = new TextureFromFile(filenames[0]);
        if (!tex->image.isValid()){
            cout << "Warning: failed to load texture " << filenames[0] << endl;
            delete tex;
            return;
        }
        textures.add (tex);
        selectTexture(textures.size()-1);
    } else if (ext == "obj"){
        // todo: support obj loading here
    }
}

const Shader* Renderer::getShaderByName(const String name)
{
    for (int i=0; i<shaders.size(); i++) {
        Shader *s = shaders[i];
        if (s->name == name)
            return s;
    }
    return nullptr;
}

Texture* Renderer::getTextureByName(const String name)
{
    for (int i=0; i<textures.size(); i++) {
        Texture *t = textures[i];
        if (t->name == name)
            return t;
    }
    return nullptr;
}

void Renderer::hideBallMenu()
{
    if (ballMenu) {
        ballMenu = nullptr;
        mainCaption = "3D Viewer";
    }
}

Shader::Shader(OpenGLContext& openGLContext, String _name, String baseFilename, String preprocdefines)
{
    cout << "Loading vertex shader " << _name << " from file: " << baseFilename << endl;
    File vertFile(baseFilename + ".vert");
    if (!vertFile.existsAsFile()) {
        cout << "Error!" << endl;
        return;
    }
    
    cout << "Loading fragment shader " << _name << " from file: " << baseFilename << endl;
    File fragFile(baseFilename + ".frag");
    if (!fragFile.existsAsFile()) {
        cout << "Error!" << endl;
        return;
    }

    build(openGLContext, _name, preprocdefines + "\n" + vertFile.loadFileAsString(), preprocdefines + "\n" + fragFile.loadFileAsString());
}

Shader::Shader(OpenGLContext& openGLContext, String _name, String vertexShader, String fragmentShader, String preprocdefines)
{
    build(openGLContext, _name, preprocdefines + "\n" + vertexShader, preprocdefines + "\n" + fragmentShader);
}

Shader::~Shader()
{
    attributes = nullptr;
    uniforms = nullptr;
    shader = nullptr;
}

void Shader::build(OpenGLContext& openGLContext, String _name, String vertexShader, String fragmentShader)
{
    
    cout << "Shader Assembler, assembling shader: " << _name << endl;
    name = _name;
    shader = new OpenGLShaderProgram(openGLContext);
    if (shader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
        && shader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
        && shader->link())
    {
        attributes = new Attributes (openGLContext, *shader);
        uniforms   = new Uniforms (openGLContext, *shader);
        
        cout << endl << "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2) << endl;
        ok = true;
    }
    else
    {
        ok = false;
        cout << endl << "Error: " << shader->getLastError() << endl;
    }
}

const bool Shader::isOk()
{
    return ok;
}

