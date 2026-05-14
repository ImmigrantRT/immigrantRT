#include "../includes/node.hpp"

Node::Node(std::string folder, std::string ip, bool use_forecasts, bool read_preds) {
    ip_ = ip;

    if (use_forecasts) { // for ranking
        ReadForecasts(folder);    
    } else { // for trigger, validation
        if (read_preds) {
            ReadPredictions(folder); 
        }
    } 

    if (ip_.compare("self") == 0) { // for trigger
        ComputeLowestQoSFreq(folder); 
    } else { // for ranking, validation
        ComputeHealthScore(folder);
        ReadThroughput(folder);
    }
}

std::string Node::GetIP() { return ip_; }

std::vector<DataPoint2> Node::GetPredictions() { return predictions_; }

std::vector<DataPoint1> Node::GetForecasts() { return forecasts_; }

std::vector<std::pair<std::string, int>> Node::GetLowestQoSFreq() { return lowest_qos_freq_; }

double Node::GetHealthScore() { return health_score_; }

double Node::GetThroughput() { return throughput_; }

void Node::ReadPredictions(std::string folder) {
    std::string path;
    if (ip_.compare("self") == 0) {
        path = folder + "/other/predictions.csv"; // usage of other applications on the node
    } else {
        path = folder + "/remote_nodes/" + ip_ + "/predictions.csv";
    }
    
    std::ifstream file(path);
    std::string line;

    while (getline(file, line)) {
        Tokens tokens = SplitString(line);

        predictions_.push_back(DataPoint2(
            boost::get<double>(tokens[0]), boost::get<double>(tokens[1]),
            boost::get<double>(tokens[2]), boost::get<double>(tokens[3])
        ));
    }

    file.close();
}

void Node::ReadForecasts(std::string folder) {
    std::ifstream file(folder + "/remote_nodes/" + ip_ + "/forecasts.csv");
    std::string line;

    while (getline(file, line)) {
        Tokens tokens = SplitString(line);

        forecasts_.push_back(DataPoint1(
            boost::get<double>(tokens[0]), boost::get<double>(tokens[1])
        ));
    }

    file.close();
}

void Node::ComputeLowestQoSFreq(std::string folder) {
    std::map<std::string, int> freq_map; // to count the frequency of each container id in QoS.csv

    std::ifstream file(folder + "/self/QoS.csv");
    std::string line;

    while (getline(file, line)) {
        freq_map[line]++;
    }

    file.close();

    lowest_qos_freq_.assign(freq_map.begin(), freq_map.end());
}

void Node::ComputeHealthScore(std::string folder) {
    std::vector<double> health;

    std::ifstream file(folder + "/remote_nodes/" + ip_ + "/health.txt");
    std::string line;

    while (getline(file, line)) {
        Tokens tokens = SplitString(line, ' ');

        health.push_back(boost::get<double>(tokens[2]));
    }

    file.close();

    int n = health.size();

    // std::vector<double> (*get_weights)(int) = equal_weights;
    // std::vector<double> (*get_weights)(int) = geometric_progression_weights;
    std::vector<double> (*get_weights)(int) = sigmoid_weights;

    std::vector<double> weights = get_weights(n);

    health_score_ = 0;
    for (int i = 0; i < n; i++) {
        health_score_ += weights[i] * health[n - i - 1]; // aggregating health data
    }

}

void Node::ReadThroughput(std::string folder) {
    std::ifstream file(folder + "/remote_nodes/" + ip_ + "/throughput.txt");
    std::string line;

    getline(file, line);

    throughput_ = std::stod(line);
}
