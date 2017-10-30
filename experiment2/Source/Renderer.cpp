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
    textures.add (new BuiltInTexture ("Checker", BinaryData::checker_jpg, BinaryData::checker_jpgSize));
    rotation = 0.0f;
    zoom = 0.4f;
    
//    selectPreset(0);
    selectTexture(0);
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

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("Renderer", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void Renderer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    // Must set for mouse interactions
    draggableOrientation.setViewport (getLocalBounds());
    if (ballMenu != nullptr) {
        ballMenu->setBounds(getLocalBounds().reduced(10,10));
    }
}

void Renderer::newOpenGLContextCreated()
{
    // nothing to do in this case - we'll initialise our shaders + textures
    // on demand, during the render callback.
    
#ifdef USE_RTT
    // test off screen render
    // todo: reinitialise FBO with current window size during resize()
    fbo.initialise(openGLContext, 512, 512);
#endif
    
    // Load plane obj used to for RTT processing
    shape = new Shape(openGLContext, "../../../../Source/Resources/teapot.obj");
    planeShape = new Shape(openGLContext, "../../../../Source/Resources/plane.obj");

    for (int i=0; i<ShaderPreset::getPresets().size(); i++) {
        const ShaderPreset p = ShaderPreset::getPresets()[i];
        cout << "Linking shader: " << p.name;
        shaders.add(new Shader(openGLContext, p.name, p.vertexShader, p.fragmentShader));
        cout << " OK! " << endl;
    }
    
    // Manually load some shaders
    fsBlitterShader = new Shader(openGLContext, "fsblitter", "../../../../Source/Resources/fsblit");
    cout << "Done" << endl;
}

void Renderer::openGLContextClosing()
{
    // When the context is about to close, you must use this callback to delete
    // any GPU resources while the context is still current.
    shape = nullptr;
    planeShape = nullptr;
    texture.release();
#ifdef USE_RTT
    fbo.release();
#endif
}

// This is a virtual method in OpenGLRenderer, and is called when it's time
// to do your GL rendering.
void Renderer::renderOpenGL()
{
    jassert (OpenGLHelpers::isContextActive());
    const float desktopScale = (float) openGLContext.getRenderingScale();
    OpenGLHelpers::clear (Colours::darkblue);
    
    if (textureToUse != nullptr)
        if (! textureToUse->applyTo (texture))
            textureToUse = nullptr;

    const Shader *shader = getShaderByName("Texture + Lighting");
    if(shader == nullptr)
        return;
    
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
    
    //glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));
    glViewport (0, 0, roundToInt(fbo.getWidth()), roundToInt(fbo.getHeight()));
    
    texture.bind();
    
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    shader->shader->use();
    
    if (shader->uniforms->projectionMatrix != nullptr)
        shader->uniforms->projectionMatrix->setMatrix4 (getProjectionMatrix().mat, 1, false);
    
    if (shader->uniforms->viewMatrix != nullptr)
        shader->uniforms->viewMatrix->setMatrix4 (getViewMatrix().mat, 1, false);
    
    if (shader->uniforms->texture != nullptr)
        shader->uniforms->texture->set ((GLint) 0);
    
    if (shader->uniforms->lightPosition != nullptr)
        shader->uniforms->lightPosition->set (-15.0f, 10.0f, 15.0f, 0.0f);
    
    Attributes *attr = shader->attributes;
    shape->draw(openGLContext, *attr);
    
    // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    
#ifdef USE_RTT
    fbo.releaseAsRenderingTarget();
    
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LESS);

    openGLContext.extensions.glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, fbo.getTextureID());
    glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));

    // Post fx blitter
    shader = fsBlitterShader;
    if(shader == nullptr)
        return;
    
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
    cout << "Mouse down event" << endl;
    if (ballMenu == nullptr){
        ballMenu = new BallMenu();
        addAndMakeVisible(ballMenu);
        ballMenu->setBounds(getLocalBounds().reduced(10,10));
        ballMenu->launch();
    }
}

void Renderer::mouseDrag (const MouseEvent& e)
{
    draggableOrientation.mouseDrag (e.getPosition());
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

Shader::Shader(OpenGLContext& openGLContext, String _name, String baseFilename)
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

    build(openGLContext, _name, vertFile.loadFileAsString(), fragFile.loadFileAsString());
}

Shader::Shader(OpenGLContext& openGLContext, String _name, String vertexShader, String fragmentShader)
{
    build(openGLContext, _name, vertexShader, fragmentShader);
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

