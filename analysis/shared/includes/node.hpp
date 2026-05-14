#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/variant.hpp>

#include "datapoint.hpp"
#include "types.hpp"
#include "utilities.hpp"
#include "weights.hpp"

class Node {
public: 
    /// @brief Constructs a Node object
    /// @param ip IP address of remote node (for ranking & validation) OR `self` (for trigger) 
    /// @param use_forecasts Whether to use ARIMA forecasts (for ranking) OR curve fitting predictions (for trigger & validation)
    /// @param read_preds Only neded for validation. 
    ///                   Whether to read predictions (if new data is available from the remote nodes) OR not. 
    ///                   Default is `true`
    Node(std::string, std::string, bool use_forecasts, bool read_preds = true);

    std::string GetIP();
    std::vector<DataPoint2> GetPredictions();
    std::vector<DataPoint1> GetForecasts();
    std::vector<std::pair<std::string, int>> GetLowestQoSFreq();
    double GetHealthScore();
    double GetThroughput(); 

private:
    void ReadPredictions(std::string);
    void ReadForecasts(std::string);
    void ComputeLowestQoSFreq(std::string);
    void ComputeHealthScore(std::string);
    void ReadThroughput(std::string);

    std::string ip_;
    std::vector<DataPoint2> predictions_;
    std::vector<DataPoint1> forecasts_;
    std::vector<std::pair<std::string, int>> lowest_qos_freq_;
    double health_score_;
    double throughput_;
};

#endif
