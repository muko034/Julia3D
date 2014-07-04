#version 400

#define BOUNDING_RADIUS_2	3.0
#define ESCAPE_THRESHOLD	1e1
#define DEL					1e-4

//uniform vec3 r0;				// TEXCOORD0 ray origin
//uniform vec3 rD;				// TEXCOORD1 ray direction (unit length)

//uniform vec4 mu;				// quaternion constant specifying the particular set (C)
//uniform float epsilon;			//
//uniform vec3 eye;
//uniform vec3 light;
//uniform bool renderShadows;
//uniform int maxIterations;

in vec2 fragCoord;

out vec4 vFragColor;

vec4 quatMult(vec4 q1, vec4 q2)
{
	vec4 r;

	r.x = q1.x * q2.x - dot(q1.yzw, q2.yzw);
	r.yzw = q1.x * q2.yzw + q2.x * q1.yzw + cross(q1.yzw, q2.yzw);

	return r;
}

vec4 quatSq(vec4 q)
{
	vec4 r;

	r.x = q.x * q.x - dot(q.yzw, q.yzw);
	r.yzw = 2 * q.x * q.yzw;

	return r;
}

void iterateIntersect(inout vec4 q, inout vec4 qp, vec4 c, int maxIterations)
{
	for (int i=0; i < maxIterations; i++) {
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
	//while (true) {
	while (i<100) {
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
	vec3 diffuse = vec3(1.00, 0.45, 0.25);	// base color
	const int specularExponent = 10;		// shininess od shading
	const float specularity = 0.45;			// amplitude of specular highlight

	vec3 L		= normalize(light - pt);
	vec3 E		= normalize(eye - pt);
	float NdotL	= dot(N, L);
	vec3 R		= L - 2 * NdotL * N;

	diffuse += abs(N)*0.3;

	return ( diffuse * max(NdotL, 0) + specularity * pow(max(dot(E, R), 0), specularExponent) );
}

vec3 intersectSphere(vec3 r0, vec3 rD)
{
	float B, C, d, t0, t1, t;

	B = 2.0 * dot(r0, rD);
	C = dot(r0, r0) - BOUNDING_RADIUS_2;
	d = sqrt(B*B - 4*C);
	t0 = (-B + d) * 0.5;
	t1 = (-B - d) * 0.5;
	t = min(t0, t1);
	r0 += t * rD;

	return r0;
}

void main()
{
	vec3 r0						= vec3(0.5, 0.0, 2.0);			
	vec3 rD						= normalize(vec3(fragCoord, -1.0));

	const vec4 mu				= vec4(-0.291,-0.399,0.339,0.437);
	const float epsilon			= 1e-4;							
	const vec3 eye				= vec3(0.5, 0.5, 2.0);
	const vec3 light			= normalize(vec3(0.57703));
	const bool renderShadows	= false;
	const int maxIterations		= 10;

	const vec4 backgroundColor = vec4(0.3, 0.3, 0.3, 1.0);

	vec4 color = backgroundColor;

	rD = normalize(rD);
	r0 = intersectSphere(r0, rD);

	float dist = intersectQJulia(r0, rD, mu, maxIterations, epsilon);

	if (dist < epsilon) {
		vec3 N = normEstimate(r0, mu, maxIterations);
		
		color.xyz = Phong(light, rD, r0, N);
		color.w = 1.0;

		if (renderShadows == true) {
			vec3 L = normalize(light - r0);
			r0 += N * epsilon * 2.0;
			dist = intersectQJulia(r0, L, mu, maxIterations, epsilon);

			if (dist < epsilon) {
				color.xyz *= 0.4;
			}
		}
	}

	vFragColor = color;

	//if (fragCoord.x >= -0.5 && fragCoord.x <= 0.5) {
	//	vFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//} else {
	//	vFragColor = backgroundColor;
	//}
}
