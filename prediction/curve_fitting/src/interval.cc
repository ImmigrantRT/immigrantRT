#include "interval.hpp"

double ComputeResidualVariance(Data data, std::vector<double> coeffs) {
	double sum_of_sq_residuals = 0;
	for (int i = 0; i < data.size(); i++) {
		sum_of_sq_residuals += std::pow(data[i].second - Predict(data, coeffs, i - 19), 2);
	}

	return sum_of_sq_residuals / (data.size() - coeffs.size());
}

double ComputeLeverage(std::vector<double> t_vals, double t_new) {
	size_t n = t_vals.size();
	
	double t_mean = std::accumulate(t_vals.begin(), t_vals.end(), 0.0) / n;

	double denominator = 0;
	for (double t_val : t_vals) {
		denominator += std::pow(t_val - t_mean, 2);
	}

	return (1.0 / n) + std::pow(t_new - t_mean, 2) / denominator;

}

double GetCriticalValueTDistribution(size_t n, size_t p) {
	// args: n = number of samples, p = number of parameters

	double alpha = 0.05; // 95% prediction level

	boost::math::students_t dist(n - p); // distribution
	double t_dist_crit = boost::math::quantile(dist, 1 - alpha / 2);
	// std::cout << "Critical t-value for 95% confidence: " << t_dist_crit << std::endl;

	return t_dist_crit;
}

std::vector<double> ComputeMarginOfError(Data data, std::vector<double> coeffs, int n_preds) {
	// compute and return the margin of error for the prediction interval

	double res_var = ComputeResidualVariance(data, coeffs); // residual variance

	double t_dist_crit = GetCriticalValueTDistribution(data.size(), coeffs.size());

	std::vector<double> t_vals;
	for (const auto& d : data) {
		t_vals.push_back(d.first);
	}

	std::vector<double> error_margins(n_preds);
	for (int t = 1; t <= n_preds; t++) {
		error_margins[t - 1] = t_dist_crit * std::sqrt(res_var * (1 + ComputeLeverage(t_vals, t_vals.back() + t))); 
		// pred interval = y_pred +- error_margin
	}

	return error_margins;
}
