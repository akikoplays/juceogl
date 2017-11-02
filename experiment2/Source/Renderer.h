/*
  ==============================================================================

    Renderer.h
    Created: 24 Oct 2017 11:18:36am
    Author:  BorisP

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveObjParser.h"

class BallMenu;

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
    Shape (OpenGLContext& openGLContext, String filename)
    {
        if (shapeFile.load(File(filename)).wasOk()) {
            for (int i = 0; i < shapeFile.shapes.size(); ++i)
                vertexBuffers.add (new VertexBuffer (openGLContext, *shapeFile.shapes.getUnchecked(i)));
        }
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

class ImageAux
{
public:
    static Image resizeImageToPowerOfTwo(Image image)
    {
        if (! (isPowerOfTwo (image.getWidth()) && isPowerOfTwo (image.getHeight())))
            return image.rescaled (jmin (1024, nextPowerOfTwo (image.getWidth())),
                                   jmin (1024, nextPowerOfTwo (image.getHeight())));
        
        return image;
    }
};


//==============================================================================
// These classes are used to load textures from the various sources that the demo uses..
struct Texture
{
    virtual ~Texture() {}
    virtual bool applyTo (OpenGLTexture&) = 0;
    
    String name;
};

struct BuiltInTexture   : public Texture
{
    BuiltInTexture (const char* nm, const void* imageData, size_t imageSize)
    //: image (resizeImageToPowerOfTwo (ImageFileFormat::loadFrom (imageData, imageSize)))
    {
        image = ImageAux::resizeImageToPowerOfTwo (ImageFileFormat::loadFrom (imageData, imageSize));
        name = nm;
    }
    
    Image image;
    
    bool applyTo (OpenGLTexture& texture) override
    {
        texture.loadImage (image);
        return false;
    }
};

struct TextureFromFile   : public Texture
{
    TextureFromFile (const File& file)
    {
        name = file.getFileName();
        image = ImageAux::resizeImageToPowerOfTwo (ImageFileFormat::loadFrom (file));
    }
    
    Image image;
    
    bool applyTo (OpenGLTexture& texture) override
    {
        texture.loadImage (image);
        return false;
    }
};


class Shader {
public:
    Shader(OpenGLContext& openGLContext, String _name, String baseFilename, String preprocdefines);
    Shader(OpenGLContext& openGLContext, String _name, String vertexShader, String fragmentShader, String preprocdefines);
    ~Shader();
    const bool isOk();
private:
    void build(OpenGLContext& openGLContext, String _name, String vertexShader, String fragmentShader);
public:
    ScopedPointer<OpenGLShaderProgram> shader;
    ScopedPointer<Attributes> attributes;
    ScopedPointer<Uniforms> uniforms;
    String name;
    
private:
    bool ok;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Shader)
};

//==============================================================================
/*
*/
class Renderer    : public Component,
                    public FileDragAndDropTarget,
                    private OpenGLRenderer
{
public:
    Renderer();
    ~Renderer();

    // Inherited methods:
    void paint (Graphics&) override;
    void resized() override;
    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void renderOpenGL() override;
    void mouseMove (const MouseEvent& e) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& d) override;
    void mouseMagnify (const MouseEvent&, float magnifyAmmount) override;
    void filesDropped (const StringArray& filenames, int /* x */, int /* y */) override;
    bool isInterestedInFileDrag (const StringArray&) override;

    // User defined methods:
    void selectTexture (int itemID);
    Matrix3D<float> getProjectionMatrix() const;
    Matrix3D<float> getViewMatrix() const;
    const Shader* getShaderByName(const String name);
    Texture* getTextureByName(const String name);
    void setTexture (Texture* t);
    void hideBallMenu();
    void executeUserMenu(const Button *button, const String name);
    
private:
    File planeObj;
    ScopedPointer<Shape> planeShape;
    ScopedPointer<Shape> shape;
    ScopedPointer<Shader> fsBlitterShader;
    Draggable3DOrientation draggableOrientation;
    ScopedPointer<BallMenu> ballMenu;
    float zoom;
    float rotation;
    OwnedArray<Texture> textures;
    OwnedArray<Shader> shaders;
    Texture *textureToUse;
    OpenGLTexture texture;
    OpenGLTexture backgroundTexture;
    ComponentAnimator toolbarAnimator;
    OpenGLContext openGLContext;
    OpenGLFrameBuffer fbo;
    String mainCaption;
    int64 clickStart;
    bool wasDrag;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Renderer)
};
