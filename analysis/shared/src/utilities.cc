#include "../includes/utilities.hpp"

bool IsNumber(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    
    std::istringstream iss(s);
    double d;
    
    iss >> d; // try to convert the string to a double
    
    // check if conversion was successful and the whole string was processed
    return !iss.fail() && iss.eof();
}

Tokens SplitString(const std::string& s, char delimiter) {
    Tokens tokens;

    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        if (IsNumber(token)) {
            tokens.push_back(std::stod(token));
        } else {
            tokens.push_back(token);
        }
    }

    return tokens;
}

double GetTotalMemory() {
    double total_mem;

    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        total_mem = info.totalram / 1024; // convert it to KB
    }

    return total_mem;
}

// to read the IP addresses of all remote nodes/ candidates
std::vector<std::string> ReadNodeIP(const std::string& filename) {
    std::vector<std::string> node_ip;

    std::ifstream file(filename);
    std::string line;

    while (getline(file, line) && !line.empty()) {
        node_ip.push_back(line);
    }

    file.close();
    return node_ip;
}

// to read the IDs and criticality levels of the containers
std::map<std::string, int> ReadContainerInfo(const std::string& folder) {
    std::map<std::string, int> container_info; // container id + critiality

    std::ifstream file(folder + "/containers.csv");
    std::string line;

    while (getline(file, line)) {
        Tokens tokens = SplitString(line);

        container_info[boost::get<std::string>(tokens[0])] = boost::get<double>(tokens[1]);
    }

    file.close();
    return container_info;
}

void WriteFileIfNeeded(const std::string& filename, const std::vector<std::string>& lines) {
    // try to open the file for reading
    std::ifstream file_in(filename);

    if (file_in) {
        // read its contents
        std::vector<std::string> file_lines;

        std::string line;
        while (std::getline(file_in, line)) {
            file_lines.push_back(line);
        }

        if (file_lines == lines) {
            return; // if the contents are identical, do nothing
        }
    }

    // if the content is different or file doesn't exist, overwrite the file
    std::ofstream file_out(filename, std::ofstream::trunc);

    if (file_out) { // no errors opening the file
        for (const auto& str : lines) {
            file_out << str << std::endl;
        }
    } else {
        std::cerr << "Error opening file " << filename << " for writing!" << std::endl;
    }
}
