/*                 Texture + Lighting
*/              
 attribute vec4 position;  
 attribute vec4 normal;  
 attribute vec4 sourceColour;  
 attribute vec2 textureCoordIn;  
   
 uniform mat4 projectionMatrix;  
 uniform mat4 viewMatrix;  
 uniform vec4 lightPosition;  
   
 varying vec4 destinationColour;  
 varying vec2 textureCoordOut;  
 varying float lightIntensity;  
   
 void main()  
 {  
     destinationColour = sourceColour;  
     textureCoordOut = textureCoordIn;  
   
     vec4 light = viewMatrix * lightPosition;  
     lightIntensity = dot (light, normal);  
   
     gl_Position = projectionMatrix * viewMatrix * position;  
 } 
