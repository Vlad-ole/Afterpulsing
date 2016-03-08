#pragma once
class Functions
{
public:
	Functions();
	~Functions();

	static double *x_talk_factor(double p, double PDE);
	static double afp_factor(double p_s, double p_f, double a_s, double a_f);
};

