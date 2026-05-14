from math import sqrt
from statsmodels.tsa.stattools import adfuller, pacf

def estimate_d(series, d_max):
    """Estimate the degree of differencing."""
    d = 0 # number of times the series is differenced
    alpha = 0.05 # significance level

    series2 = series
    while d < d_max:
        series2 = series2[d:]
        series2.fillna(0, inplace=True)
        
        adf_p_val = adfuller(series2, autolag='AIC')[1] # p-value statistic
        if adf_p_val <= alpha:
            break

        series2 = series2.diff() # differencing
        d += 1

    return d

def estimate_p(series, p_max, d):
    """Estimate the lag order."""
    for i in range(0, d):
        series = series.diff()

    lags = min(int(series.shape[0] * 0.5) - 1, p_max)
    pacf_arr = pacf(series, method='ywm', nlags=lags)

    sig_level = 2 / sqrt(series.shape[0])

    p = 0 # number of autoregressive terms    
    pacf_arr = pacf_arr[1:] # pacf of lag 0 is always 1
    for i in range(len(pacf_arr)):
        if pacf_arr[i] > sig_level or pacf_arr[i] < -sig_level:
            p += 1
        else:
            break
            
    return p
