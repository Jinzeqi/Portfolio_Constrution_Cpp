#ifndef Model_Run.h
#define Model_Run.h

#include <vector>
#include <iostream>
#include "Model_Input.h"
#include "Inverse_Matrix.h"

using namespace std;

vector<double> get_equaldollar_port_weight(vector<double>);

vector<double> get_equalvol_port_weight(vector<double>);

vector<double> get_minvol_port_weight(vector<vector<double>>);

#endif
