#ifndef Model_Input.h
#define Model_Input.h
#include <sstream>
#include <fstream>
#include <vector>
#include <math.h> 
using namespace std;

bool parser(stringstream&, string&, double*, int);

vector<int> find_date_index(vector<int>, vector<int>, int&, int&);

vector<int> find_date_index_month(vector<int>, vector<int>, int&, int&);

vector<double> get_exp_weight(double&, int&);

double get_vec_mean(vector<double>);

double get_vec_std(vector<double>);

vector<vector<double>> get_return_matrix(vector<vector<double>>);

vector<vector<double>> get_cov_mat(vector<vector<double>>, vector<double>);
		
#endif
