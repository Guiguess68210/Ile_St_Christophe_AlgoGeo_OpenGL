#pragma once
#ifndef POINT_H
#define POINT_H

#include <vector>
#include <array>

using namespace std;

class Point
{
public:
	// constructeurs
	Point() : d_x{ 0 }, d_y{ 0 }, d_z{ 0 }, d_r{ 0 }, d_g{ 0 }, d_b{ 0 } {}
	Point(float x, float y, float z) : d_x{ x }, d_y{ y }, d_z{ z } {}
	Point(float x, float y, float z, float r, float g, float b) : d_x{ x }, d_y{ y }, d_z{ z }, d_r{ r }, d_g{ g }, d_b{ b } {}
	// accesseurs
	float x() const { return d_x; }
	float y() const { return d_y; }
	float z() const { return d_z; }
	float r() const { return d_r; }
	float g() const { return d_g; }
	float b() const { return d_b; }

	// renvoie 1, -1 ou 0 selon que le point auquel la m�thode est appliqu�e est
	// � gauche de, � droite de, ou sur la droite (ab) orient�e de a vers b.
	int aGauche(const Point& a, const Point& b) const
	{
		int determinant = (b.x() - a.x()) * (this->y() - a.y()) - (this->x() - a.x()) * (b.y() - a.y());
		if (determinant > 0)
		{
			return 1;
		}
		else if (determinant < 0)
		{
			return -1;
		}
		else {
			return 0;
		}
	};

	int dansCercle(const Point& a, const Point& b, const Point& c) const
	{
		double A = a.x() - this->x();
		double B = a.y() - this->y();
		double C = pow(a.x() - this->x(), 2) + pow(a.y() - this->y(), 2);

		double D = b.x() - this->x();
		double E = b.y() - this->y();
		double F = pow(b.x() - this->x(), 2) + pow(b.y() - this->y(), 2);

		double G = c.x() - this->x();
		double H = c.y() - this->y();
		double I = pow(c.x() - this->x(), 2) + pow(c.y() - this->y(), 2);

		double determinant = (A * E * I) + (D * H * C) + (B * F * G) - (((G * E * C) + (D * B * I) + (A * H * F)));
		if (determinant > 0) {
			return 1;
		}
		else if (determinant == 0)
		{
			return 0;
		}
		else {
			return -1;
		}
	};

	// accesseurs
	array<float, 6> getVertice() const
	{
		array<float, 6> vertice;

		vertice =
		{
			x(), y(), z(),
			r(), g(), b()
		};

		return vertice;
	};

private:
	// coordonn�es
	float d_x, d_y, d_z, d_r, d_g, d_b;
};

#endif