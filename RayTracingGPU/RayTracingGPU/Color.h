#pragma once
#include "qcolor.h"
class Color :
	public QColor
{
public:
    Color operator*(const double &rate)
	{
		return Color(
			this->redF() * rate, 
			this->greenF() * rate, 
			this->blueF() * rate);
	}
    Color operator*(const Color &color)
	{
		return Color(
			this->redF() * color.redF(), 
			this->greenF() * color.greenF(), 
			this->blueF() * color.blueF());
	}
    Color operator+(const Color &color)
	{
		return Color(
			this->redF() + color.redF(), 
			this->greenF() + color.greenF(),
			this->blueF() + color.blueF());
	}

	Color():QColor()
	{
	}
	Color(double r, double g, double b):QColor()
	{
		this->setRedF(r);
		this->setGreenF(g);
		this->setBlueF(b);
		this->setAlphaF(1.0);
	}
	Color(double r, double g, double b, double a) :QColor()
	{
		this->setRedF(r);
		this->setGreenF(g);
		this->setBlueF(b);
		this->setAlphaF(a);
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
};

