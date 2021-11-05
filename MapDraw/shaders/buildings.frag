
/* Simple Vertex Shader */

varying vec4 vColor;

void main()
{	
	gl_FragColor = vColor;
	//gl_FragColor = vec4(1.0, 1.0, 0.66 ,1.0);
}