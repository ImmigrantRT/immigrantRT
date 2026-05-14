def get_all_forecasts(timesteps, models, norm_weights):
    """Get forecasts from all models."""
    all_forecasts = [[0] * timesteps] * len(models) # forecasts from different models
    for i in range(len(models)):
        model_result = models[i]
        if norm_weights[i] != 0:
            all_forecasts[i] = list(model_result.forecast(steps=timesteps))
    
    return all_forecasts

def forecast(series, timesteps, models, norm_weights):
    """Generate forecasts for the next t timesteps."""
    all_forecasts = get_all_forecasts(timesteps, models, norm_weights)
    
    forecasts = []
    for t in range(timesteps):
        forecast_t = sum([all_forecasts[i][t] * norm_weights[i] for i in range(len(models))])
        forecasts.append(forecast_t)

    return forecasts