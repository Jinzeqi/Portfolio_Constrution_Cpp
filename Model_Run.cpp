#include "Model_Run.h"
using namespace std;

vector<double> get_equaldollar_port_weight(vector<double> price_vec){
	int n = price_vec.size();
	vector<double> weight(n, (double)1 / n);
	return weight;
}

vector<double> get_equalvol_port_weight(vector<double> vol_vec){
	vector<double> weight;
	double inv_vol_sum = 0;
	for(int i = 0; i < vol_vec.size(); ++i){
		inv_vol_sum += (double)1 / vol_vec[i];
	}	
	for(int i = 0; i < vol_vec.size(); ++i){
		weight.push_back((double)1 / (inv_vol_sum * vol_vec[i]));
	}
	return weight;
}
	
vector<double> get_minvol_port_weight(vector<vector<double>> cov_mat){
	// get the inverse covariance matrix
	vector<vector<double>> inv_cov = inverse(cov_mat);
	vector<double> weight;
	int n = cov_mat.size();
	// get the sum of all elements in the inverse matrix
	double sum_inverse = 0;
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < n; ++j){
			sum_inverse += 	inv_cov[i][j];
		}
	}		 
	for(int i = 0; i < n; ++i){
		double sum_row = 0;
		for(int j = 0; j < n; ++j){
			sum_row += inv_cov[i][j];
		}
		weight.push_back((double)sum_row / sum_inverse);	
	}	
	return weight;
}

