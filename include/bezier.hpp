#ifndef BEZIER_HPP
#define BEZIER_HPP

#include "Vector2f.h"
#include <algorithm>
#include <cmath>

extern const double eps;

#define sqr(a) ((a)*(a))
#define fmax(a, b) ((a)>(b)?(a):(b))
#define fmin(a, b) ((a)<(b)?(a):(b))

class BezierCurve2D {
public:
	double *dx, *dy, max, height, max2, r, num;
	int n;
	struct D{
		double t0, t1, width, y0, y1, width2;
	}data[20];

	// x(t) = \sum_{i=0}^n dx_i * t^i
	// y(t) = \sum_{i=0}^n dy_i * t^i
	/*
		初始化一条二维平面上的Bezier曲线
		v:		控制点坐标
		n:		控制点数量
		num:	Bezier曲线次数
		r:		微小扰动
	*/
	BezierCurve2D(Vector2f *v, int n, int num, double r = 0.365): num(num), n(n), r(r) {
		double *px = new double[n];
		double *py = new double[n];
		for (int i = 0; i < n; ++i){
			px[i] = v[i].x();
			py[i] = v[i].y();
		}
		dx = new double[n];
		dy = new double[n];
		assert(std::fabs(py[0]) <= eps);
		--n;
		// preproces
		for(int i = 0; i <= n; ++i)
		{
			dx[i] = px[0];
			dy[i] = py[0];
			for (int j = 0; j <= n - i; ++j)
			{
				px[j] = px[j + 1] - px[j];
				py[j] = py[j + 1] - py[j];
			}
		}
		double n_down = 1, fac = 1, nxt = n;
		for (int i = 0; i <= n; ++i, --nxt)
		{
			fac = fac * (i == 0 ? 1 : i);
			dx[i] = dx[i] * n_down / fac;
			dy[i] = dy[i] * n_down / fac;
			n_down *= nxt;
		}
		max = 0;
		double interval = 1. / (num - 1), c = 0;
		for (int cnt = 0; cnt <= num; c += interval, ++cnt)
		{
			data[cnt].width = 0;
			data[cnt].t0 = fmax(0., c - r);
			data[cnt].t1 = fmin(1., c + r);
			data[cnt].y0 = getValue(data[cnt].t0).y();
			data[cnt].y1 = getValue(data[cnt].t1).y();
			for (double t = data[cnt].t0; t <= data[cnt].t1; t += 0.00001)
			{
				Vector2f pos = getValue(t);
				if (data[cnt].width < pos.x())
					data[cnt].width = pos.x();
			}
			if (max < data[cnt].width)
				max = data[cnt].width;
			data[cnt].width += eps;
			data[cnt].width2 = sqr(data[cnt].width);
		}
		max += eps;
		max2 = max * max;
		height = getValue(1).y();
		delete[] px;
		delete[] py;
	}
	~BezierCurve2D(){
		delete[] dx;
		delete[] dy;
	}
	
	/*
		获得Bezier曲线在t的值
	*/
	Vector2f getValue(double t)
	{
		double ans_x = 0, ans_y = 0, t_pow = 1;
		for (int i = 0; i <= n; ++i)
		{
			ans_x += dx[i] * t_pow;
			ans_y += dy[i] * t_pow;
			t_pow *= t;
		}
		return Vector2f(ans_x, ans_y);
	}

	/*
		获得Bezier曲线在t的一阶导数
	*/
	Vector2f getTangent(double t)
	{
		double ans_x = 0, ans_y = 0, t_pow = 1;
		for(int i = 1; i <= n; ++i)
		{
			ans_x += dx[i] * i * t_pow;
			ans_y += dy[i] * i * t_pow;
			t_pow *= t;
		}
		return Vector2f(ans_x, ans_y);
	}

	/*
		获得Bezier曲线在t的二阶导数
	*/
	Vector2f getTangent2(double t)
	{
		double ans_x = 0, ans_y = 0, t_pow = 1;
		for(int i = 2; i <= n; ++i)
		{
			ans_x += dx[i] * i * (i - 1) * t_pow;
			ans_y += dy[i] * i * (i - 1) * t_pow;
			t_pow *= t;
		}
		return Vector2f(ans_x, ans_y);
	}
};

#endif