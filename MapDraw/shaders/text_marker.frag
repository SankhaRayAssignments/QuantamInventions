
/* Simple Vertex Shader */

varying vec2 vTexCoord;

uniform sampler2D sTexture;

void main()
{	
	//gl_FragColor =  texture2D(sTexture,vTexCoord) + vec4(0,0.5,0,1);
	gl_FragColor =  texture2D(sTexture,vTexCoord);
}