
void main(void)
{
	//Transformacje punktu do wsp�rz�dnych okna
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}