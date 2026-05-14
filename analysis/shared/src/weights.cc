#include "../includes/weights.hpp"

void normalize(std::vector<double>& v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);  // sum of elements

    std::vector<double> normalized_v(v.size());

    for (int i = 0; i < v.size(); i++) {
        v[i] /= sum;  // Normalize each element
    }
}

std::vector<double> equal_weights(int n) {
    std::vector<double> w(n, 1.0);  // vector of size n filled with 1's

    normalize(w);
    return w;
}

std::vector<double> geometric_progression_weights(int n) {
    std::vector<double> w(n);

    double a = 1.0;
    double r = 0.5;

    for (int i = 0; i < n; i++) {
        w[i] = a * std::pow(r, i);
    }

    normalize(w);
    return w;
}

std::vector<double> sigmoid_weights(int n) {
    std::vector<double> w(n);

    for (int i = 0; i < n; i++) {
        int x = -i;
        w[i] = 1.0 / (1.0 + std::exp(-x));
    }

    normalize(w);
    return w;
}
