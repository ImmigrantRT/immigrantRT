#include "model.hpp"

Matrix CreateAugmentedMatrix(Data& data, int k) {
	double tmp1[2 * k + 1] = {};	// to store n, sum xi, sum xi^2, ..., sum xi^2k
	double tmp2[k + 1] = {};		// to store sum yi, sum yi*xi, sum yi*xi^2, ..., sum yi*xi^k

	for (int i = 0; i < data.size(); i++) {
		double x = data[i].first;
		double y = data[i].second;

		double x_pow = 1;

		for (int j = 0; j <= 2 * k; j++) {
			tmp1[j] += x_pow;
			if (j <= k) {
				tmp2[j] += y * x_pow;
			}
			x_pow *= x;
		}
	}

	Matrix augmented_matrix(k + 1, std::vector<double>(k + 2, 0));
	for (int i = 0; i <= k; i++) {
		for (int j = 0; j <= k; j++) {
			augmented_matrix[i][j] = tmp1[i + j];
		}
	}
	for (int i = 0; i <= k; i++) {
		augmented_matrix[i][k + 1] = tmp2[i]; 
	}

	return augmented_matrix;
}

std::vector<double> GaussJordanElimination(Matrix& augmented_matrix) {
	int n = augmented_matrix.size(); // number of equations

	for (int i = 0; i < n; i++) {
		// pivoting
		int max_row = i;
		for (int j = i + 1; j < n; j++) {
			if (abs(augmented_matrix[j][i]) < abs(augmented_matrix[max_row][i])) {
				max_row = j;
			}
		}
		if (max_row != i) {
			SwapRows(augmented_matrix, i, max_row);
		}

		// normalize pivot row
		ScaleRow(augmented_matrix, i, 1.0 / augmented_matrix[i][i]);

		// eliminate entries below pivot
		for (int j = i + 1; j < n; j++) {
			if (augmented_matrix[j][i] != 0) {
				SubtractRows(augmented_matrix, j, i, augmented_matrix[j][i]);
			}
		}
	}

	// eliminate above pivots
	for (int i = n - 1; i >= 0; i--) {
		for (int j = i - 1; j >= 0; j--) {
			if (augmented_matrix[j][i] != 0) {
				SubtractRows(augmented_matrix, j, i, augmented_matrix[j][i]);
			}
		}
	}

	/*
		at this point, the augmented matrix is in reduced row-echelon form
		==> solution = last column of the augmented matrix
	*/ 

	std::vector<double> solution;
	for (int i = 0; i < n; i++) {
		solution.push_back(augmented_matrix[i][n]);
	}

	return solution;
}

std::vector<double> Fit(Data& data, int degree) {
	// fit a k-degree polynomial on a dataset with n samples

	Matrix augmented_matrix = CreateAugmentedMatrix(data, degree);
	
	std::vector<double> coeffs = GaussJordanElimination(augmented_matrix);
	return coeffs;
}

double Predict(Data data, std::vector<double> coeffs, int t) {
	// predict data value t secs after last reading

	int t_pred = data[data.size() - 1].first + t;

	double x_pred = 0;
	for (int i = 0; i < coeffs.size(); i++) {
		x_pred += coeffs[i] * pow(t_pred, i);
	}

	return x_pred;
}