/* 

Simple vertex shader used for full screen quad rendering, useful when applying post fx from 
a RTT source buffer (FBO).

Following this thread: https://stackoverflow.com/questions/2588875 whats-the-best-way-to-draw-a-fullscreen-quad-in-opengl-3-2

*/

// #define LUMA 0

// #if JUCE_OPENGL_ES
//     precision mediump float;
//     varying lowp vec2 textureCoordOut;
// #else
    varying vec2 textureCoordOut;
// #endif

uniform sampler2D demoTexture;
void main()
{
    vec4 texel = texture2D (demoTexture, textureCoordOut);
#if LUMA    
    float Y = 0.2126 * texel.x + 0.7152 * texel.y + 0.0722 * texel.z;
    gl_FragColor = vec4(Y, Y, Y, texel.w);
#else
    gl_FragColor = texel;
#endif
}
