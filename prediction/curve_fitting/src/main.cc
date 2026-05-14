#include <chrono>

#include "types.hpp"
#include "utilities.hpp"
#include "model_selection.hpp"
#include "interval.hpp"

PredsWErrors GetPredictionsWithErrorMargins(Data data, int n_preds) {
	std::vector<double> coeffs = FindBestFitCurve(data, n_preds);

	std::vector<double> preds(n_preds);
	for (int t = 1; t <= n_preds; t++) {
		preds[t - 1] = std::max(Predict(data, coeffs, t), 0.0);
	}

	std::vector<double> error_margins = ComputeMarginOfError(data, coeffs, n_preds);

	return {preds, error_margins};
}

int main(int argc, char* argv[]) {
	// argv = folder, number of predictions
	// output: best prediction

	if (argc != 3) {
		std::cout << "2 arguments needed" << std::endl;
		return 1;
	}

	FullData full_data = ReadFile(GetFilename(argv[1], "stats_tail.csv"));
	int n_preds = std::stoi(argv[2]); 

	// auto start = std::chrono::steady_clock::now();

	PredsWErrors cpu_preds_w_margins = GetPredictionsWithErrorMargins(full_data.first, n_preds);
	PredsWErrors mem_preds_w_margins = GetPredictionsWithErrorMargins(full_data.second, n_preds);

	// auto end = std::chrono::steady_clock::now();
	// std::cout << "Time (ns): ";
	// std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;

	WriteToFile(GetFilename(argv[1], "predictions.csv"), cpu_preds_w_margins, mem_preds_w_margins);

	return 0;
}