/*                 Texture + Lighting
*/              
#if JUCE_OPENGL_ES
                 varying lowp vec4 destinationColour;  
                 varying lowp vec2 textureCoordOut;  
                 varying highp float lightIntensity;  
#else
                 varying vec4 destinationColour;  
                 varying vec2 textureCoordOut;  
                 varying float lightIntensity;  
#endif
                   
                 uniform sampler2D demoTexture;  
                   
                 void main()  
                 {  
#if JUCE_OPENGL_ES
                    highp float l = max (0.5, lightIntensity * 0.5);  
                    highp vec4 colour =  vec4 (l, l, l, 1.0);  
#else
                    float l = max (0.5, lightIntensity * 0.5);  
                    vec4 colour = destinationColour * vec4 (l, l, l, 1.0);  
#endif
                     gl_FragColor = colour * texture2D (demoTexture, textureCoordOut);  
                 }  
