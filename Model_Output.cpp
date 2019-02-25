#include "Model_Output.h"
using namespace std;

vector<double> get_port_ret(vector<double> weight, vector<vector<double>> ret){
	int stock_num = weight.size();
	int len = ret[0].size();
	vector<double> port_ret;
	for(int i = 0; i < len; ++i){
		double temp_port_ret = 0;
		for(int j = 0; j < stock_num; ++j){
			temp_port_ret += ret[j][i] * weight[j];
		}
		port_ret.push_back(temp_port_ret);
	}
	return port_ret;
}

vector<double> get_ret_value(vector<double> ret){
	int len = ret.size();
	vector<double> value({1 + ret[0]});
	for(int i = 1; i < len; ++i){
		value.push_back((1 + ret[i]) * value[i-1]);
	}
	return value;
}

double get_avg_year_ret(double value, int size){
	double avg_ret;
	avg_ret = pow(value, (double)252 / size) - 1;
	return avg_ret;	
}

double get_sharpe_rat(double ret_mean, vector<double> ret){
	double ret_std = get_vec_std(ret);
	return((ret_mean - 0.02) / (sqrt(252) * ret_std));
}

vector<double> get_max_draw_down(vector<double> value){
	double max_draw_down, down_percentage;
	int len = value.size();
	int low_index = 0;
	int peak_index = 0;
	int max_draw_down_period = 1;
	int li, pi;
	double low = value[0];
	double peak = value[0];
	for(int i = 1; i < len; ++i){
		if(value[i] > peak){
			peak = value[i];
			low = value[i];
			peak_index = i;
			low_index = i;
		}
		else if((value[i] < low) && ((peak - value[i]) > max_draw_down)) {
			low = value[i];
			low_index = i;
			max_draw_down = peak - low;
			max_draw_down_period = low_index - peak_index + 1;	
			li = i;
			pi = peak_index;
			down_percentage = max_draw_down / peak;
			}
		}
		vector<double> result({down_percentage,(double)(li - pi + 1),(double)pi, (double)li});
		return result;
}
