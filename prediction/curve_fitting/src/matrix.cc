#include "matrix.hpp"

void SwapRows(Matrix& M, int r1, int r2) {
	// swap rows r1 & r2 of matrix M

	std::swap(M[r1], M[r2]);
}

void ScaleRow(Matrix& M, int r, double f) {
	// scale row r of matrix M by a factor f

	for (double& val : M[r]) {
		val *= f;
	}
}

void SubtractRows(Matrix& M, int r1, int r2, double f) {
	// r1 <-- r1 - r2 * f

	for (int i = 0; i < M[r1].size(); i++) {
		M[r1][i] -= M[r2][i] * f;
	}
}
