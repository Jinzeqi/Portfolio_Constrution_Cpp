#include "Model_Input.h"
using namespace std;

bool parser(stringstream& ss, string& date, double* data, int size){
	// get the date
	getline(ss,date,',');
	// get the price data, size is the stocks number
	for(int i = 0; i < size; ++i){
		stringstream temp_ss_data;
		string temp_data;
		// store data into array
		if(getline(ss, temp_data, ',')){
			temp_ss_data << temp_data;
			temp_ss_data >> data[i];
		}
		// if there is unstructural data, skip that line
		else{
			return(false);			
		}
	}
}

// get the return matrix index for the last whole year according to current date
// the input parameter is the year and month vector and the current year and month
// the return result is the starting index and the ending index 
vector<int> find_date_index(vector<int> year, vector<int> month, int& c_year, int& c_month){
	// the year and month we should start and end from
	int s_year, e_year, s_month, e_month;
	e_year = c_year;
	s_year = c_year - 1;
	s_month = c_month;
	e_month = c_month;	
	// get the index for start and end
	vector<int> date_index_last;
	int flag = 0;
	for(int i = 0; i < year.size(); ++i){
		// get the first day of the starting month
		if((year[i] == s_year) && (month[i] == s_month) && (flag == 0)){
			date_index_last.push_back(i);
			flag = 1;
		}
		// get the last day of the ending month
		if((year[i] == e_year) && (month[i] == e_month)){
			date_index_last.push_back(i-1);
			break;
		}
	}
	return date_index_last;
}

vector<int> find_date_index_month(vector<int> year, vector<int> month, int& c_year, int& c_month){	
	// get the index for start and end
	vector<int> date_index_now;
	int count = 0;
	for(int i = 0; i < year.size(); ++i){
		// get the first day of the starting month
		if((year[i] == c_year) && (month[i] == c_month)){
			if(count == 0){
				date_index_now.push_back(i);	
			}
			count += 1;
		}
	}
	date_index_now.push_back(date_index_now[0] + count - 1);	
	return date_index_now;
}

vector<double> get_exp_weight(double& lambda, int& len){
	vector<double> exp_weight;
	for(int i = 0; i < len; ++i){
		exp_weight.push_back((1 - lambda) * pow(lambda, i));	
	}
	return exp_weight;
}

double get_vec_mean(vector<double> vec){
	
	double vec_sum = 0;
	for(int i = 0; i < vec.size(); ++i){
		vec_sum += vec[i];
	}
	return (vec_sum / vec.size());
}

double get_vec_std(vector<double> vec){
	
	double vec_pro = 0;
	double vec_mean = get_vec_mean(vec);
	for(int i = 0; i < vec.size(); ++i){
		vec_pro += (vec[i] - vec_mean) * (vec[i] - vec_mean);
	}
	return (sqrt(vec_pro / vec.size()));	
}

// convert price matrix into return matrix
vector<vector<double>> get_return_matrix(vector<vector<double>> price_matrix){
	int col_len = price_matrix.size();
	int line_index = price_matrix[0].size();
	vector<vector<double>> return_matrix(col_len);	
	for(int i = 0; i < col_len; ++i){
		for (int j = 0; j < line_index-1; ++j){
			return_matrix[i].push_back(log((float)price_matrix[i][j+1] / price_matrix[i][j]));
		}
	}
	return return_matrix;
}	

vector<vector<double>> get_cov_mat(vector<vector<double>> ret_mat, vector<double> weight){
	// each row is one series of stock return
	int row_len = ret_mat.size();
	// each column is one slice of all stocks return in one day
	int col_len = ret_mat[0].size();
	// calculate the covariance matrix
	vector<vector<double>> cov_mat(row_len);
	double temp_cov;
	for(int i = 0; i < row_len; ++i){
		double vec_mean_i = get_vec_mean(ret_mat[i]);
		for (int j = 0; j < row_len; ++j){
			double vec_mean_j = get_vec_mean(ret_mat[j]);	
			temp_cov = 0;
			for (int k = 0; k < col_len; ++k){
				temp_cov += (ret_mat[i][k] - vec_mean_i) * (ret_mat[j][k] - vec_mean_j) * weight[k];
				
			}
			cov_mat[i].push_back(temp_cov);
		}
	}
	return cov_mat;
}
