import numpy as np
import math
from statsmodels.tsa.arima.model import ARIMA

def fit_models(series, param_grid):
    """Fit an ARIMA model to the time series for each order in the grid."""
    models = []
    for order in param_grid:
        try: 
            model = ARIMA(series, order=order).fit()
            models.append(model)
        except:
            continue
    return models

def evaluate_model(series, order):
    """Find the RMSE using walk-forward validation."""
    split = int(len(series) * 0.9) # 90-10 split
    training_samples = series[0 : split]
    test_samples = series[split : len(series)]
    
    history = training_samples
    predictions = []
    
    for i in range(len(test_samples)):
        model = ARIMA(history, order=order).fit()
        pred = model.forecast()
        predictions.append(pred[0])
        history.append(test_samples[i])
    
    mse = ((np.array(test_samples) - np.array(predictions))**2).mean()
    rmse = math.sqrt(mse)

    # print('Order:', order)
    # print('RMSE:', rmse)

    return rmse

def minimize_rmse(series, param_grid):
    """Select ARIMA model with the lowest RMSE."""
    models = fit_models(series, param_grid)
    rmse = [evaluate_model(list(series), order) for order in param_grid] # walk-forward validation

    idx = rmse.index(min(rmse))
    norm_weights = [(1 if i == idx else 0) for i in range(len(models))]

    return models, norm_weights

def minimize_aic(series, param_grid):
    """Select ARIMA model with the lowest AIC."""
    models = fit_models(series, param_grid)
    aics = [model.aic for model in models]

    idx = aics.index(min(aics))
    norm_weights = [(1 if i == idx else 0) for i in range(len(models))]

    return models, norm_weights

def create_ensemble(series, param_grid):
    """Create an ensemble of ARIMA models with weights assigned based on the AICs."""
    models = fit_models(series, param_grid)
    aics = [model.aic for model in models]
    
    # eliminate outliers
    sorted_aics = aics
    sorted_aics.sort()
    
    q1 = np.percentile(sorted_aics, 25, interpolation='midpoint')
    q3 = np.percentile(sorted_aics, 75, interpolation='midpoint')
    iqr = q3 - q1 # interquartile range
    upper_limit = q3 + 1.5 * iqr
    lower_limit = q1 - 1.5 * iqr
    
    for aic in sorted_aics:
        if aic < lower_limit or aic > upper_limit:
            idx = aics.index(aic)
            del aics[idx]
            del models[idx]

    weights = []
    for i in range(len(aics)):
        delta = aics[i] - min(aics)
        weights.append(math.exp(-0.5 * delta))

    norm_weights = [float(w) / sum(weights) for w in weights]

    return models, norm_weights
