#version 400
precision highp float;

#define BOUNDING_RADIUS_2	1.4
#define ESCAPE_THRESHOLD	1e1
#define DEL					1e-4

uniform vec3 u_r0;				// TEXCOORD0 ray origin
//uniform vec3 rD;				// TEXCOORD1 ray direction (unit length)
uniform vec2 size;
uniform vec4 camera;

//uniform vec4 mu;				// quaternion constant specifying the particular set (C)
//uniform float epsilon;			//
//uniform vec3 eye;
//uniform vec3 light;
//uniform bool renderShadows;
//uniform int maxIterations;

in vec2 fragCoord;

out vec4 vFragColor;

//vec3 r0						= u_r0;			
//vec3 rD						= normalize(vec3(fragCoord, -1.0));

//const vec4 mu				= vec4(-0.591,-0.399,0.339,0.437);
const vec4 mu				= vec4(-0.04, 0, 0.72, 0);
const float epsilon			= 1e-4;							
const vec3 light			= vec3(2.0, 2.0, 2.0);
const bool renderShadows	= true;
const int maxIterations		= 8;
//const vec4 camera			= vec4(-50, 0, -30, 3);
const vec4 backgroundColor = vec4(0.3, 0.3, 0.3, 1.0);

float aspectRatio = size.x / size.y;

// Camera orientation
float c1 = cos(-camera.x);
float s1 = sin(-camera.x);
mat3 viewRotationY = mat3( c1, 0, s1, 
						   0, 1, 0, 
						 -s1, 0, c1);

float c2 = cos(-camera.y);
float s2 = sin(-camera.y);
    
mat3 viewRotationZ = mat3( c2, -s2, 0,
                              s2, c2, 0, 
                              0, 0, 1);
    
float c3 = cos(-camera.z);
float s3 = sin(-camera.z);
    
mat3 viewRotationX = mat3( 1, 0, 0,
                              0, c3, -s3, 
                              0, s3, c3);
    
    
mat3 viewRotation = viewRotationX * viewRotationY * viewRotationZ;
    
vec3 eye = vec3(0, 0, camera.w) * viewRotation;
vec3 lightSource = light * viewRotation;

vec4 quatMult(vec4 q1, vec4 q2)
{
	vec4 r;

	//// optimized
	//r.x = q1.x*q2.x - q1.y*q2.y - q1.z*q2.z - q1.w*q2.w;
	//r.y = q1.x*q2.y + q1.y*q2.x + q1.z*q2.w - q1.w*q2.z;
	//r.z = q1.x*q2.z - q1.y*q2.w + q1.z*q2.x - q1.w*q2.y;
	//r.w = q1.x*q2.w + q1.y*q2.z - q1.z*q2.y - q1.w*q2.x;

	r.x = q1.x * q2.x - dot(q1.yzw, q2.yzw);
	r.yzw = q1.x * q2.yzw + q2.x * q1.yzw + cross(q1.yzw, q2.yzw);

	return r;
}

// q^2
vec4 quatSq(vec4 q)
{
	vec4 r;

	// optimized
	r.x = q.x*q.x - q.y*q.y - q.z*q.z - q.w*q.w;
	r.y = 2.0*q.x*q.y;
	r.z = 2.0*q.x*q.z;
	r.w = 2.0*q.x*q.w;

	//r.x = q.x * q.x - dot(q.yzw, q.yzw);
	//r.yzw = 2 * q.x * q.yzw;

	return r;
}

void iterateIntersect(inout vec4 q, inout vec4 qp, vec4 c, int maxIterations)
{
	for (int i=0; i < maxIterations; ++i) {
		qp = 2.0 * quatMult(q, qp);
		q = quatSq(q) + c;

		if (dot(q, q) > ESCAPE_THRESHOLD) {
			break;
		}
	}
}

vec3 normEstimate(vec3 p, vec4 c, int maxIterations)
{
	vec3 N;
	vec4 qP = vec4(p, 0.0);
	float gradX, gradY, gradZ;

	vec4 gx1 = qP - vec4(DEL, 0.0, 0.0, 0.0);
	vec4 gx2 = qP + vec4(DEL, 0.0, 0.0, 0.0);
	vec4 gy1 = qP - vec4(0.0, DEL, 0.0, 0.0);
	vec4 gy2 = qP + vec4(0.0, DEL, 0.0, 0.0);
	vec4 gz1 = qP - vec4(0.0, 0.0, DEL, 0.0);
	vec4 gz2 = qP + vec4(0.0, 0.0, DEL, 0.0);

	for (int i=0; i < maxIterations; i++) {
		gx1 = quatSq(gx1) + c;
		gx2 = quatSq(gx2) + c;
		gy1 = quatSq(gy1) + c;
		gy2 = quatSq(gy2) + c;
		gz1 = quatSq(gz1) + c;
		gz2 = quatSq(gz2) + c;
	}

	gradX = length(gx2) - length(gx1);
	gradY = length(gy2) - length(gy1);
	gradZ = length(gz2) - length(gz1);

	N = normalize(vec3(gradX, gradY, gradZ));

	return N;
}

float intersectQJulia(inout vec3 r0, inout vec3 rD, vec4 c, int maxIterations, float epsilon)
{
	float dist;
	int i = 0;
	while (true) {
	//while (i<1000) {
		vec4 z = vec4(r0, 0.0);

		vec4 zp = vec4(1.0, 0.0, 0.0, 0.0);

		iterateIntersect(z, zp, c, maxIterations);

		float normZ = length(z);
		dist = 0.5 * normZ * log(normZ) / length(zp);

		r0 += rD * dist;	// (step)

		if (dist < epsilon || dot(r0, r0) > BOUNDING_RADIUS_2) {
			break;
		}
		++i;
	}

	return dist;
}

vec3 Phong(vec3 light, vec3 eye, vec3 pt, vec3 N)
{
	vec3 diffuse = vec3(1.00, 0.30, 0.35);	// base color
	const int specularExponent = 10;		// shininess od shading
	const float specularity = 0.9;			// amplitude of specular highlight

	vec3 L		= normalize(light - pt);
	vec3 E		= normalize(eye - pt);
	float NdotL	= dot(N, L);
	vec3 R		= L - 2 * NdotL * N;

	diffuse += abs(N)*0.3;

	return ( diffuse * max(NdotL, 0) + specularity * pow(max(dot(E, R), 0), specularExponent) );
}

int intersectSphere(inout vec3 r0, vec3 rD)
{
	float A, B, C, d, t0, t1, t;

	//A = dot(rD, rD);
	A = 1.0;
	B = 2.0 * dot(r0, rD);
	C = dot(r0, r0) - BOUNDING_RADIUS_2;
	d = B*B - 4*A*C;	// delta
	if (d <= 0) {
		return 1;
	} 
	d = sqrt(d);	
	t0 = (-B + d) * 0.5 / A;
	t1 = (-B - d) * 0.5 / A;
	t = min(t0, t1);
	r0 += t * rD;

	return 0;
}

// Define the ray direction from the pixel coordinates
vec3 rayDirection(vec2 p, float aspectRatio, mat3 viewRotation)
{
	vec3 direction = vec3( 2.0 * aspectRatio * p.x / float(size.x) - aspectRatio, 
								-2.0 * p.y / float(size.y) + 1.0, 
								-2.0);
	return direction * viewRotation;
}

void evaluateDependents(inout float aspectRatio, vec4 camera, inout mat3 viewRotation, inout vec3 eye, inout vec3 lightSource, vec3 light)
{
	aspectRatio = size.x / size.y;
		
	// Camera orientation
	float c1 = cos(radians(-camera.x));
	float s1 = sin(radians(-camera.x));

	mat3 viewRotationY = mat3(  c1, 0, s1, 
						 0,  1, 0, 
						-s1, 0, c1 );
		
    float c2 = cos(radians(-camera.y));
	float s2 = sin(radians(-camera.y));
        
    mat3 viewRotationZ = mat3( c2, -s2, 0,
                        s2,  c2, 0, 
                        0,   0,  1 );
        
    float c3 = cos(radians(-camera.z));
	float s3 = sin(radians(-camera.z));
        
    mat3 viewRotationX = mat3( 1, 0,   0,
                        0, c3, -s3, 
                        0, s3,  c3 );
        
        
    viewRotation = viewRotationX * viewRotationY * viewRotationZ;
        
	eye = vec3(0, 0, camera.w) * viewRotation;
    lightSource = light * viewRotation;
}

void main()
{


	//evaluateDependents(aspectRatio, camera, viewRotation, r0, lightSource, light);
	vec3 r0 = eye;
	vec3 rD = rayDirection(fragCoord * size, aspectRatio, viewRotation);

	vec4 color = backgroundColor;

	rD = normalize(rD);
	//intersectSphere(r0, rD);
	if (intersectSphere(r0, rD) != 0) {		// ray doesn't intersect the sphere
		vFragColor = vec4(1.0, 0.0, 0.0, 1.0);
		//vFragColor = backgroundColor;
		return;
	}
	
	float dist = intersectQJulia(r0, rD, mu, maxIterations, epsilon);

	if (dist < epsilon) {
		vec3 N = normEstimate(r0, mu, maxIterations);
		
		color.xyz = Phong(lightSource, rD, r0, N);
		color.w = 1.0;

		if (renderShadows == true) {
			vec3 L = normalize(lightSource - r0);
			r0 += N * epsilon * 2.0;
			dist = intersectQJulia(r0, L, mu, maxIterations, epsilon);

			if (dist < epsilon) {
				color.xyz *= 0.4;
			}
		}
		//color.x *= 1.5;
	}

	vFragColor = color;
}
