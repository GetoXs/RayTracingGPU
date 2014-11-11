
void main(void)
{
	//Transformacje punktu do wspó³rzêdnych okna
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}