#ifndef Model_Output.h
#define Model_Output.h

#include <vector>
#include <math.h>
#include "Model_Input.h"

using namespace std;

vector<double> get_port_ret(vector<double>, vector<vector<double>>);

vector<double> get_ret_value(vector<double>);

double get_avg_year_ret(double, int);

double get_sharpe_rat(double, vector<double>);

vector<double> get_max_draw_down(vector<double>);


#endif
