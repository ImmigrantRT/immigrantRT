from parameter_estimation import estimate_p, estimate_d

# parameter limits
p_max = 20
d_max = 5
q_max = 10

def get_parameter_grid():
    """Specify hyperparameters for a 3D grid search."""
    grid = []
    for p in range(0, p_max):
        for d in range(0, d_max):
            for q in range(0, q_max):
                grid.append((p, d, q))
    
    return grid

def get_reduced_parameter_grid(series):
    """Specify hyperparameters for a 1D grid search."""
    d = estimate_d(series, d_max)
    p = estimate_p(series, p_max, d)

    grid = []
    for q in range(0, q_max):
        grid.append((p, d, q))
    
    return grid
