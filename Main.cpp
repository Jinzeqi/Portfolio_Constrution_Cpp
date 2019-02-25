#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <math.h>
#include "Model_Input.h"
#include "Model_Run.h"
#include "Model_Output.h"

using namespace std;

int main(){	

//******************************************* Read CSV file ; Get price matrix and Date vectors ***************************************************************
	// read file 
	string filename = "hist_prices.csv";
	ifstream in_file(filename);	
	if(in_file.fail()){
		cerr << "fail to open file "<< filename <<"\n";
	}
	// read the column name 
	string readline_colname;
	getline(in_file,readline_colname);
	stringstream ss_colname(readline_colname);
	// get the number of stocks
	string stock_name;
	int col_len = -1;
	while(getline(ss_colname, stock_name, ',')){
		++col_len;
	}		
	// store the stock prices into vectors
	vector<vector<double>> price_matrix(col_len);
	// store date string into vector
	vector<string> date;
	// store year and month data into two seperate vectors
	vector<int> year, month;
	
	string readline;
	int line_index = 0;
	int t_month,t_year;
	double price[col_len]; 
	// loop for every line
	while(getline(in_file, readline)){
		stringstream ss(readline);
		string temp_date, temp_day, temp_year, temp_month;
		// if data in this line is valid, store data into vector
		if(parser(ss,temp_date,price,col_len)){
			date.push_back(temp_date);
			for(int i = 0; i < col_len; ++i){
				price_matrix[i].push_back(price[i]);
			}
			// store year and month data into vectors
			stringstream ss_date(temp_date);
			getline(ss_date, temp_month, '/');
			stringstream ss_month(temp_month);
			ss_month >> t_month;
			month.push_back(t_month);
			getline(ss_date, temp_day, '/');	
			getline(ss_date, temp_year, '/');	
			stringstream ss_year(temp_year);
			ss_year >> 	t_year;
			year.push_back(t_year);
			// self-increment for the number of lines
			++line_index;			
		}
	}	
	in_file.close();	

//******************************************* Construct portfolio and rebalancing each month ***************************************************************
	// convert the price matrix into return matrix 
	vector<vector<double>> return_matrix = get_return_matrix(price_matrix);

	// Input the starting rebalaning year data
	int current_year = 2012;
	int current_month = 1;
	// define the vector for portfolio data in dailg basis 
	vector<double> total_equaldollar_port_weight, total_equalvol_port_weight, total_equaldollar_port_ret, total_equalvol_port_ret;
	vector<double> total_minvol_port_ret, total_minvol_port_weight;
	
	// get the number of total rebalancing periods (74)
	int total_reb_month = (year.back() - current_year) * 12 + (month.back() - current_month + 1);

	// get the rebalancing weight
	for(int k = 0; k < total_reb_month; ++k){
		// generate the last year date index for calculation of covariance matrix
		vector<int> date_index_last = find_date_index(year,month,current_year,current_month);
		// generate the year date index for back-test calculation
		vector<int> date_index_now = find_date_index_month(year,month,current_year,current_month);		
		// generate the return matrix and price vector		
		vector<vector<double>> return_matrix_last(col_len);
		vector<vector<double>> return_matrix_now(col_len);		
		vector<double> temp_price_vector;		
		for(int i = 0; i < col_len; ++i){
			vector<double> temp_ret_vec = return_matrix[i];
			vector<double> temp_vec_pri = price_matrix[i];
			vector<double>::iterator it_s_ret,it_e_ret,it_s_ret_now,it_e_ret_now;
			it_s_ret = (i == 0) ? (temp_ret_vec.begin() + date_index_last[0]) : (temp_ret_vec.begin() + date_index_last[0] - 1);
			it_e_ret = temp_ret_vec.begin() + date_index_last[1] - 1;
			it_s_ret_now = temp_ret_vec.begin() + date_index_now[0] - 1;
			it_e_ret_now = temp_ret_vec.begin() + date_index_now[1];
			return_matrix_last[i].assign(it_s_ret,it_e_ret);
			return_matrix_now[i].assign(it_s_ret_now,it_e_ret_now);
			temp_price_vector.push_back(temp_vec_pri[date_index_last[1]]);
		}
		// get the exponential weight with specific expected life 
		double lambda = (double)9 / 12;
		int len = date_index_last[1] - date_index_last[0] + 1;
		vector<double> exp_weight =  get_exp_weight(lambda, len);		
		// calculate the volatility and correlation matrix
		// get the covariance matrix
		vector<vector<double>> cov_mat = get_cov_mat(return_matrix_last,exp_weight);	
		// calculate the standard deviation
		vector<double> volatility;
		for(int i = 0; i < col_len; ++i){
			volatility.push_back(sqrt(cov_mat[i][i]));
		}
		// calculate the correlation matrix
		vector<vector<double>> cor_mat(col_len); 	
		for(int i = 0; i < col_len; ++i){
			for (int j = 0; j < col_len; ++j){
				cor_mat[i].push_back(cov_mat[i][j] / (volatility[i] * volatility[j]));
			}
		}
		// calculate the portfolio weights
		vector<double> equaldollar_port_weight, equalvol_port_weight, minvol_port_weight;
		// calculate the current period weight
		equaldollar_port_weight = get_equaldollar_port_weight(temp_price_vector);
		equalvol_port_weight = get_equalvol_port_weight(volatility);
		minvol_port_weight = get_minvol_port_weight(cov_mat);
		// update the total weights
		total_equaldollar_port_weight.insert(total_equaldollar_port_weight.end(), equaldollar_port_weight.begin(), equaldollar_port_weight.end());
		total_equalvol_port_weight.insert(total_equalvol_port_weight.end(), equalvol_port_weight.begin(), equalvol_port_weight.end());	
		total_minvol_port_weight.insert(total_minvol_port_weight.end(), minvol_port_weight.begin(), minvol_port_weight.end());					
		// calculate the portfolio return
		vector<double> equaldollar_port_ret, equalvol_port_ret, minvol_port_ret;
		// calculate the current period return
		equaldollar_port_ret = get_port_ret(equaldollar_port_weight,return_matrix_now);
		equalvol_port_ret = get_port_ret(equalvol_port_weight,return_matrix_now);
		minvol_port_ret = get_port_ret(minvol_port_weight,return_matrix_now);		
		// update the total return		
		total_equaldollar_port_ret.insert(total_equaldollar_port_ret.end(), equaldollar_port_ret.begin(), equaldollar_port_ret.end());		
		total_equalvol_port_ret.insert(total_equalvol_port_ret.end(), equalvol_port_ret.begin(), equalvol_port_ret.end());	
		total_minvol_port_ret.insert(total_minvol_port_ret.end(), minvol_port_ret.begin(), minvol_port_ret.end());					

		// update the current year and month => move to next month
		current_year = (current_month == 12) ? (current_year + 1) : current_year;
		current_month = (current_month == 12) ? 1: (current_month + 1);		 
	}

//******************************************* Calculate stats for existing portfolio ***************************************************************
	// get the total length for back-test period on daily basis (1537)
	int total_ret_len = total_equaldollar_port_ret.size(); 
	// get the portfolio value from day1 
	vector<double> equaldollar_port_ret_value = get_ret_value(total_equaldollar_port_ret);
	vector<double> equalvol_port_ret_value = get_ret_value(total_equalvol_port_ret);
	vector<double> minvol_port_ret_value = get_ret_value(total_minvol_port_ret);	
	// calculate average year return, use componded rate
	double avg_ret_equaldollar = get_avg_year_ret(equaldollar_port_ret_value.back(), total_ret_len);
	double avg_ret_equalvol = get_avg_year_ret(equalvol_port_ret_value.back(), total_ret_len);
	double avg_ret_minvol = get_avg_year_ret(minvol_port_ret_value.back(), total_ret_len);
	vector<double> avg_year_ret({avg_ret_equaldollar, avg_ret_equalvol, avg_ret_minvol});
	// calculate sharpe ratio for yearly basis
	double sharpe_equaldollar = get_sharpe_rat(avg_ret_equaldollar,total_equaldollar_port_ret);
	double sharpe_equalvol = get_sharpe_rat(avg_ret_equalvol,total_equalvol_port_ret);
	double sharpe_minvol = get_sharpe_rat(avg_ret_minvol,total_minvol_port_ret);
	vector<double> sharpe_rat({sharpe_equaldollar, sharpe_equalvol, sharpe_minvol});
	// calculate max draw down amount, in percentage
	vector<double> mdd_equaldollar = get_max_draw_down(equaldollar_port_ret_value);
	vector<double> mdd_equalvol = get_max_draw_down(equalvol_port_ret_value);
	vector<double> mdd_minvol = get_max_draw_down(minvol_port_ret_value);	
	vector<double> max_draw_down({mdd_equaldollar[0], mdd_equalvol[0], mdd_minvol[0]});
	// calculate max draw down period, in days		
	vector<double> max_draw_down_period({mdd_equaldollar[1], mdd_equalvol[1], mdd_minvol[1]});
	// calculate max draw down start date index 
	vector<double> max_draw_down_start_date({mdd_equaldollar[2], mdd_equalvol[2], mdd_minvol[2]});
	// calculate max draw down end date index 
	vector<double> max_draw_down_end_date({mdd_equaldollar[3], mdd_equalvol[3], mdd_minvol[3]});
	
//******************************************* Exporting Result to CSV files ***************************************************************
	// output the portfolio daily return series to a csv file 
	// get the starting index of date
	current_year = 2012;
	current_month = 1;
	int output_date_index = find_date_index_month(year, month, current_year, current_month)[0];
	ofstream out_file("HW3_port_return.csv");
	if(!out_file.is_open()){
		cerr << "fail to open HW3_port_return.csv for writin \n";
	}	
	out_file << " Date" << ',' << "Equal-Dollar-Port" << ',' << "Equal-Volatility-Port" << ',' << "Min-Variance-Port" << endl;
	for(int i = 0; i < total_ret_len; ++i){
		out_file << date[output_date_index + i] << ',' << total_equaldollar_port_ret[i] << ',' << total_equalvol_port_ret[i] << ',' << total_minvol_port_ret[i] << endl;
	}	
	out_file.close();

	// output the portfolio daily value to a csv file 
	ofstream out_file2("HW3_port_value.csv");
	if(!out_file2.is_open()){
		cerr << "fail to open HW3_port_value.csv for writin \n";
	}	
	out_file2 << " Date" << ',' << "Equal-Dollar-Port" << ',' << "Equal-Volatility-Port" << ',' << "Min-Variance-Port" << endl;
	for(int i = 0; i < total_ret_len; ++i){
		out_file2 << date[output_date_index + i] << ',' << equaldollar_port_ret_value[i] << ',' << equalvol_port_ret_value[i] << ',' << minvol_port_ret_value[i] << endl;
	}	
	out_file2.close();
		
	// output the portfolio stats result to a csv file 
	ofstream out_file1("HW3_port_stats.csv");
	if(!out_file1.is_open()){
		cerr << "fail to open HW3_port_stats.csv for writin \n";
	}
	out_file1 <<" " << ',' << "Equal-Dollar-Port" << ',' << "Equal-Volatility-Port" << ',' << "Min-Variance-Port" << endl;
	out_file1 <<" Avg_Return " << ',' << avg_ret_equaldollar << ',' << avg_ret_equalvol << ',' << avg_ret_minvol << endl;
	out_file1 <<" Sharpe Ratio " << ',' << sharpe_equaldollar << ',' << sharpe_equalvol << ',' << sharpe_minvol << endl;
	out_file1 <<" Max drawdown" << ',' << max_draw_down[0] << ',' << max_draw_down[1] << ',' << max_draw_down[2] << endl;
	out_file1 <<" Max drawdown Period " << ',' << max_draw_down_period[0] << ',' << max_draw_down_period[1] << ',' << max_draw_down_period[2] << endl;
	out_file1 <<" Max drawdown Date " << ',' << date[output_date_index + max_draw_down_start_date[0]] << "-" << date[output_date_index + max_draw_down_end_date[0]] << ',' << date[output_date_index + max_draw_down_start_date[1]] << "-" << date[output_date_index + max_draw_down_end_date[1]] << ',' << date[output_date_index + max_draw_down_start_date[2]] << "-" << date[output_date_index + max_draw_down_end_date[2]] << endl;
	out_file1.close();
	
	return 0;	
}
