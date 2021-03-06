/* Simple Vertex Shader */
 
attribute vec4 aPosition; 
attribute vec4 aColor;

varying vec4 vColor;

uniform mat4 uMVPMatrix;

void main() { 
  gl_Position = uMVPMatrix * aPosition; 
  vColor = aColor;
}