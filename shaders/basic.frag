#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 firefly1Pos;
uniform vec3 candle1Pos;

//firefly
uniform int isFirefly;


// textures
uniform sampler2D diffuseTexture;
//uniform sampler2D specularTexture;

//fog
uniform int fog;
uniform float fogDensity;

//components
vec3 ambient;
float ambientStrength = 0.3f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.1f;

float constant = 0.01f;
float linear = 0.0f;
float quadratic = 0.03f;

float shininess = 60.0f;




vec3 fly1Color = vec3(0.0f,0.0f,1.0f);
vec3 candleColor = vec3(1.0f,0.6f,0.0f);

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor*0.1f;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor * 0.1f;

    //compute specular light
	vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    
    specular = specularStrength * specCoeff * lightColor *0.1f;
}

void computePointLight(){
	
	vec3 vPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	vec3 normalEye = normalize(fNormal*normalMatrix);	
	vec4 fPosEye = view * model * vec4(fPosition, 1.0f);

	//compute view direction
	vec3 viewDirN = normalize(vPosEye - fPosEye.xyz);
	
	//normalize light direction
	
	vec3 lightDirN = normalize(lightDir);
	
	vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflectDir), 0.0f), 32);

	ambient = ambientStrength * lightColor;
	diffuse =  max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	specular = specularStrength * specCoeff * fly1Color;
	

	
	//firefly1

	
	lightDirN = normalize(firefly1Pos - fPosition.xyz); 
	
	//compute half vector
	//vec3 halfVector = normalize(lightDirN + viewDirN);
	
	
	//compute specular light
	//specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	
	
	float dist = length(firefly1Pos - fPosition.xyz); 
	
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist)) *0.1f;
	//compute ambient light
	ambient += att * ambientStrength * fly1Color ;
	//compute diffuse light
	diffuse += att * max(dot(normalEye, lightDirN), 0.0f) * fly1Color;
	specular += att * specularStrength * specCoeff * fly1Color;
	
	
	//candle1

	
	lightDirN = normalize(candle1Pos - fPosition.xyz); 
	
	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
	
	
	//compute specular light
	specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	
	
	dist = length(candle1Pos - fPosition.xyz); 
	
	att = 1.0f / (constant + linear * dist + quadratic * (dist * dist))*1.0f;
	//compute ambient light
	ambient += att * ambientStrength * candleColor ;
	//compute diffuse light
	diffuse += att * max(dot(normalEye, lightDirN), 0.0f) * candleColor;
	specular += att * specularStrength * specCoeff * candleColor;


}



float computeFog()
{
 vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
 //float fogDensity = 0.05f05f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}


void main() 
{
	
	if(isFirefly == 1){
		fColor = vec4(1.0f,1.0f,0.7f,1.0f);
		return;
	}
    computeDirLight();
	computePointLight();
	
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1)
		discard;


	ambient *= colorFromTexture.rgb;
	diffuse *= colorFromTexture.rgb;
	specular *= colorFromTexture.rgb;
	
	vec4 firefly1PosEye = view * vec4(firefly1Pos, 1.0f);


    //compute final vertex color
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular *texture(diffuseTexture, fTexCoords).rgb,1.0f);
	
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//fColor = fogColor
	
	//fColor = colorFromTexture;
	//fColor = fogColor*(1-fogFactor)+color*fogFactor;

	fColor = vec4((mix(fogColor, vec4(color, 1.0f), fogFactor)).xyz, colorFromTexture.a);
	

}
 