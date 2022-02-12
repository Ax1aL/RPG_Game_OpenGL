#version 400

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord;

out vec2 textureCoord;
out vec3 norm;
out vec3 fragPos;

uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform float offset;

void main()
{
	mat4 MVP=Projection*ViewMatrix*Model;

	textureCoord = texCoord;

	vec3 tempPos=pos;

	vec2 dir=vec2(1,1);
	float Amplitude=1.5f;
	float dotProd=dot(dir,vec2(pos.x,pos.z));

	float pi=180.f;

	//Frequency
	float wavelength=1400;
	float wi=2*pi/wavelength;
	float freq=2.f;
	//speed
	float qi=freq*wi;

	float steepness=0.4f;

	//logic for waves
	tempPos.x=pos.x+(steepness*Amplitude*dir.x*cos(wi*dotProd+offset*qi)) 
	+((steepness+0.1f)*Amplitude*dir.x*cos((wi-(wi/5.f))*dotProd+offset*qi*2.f))/2.f;

	tempPos.z=pos.z+(steepness*Amplitude*dir.y*cos(wi*dotProd+offset*qi)) 
	+((steepness+0.1f)*Amplitude*dir.y*cos((wi-(wi/5.f))*dotProd+offset*qi*2.f))/2.f;

	tempPos.y=Amplitude*sin(wi*dotProd+offset*qi)+(Amplitude-0.6f)*sin((wi-(wi/5.f))*dotProd+offset*qi*2.f)
	+0.4f*sin((wi-(wi/5.f))*pos.x+offset*qi*2.f) +0.9f*sin((wi-(wi/5.f))*pos.z+offset*qi*2.f);

	//for the edges to stya at 0
	if(abs(tempPos.x)>=70.f && abs(tempPos.z)>=70.f){
	tempPos.y=tempPos.y*((1-(abs(tempPos.x)-70.f)/10) *(1-(abs(tempPos.z)-70.f)/10));
	}else 
	if(abs(tempPos.x)>=70.f){
	tempPos.y=tempPos.y*(1-(abs(tempPos.x)-70.f)/10);
	}else 
	if(abs(tempPos.z)>=70.f){
	tempPos.y=tempPos.y*(1-(abs(tempPos.z)-70.f)/10);
	}
	

	fragPos = vec3(Model * vec4(tempPos, 1.0f));
	norm = mat3(transpose(inverse(Model)))*normals;
	gl_Position = MVP * vec4(tempPos, 1.0f);


}