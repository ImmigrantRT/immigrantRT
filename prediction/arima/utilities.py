import pandas as pd
import csv

def get_filename(folder, file):
    if folder[-1] != '/':
        folder += '/'
    return folder + file

def read_stats(folder):
    filename = get_filename(folder, 'stats_processed.csv')

    df = pd.read_csv(filename, header=None)
    labels = df.columns[1:]

    return df[labels[0]], df[labels[1]]

def write_forecasts(folder, forecasts):
    filename = get_filename(folder, 'forecasts.csv')

    with open(filename, 'w') as f:
        writer = csv.writer(f)
        writer.writerows(zip(forecasts[0], forecasts[1]))
