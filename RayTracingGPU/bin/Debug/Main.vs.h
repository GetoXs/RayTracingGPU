varying vec3 pixel;

void main(void)
{
	// Keep the original positions of the vertices of the quad by
	// applying the projection matrix only. It describes an orthographic
	// projection so the quad will get expanded to the whole screen.
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	// Save the positions of all four vertices of the quad. However,
	// this is a vertex shader and "p" is a varying variable. That
	// means, this position will get interpolated over the quad. So we
	// get a point on the viewing plane -- for each pixel.
	pixel = gl_Vertex.xyz;
}