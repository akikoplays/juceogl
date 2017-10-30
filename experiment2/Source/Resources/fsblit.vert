/* 

Simple vertex shader used for full screen quad rendering, useful when applying post fx from 
a RTT source buffer (FBO).

Following this thread: 
https://stackoverflow.com/questions/2588875/whats-the-best-way-to-draw-a-fullscreen-quad-in-opengl-3-2
*/

attribute vec4 position;
attribute vec2 textureCoordIn;
varying vec2 textureCoordOut;
void main()
{
        // const vec2 madd=vec2(0.5,0.5);
        textureCoordOut = textureCoordIn;
        // textureCoordOut = position.xz*madd+madd;
        gl_Position = vec4(5.0*position.x, 5.0*position.z, 0.5, 1.0);
}
