#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec3 aVertexColor;
layout( location = 3 ) in uint aVertexVis;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;

// uniform mat4 uMVPMatrix;
// uniform mat4 uMVMatrix;
// uniform mat4 uNormalMatrix;

// out vec3 vPosVC;
// out vec3 vNormVC;
// out vec2 vTexCoords;
out vec4 position;
out vec3 normal;
out vec3 color;

void main()
{
	if(aVertexVis == 0)
	{
		return;
	}
	// Passage en coordonnées homogènes
	// vec4 vertexPosition = vec4( aVertexPosition, 1 );
	// vec4 vertexNormal	= vec4( aVertexNormal, 0 );

	// Calcul des valeurs de sortie
	// vPosVC	   = vec3( uMVMatrix * vertexPosition );
	// vNormVC	   = vec3( uNormalMatrix * vertexNormal );
	// vTexCoords = aVertexTexCoords;

	// Calcul de la position projetée
	// gl_Position = uMVPMatrix * vertexPosition;

	position = uProjMatrix * uMVMatrix * vec4( aVertexPosition, 1.f );
	normal	 = (transpose(inverse(uMVMatrix)) * vec4(aVertexNormal, 1.f)).xyz;
	color	 = aVertexColor;

	gl_Position = position;
}
