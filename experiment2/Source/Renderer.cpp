/*
  ==============================================================================

    Renderer.cpp
    Created: 24 Oct 2017 11:18:36am
    Author:  BorisP

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Renderer.h"
#include "MainWindow.h"

using namespace std;

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
    
    selectPreset(0);
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
}

void Renderer::newOpenGLContextCreated()
{
    // nothing to do in this case - we'll initialise our shaders + textures
    // on demand, during the render callback.
}

void Renderer::openGLContextClosing()
{
    // When the context is about to close, you must use this callback to delete
    // any GPU resources while the context is still current.
    shape = nullptr;
    shader = nullptr;
    attributes = nullptr;
    uniforms = nullptr;
    texture.release();
}

// This is a virtual method in OpenGLRenderer, and is called when it's time
// to do your GL rendering.
void Renderer::renderOpenGL()
{
    jassert (OpenGLHelpers::isContextActive());
    const float desktopScale = (float) openGLContext.getRenderingScale();
    OpenGLHelpers::clear (Colours::black);
    
    jassert (OpenGLHelpers::isContextActive());
    
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
}

void Renderer::selectPreset (int preset)
{
    const ShaderPreset& p = ShaderPreset::getPresets()[preset];
    setShaderProgram(p.vertexShader, p.fragmentShader);
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

void Renderer::setShaderProgram (const String& _vertexShader, const String& _fragmentShader)
{
    vertexShader = _vertexShader;
    fragmentShader = _fragmentShader;
}

void Renderer::updateShader()
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
    TextureFromFile *tex = new TextureFromFile(filenames[0]);
    if (!tex->image.isValid()){
        cout << "Warning: failed to load texture " << filenames[0] << endl;
        delete tex;
        return;
    }
    textures.add (tex);
    selectTexture(textures.size()-1);
}

