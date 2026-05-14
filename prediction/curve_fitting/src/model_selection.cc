#include "model_selection.hpp"

std::vector<double> GetPredictions(Data data, int degree) {
    // fit a curve of a specific degree and use it for predicting

	data.resize(0.9 * data.size()); // 90% of samples
	
	std::vector<double> coeffs = Fit(data, degree);

	std::vector<double> preds;
	preds.push_back(std::max(Predict(data, coeffs, 1), 0.0));
	preds.push_back(std::max(Predict(data, coeffs, 2), 0.0));

	return preds;
}

std::vector<double> FindBestFitCurve(Data data, int n_preds) {
	// data has 20 samples; last 2 samples (10%) used for testing
	double x1 = data[18].second;
	double x2 = data[19].second;

	double rmse_min = std::numeric_limits<double>::max();
	double mape = 0; // mean absolute percentage error
    int best_degree = 0;

	for (int i = 1; i <= 10; i++) {
		std::vector<double> p = GetPredictions(data, i);

		double err1 = p[0] - x1;
		double err2 = p[1] - x2;
		double rmse = sqrt((pow(err1, 2) + pow(err2, 2)) / 2.0);

		if (rmse < rmse_min) {
			rmse_min = rmse;
			mape = ((fabs(err1) / x1) + (fabs(err2) / x2)) * 100 / 2.0;
			best_degree = i;
		}
	}

	std::vector<double> coeffs = Fit(data, best_degree);
	// PrintResults(coeffs, best_degree, rmse_min, mape);

    return coeffs;
}
