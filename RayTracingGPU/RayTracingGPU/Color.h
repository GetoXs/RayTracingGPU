#pragma once
class Color 
{
private:
	float _r, _g, _b, _a;
public:
#pragma region Inne metody
	void Clamp(float min, float max)
	{
		//R
		if (this->redF() < min) this->setRedF(min);
		if (this->redF() > max) this->setRedF(max);
		//G
		if (this->greenF() < min) this->setGreenF(min);
		if (this->greenF() > max) this->setGreenF(max);
		//B
		if (this->blueF() < min) this->setBlueF(min);
		if (this->blueF() > max) this->setBlueF(max);
	}
	void Clamp()
	{
		this->Clamp(0.f, 1.f);
	}
#pragma endregion

#pragma region Operatory
	Color operator*(const double &rate) const
	{
		return Color(
			this->redF() * rate, 
			this->greenF() * rate, 
			this->blueF() * rate,
			this->alphaF() * rate);
	}
	Color operator*(const Color &color) const
	{
		return Color(
			this->redF() * color.redF(), 
			this->greenF() * color.greenF(), 
			this->blueF() * color.blueF(),
			this->alphaF() * color.alphaF());
	}
	Color operator+(const Color &color) const
	{
		return Color(
			this->redF() + color.redF(), 
			this->greenF() + color.greenF(),
			this->blueF() + color.blueF(),
			this->alphaF() + color.alphaF());
	}
	Color &operator+=(const Color &color)
	{
		this->setRedF(this->redF() + color.redF());
		this->setGreenF(this->greenF() + color.greenF());
		this->setBlueF(this->blueF() + color.blueF());
		this->setAlphaF(this->alphaF() + color.alphaF());
		return *this;
	}
#pragma endregion

#pragma region Gety
	float redF() const
	{
		return this->_r;
	}
	float greenF() const
	{
		return this->_g;
	}
	float blueF() const
	{
		return this->_b;
	}
	float alphaF() const
	{
		return this->_a;
	}

	float *GetValue()
	{
		float *col = new float[4];
		col[0] = (float)this->redF();
		col[1] = (float)this->greenF();
		col[2] = (float)this->blueF();
		col[3] = (float)this->alphaF();
		return col;
	}
#pragma endregion

#pragma region Sety
	void setRedF(float r)
	{
		this->_r = r;
	}
	void setGreenF(float g)
	{
		this->_g = g;
	}
	void setBlueF(float b)
	{
		this->_b = b;
	}
	void setAlphaF(float a)
	{
		this->_a = a;
	}
#pragma endregion

#pragma region Ctors
	Color(float alpha)
		:Color(0, 0, 0, alpha)
	{
	}
	Color(double r, double g, double b)
		:Color(r, g, b, 1.)
	{
	}
	Color(double r, double g, double b, double a) 
	{
		this->setRedF(r);
		this->setGreenF(g);
		this->setBlueF(b);
		this->setAlphaF(a);
	}
#pragma endregion
};

