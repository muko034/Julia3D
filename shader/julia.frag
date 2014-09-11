#version 400
precision highp float;

#define BOUNDING_RADIUS_2	2.0
#define ESCAPE_THRESHOLD	1e1
#define DEL					1e-4
#define EPSILON				1e-3

// ZMIENNE TYPU UNIFORM
uniform vec3 u_rO;				
uniform vec4 u_q;
uniform float u_slice;			
uniform int u_maxIterations;

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

void iterateSequence(inout vec4 z, inout vec4 z_p)
{
	for (int i=0; i < u_maxIterations; i++) {
		z_p = 2.0 * quatMult(z, z_p);
		z = quatSq(z) + u_q;

		if (dot(z, z) > ESCAPE_THRESHOLD) {
			break;
		}
	}
}

vec3 approximateNorm(vec3 p)
{
	vec3 N;
	vec4 qP = vec4(p, u_slice);
	float gradX, gradY, gradZ;

	vec4 gx1 = qP - vec4(DEL, 0.0, 0.0, 0.0);
	vec4 gx2 = qP + vec4(DEL, 0.0, 0.0, 0.0);
	vec4 gy1 = qP - vec4(0.0, DEL, 0.0, 0.0);
	vec4 gy2 = qP + vec4(0.0, DEL, 0.0, 0.0);
	vec4 gz1 = qP - vec4(0.0, 0.0, DEL, 0.0);
	vec4 gz2 = qP + vec4(0.0, 0.0, DEL, 0.0);

	for (int i=0; i < u_maxIterations; i++) {
		gx1 = quatSq(gx1) + u_q;
		gx2 = quatSq(gx2) + u_q;
		gy1 = quatSq(gy1) + u_q;
		gy2 = quatSq(gy2) + u_q;
		gz1 = quatSq(gz1) + u_q;
		gz2 = quatSq(gz2) + u_q;
	}

	gradX = length(gx2) - length(gx1);
	gradY = length(gy2) - length(gy1);
	gradZ = length(gz2) - length(gz1);

	N = normalize(vec3(gradX, gradY, gradZ));

	return N;
}

bool rayMarching(inout vec3 p, vec3 rD)
{
	float dist;
	bool foundIntersection = false;
	while (true) {
		vec4 z = vec4(p, u_slice);	// czwarta wspolrzedna kwaternionu stala - okresla przekroj zbioru Julii 4D
		vec4 zp = vec4(1.0, 0.0, 0.0, 0.0);

		iterateSequence(z, zp);
	
		float normZ = length(z);
		dist = 0.5 * normZ * log(normZ) / length(zp);	// oszacowanie odleglosci od zbioru Julii

		p += rD * dist;	// krok wzd³u¿ promienia

		if (dist < EPSILON) {	
			foundIntersection = true;	// znaleziono przeciecie promienia ze zbiorem
			break;
		} 
		if (dot(p, p) > BOUNDING_RADIUS_2) { // aktualnie przetwarzany punkt poza sfera otaczajaca
			foundIntersection = false;	// nie znaleziono przeciecie promienia ze zbiorem
			break;
		} 
	}

	return foundIntersection;
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

int boundingSphere(inout vec3 p, vec3 rD)
{
	float A, B, C, d, t0, t1, t;

	//A = dot(rD, rD);
	A = 1.0;
	B = 2.0 * dot(p, rD);
	C = dot(p, p) - BOUNDING_RADIUS_2;
	d = B*B - 4*A*C;
	if (d < 0) {
		return 1;
	} 
	d = sqrt(d);	// delta
	t0 = (-B + d) * 0.5;
	t1 = (-B - d) * 0.5;
	//t1 /= A;
	//t0 /= A;
	t = min(t0, t1);
	p += t * rD;

	return 0;
}

void main()
{
	vec3 rO						= u_rO;			
	const vec3 rD				= normalize(vec3(fragCoord, -1));
					
	const vec3 light			= vec3(-0.3, 0.0, 1.0);
	const bool renderShadows	= true;

	vec4 color;

	if (boundingSphere(rO, rD) != 0) {		// ray doesn't intersect the sphere
		//vFragColor = vec4(1.0, 0.0, 0.0, 1.0);
		//return;
		discard;		
	}

	bool foundIntersection = rayMarching(rO, rD);

	if (foundIntersection) {
		vec3 N = approximateNorm(rO);
		
		color.xyz = Phong(light, rD, rO, N);
		color.w = 1.0;

		if (false/*renderShadows == true*/) {
			vec3 L = normalize(light - rO);
			rO += N * EPSILON * 2.0;
			foundIntersection = rayMarching(rO, L);
			if (foundIntersection) {
				color.xyz *= 0.4;
			}
		}
		//color.x *= 1.5;
	} else {
		discard;
	}

	vFragColor = color;
}
