#include "Inverse_Matrix.h"

using namespace std;

double det(vector<double> matrix){
	int n = sqrt(matrix.size());
    double result = 0;
    //calculation for the smallest matrix
    if(n==2){
        return result = matrix[0] * matrix[3] - matrix[1] * matrix[2];
    }
    //other conditions
    else{
        //construct a new matrix
        vector<double> matrix_elements((n-1) * (n-1),-1);
        for(int i=0;i<n;++i){            
            //select the element
            double element = matrix[i];
            int index=0;
            
            //generate the elements inside minor matrix
            for(int j = 1; j < n; ++j){
                for(int k = 0; k < n; ++k){
                    if(k != i){
                        matrix_elements[index]=matrix[j * n + k];
                        ++index;
                    }
                }
            }            
            //generate results using recursive functions
            result += pow(-1, i) * element * det(matrix_elements);
        }        
    }
    return result;	
}

vector<vector<double>> inverse(vector<vector<double>> matrix){
	// convert matrix into vector
	vector<double> mat_vec;
	int n = matrix.size();
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < n; ++j)
		mat_vec.push_back(matrix[i][j]);
	}
	// calculate the determinant
	double abs_determinant = fabs(det(mat_vec));
	vector<vector<double>> result(n);
	// generate the Cofactors
	for(int p = 0; p < n; ++p){
	    for(int i = 0; i < n; ++i){            
		    //generate the elements inside minor matrix
		    int index = 0;
		    vector<double> matrix_elements((n-1) * (n-1),-1);
		    double sub_determinant;
		    for(int j = 0; j < n; ++j){
		        for(int k = 0; k < n; ++k){
		            if((k != p) && (j != i)){
		                matrix_elements[index]=matrix[j][k];
		                ++index;
		            }
		        }
		    }
			// calculate the determinant for each sub matrix
			sub_determinant = det(matrix_elements);
			result[p].push_back(double(sub_determinant) / abs_determinant);
		}
	}
	return result;	
}
