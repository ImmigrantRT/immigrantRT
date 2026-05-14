import sys
import warnings

from utilities import read_stats, write_forecasts
from parameter_search import get_parameter_grid, get_reduced_parameter_grid
from model_selection import minimize_rmse, minimize_aic, create_ensemble
from forecasting import forecast

def main():
    cpu_data, mem_data = read_stats(sys.argv[1])
    timesteps = int(sys.argv[2])
    
    forecasts = []
    for series in [cpu_data, mem_data]:
        # Grid
        # param_grid = get_parameter_grid()               # Full 3D grid search
        param_grid = get_reduced_parameter_grid(series) # Reduced 1D hyperparameter search
        # print('Parameter grid:', param_grid)
        
        # Models + selection
        # models, norm_weights = minimize_rmse(series, param_grid)     # Walk-forward validation + min RMSE
        # models, norm_weights = minimize_aic(series, param_grid)      # Min AIC (no walk-forward validation)
        models, norm_weights = create_ensemble(series, param_grid)   # Ensemble of models
        # print('Normalized weights:', norm_weights)

        forecasts.append(forecast(series, timesteps, models, norm_weights))
    
    # print('CPU forecasts:', forecasts[0])
    # print('Mem forecasts:', forecasts[1])

    write_forecasts(sys.argv[1], forecasts)

if __name__ == "__main__":
    warnings.filterwarnings("ignore")
    main()
