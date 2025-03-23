//
// Created by vcnt on 3/23/25.
//

#include "Utils.h"

#include <cmath>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <format>
#include <unordered_set>
#include <unistd.h>
#include <limits.h>

double Utils::euclideanDistance(const Point &p1, const Point &p2) {

    const size_t size = p1.cords().size();

    if ( size != p2.cords().size() ) {
        throw std::invalid_argument("The sizes of the coordinates do not match.");
    }

    double sum = .0;

    for ( size_t i = 0; i < size; ++i) {
        sum += std::pow( p1.cords().at(i) - p2.cords().at(i), 2 );
    }

    return std::sqrt(sum);
}

bool Utils::validate(const std::vector<Point> &vec) {

    // ensure vector has elements
    const size_t size = vec.size();
    if (size == 0) return false;

    // get expected amount of elements
    const size_t expected = vec.at(0).cords().size();
    if ( expected == 0) return false;

    return validate(vec,expected);
}

bool Utils::validate(const std::vector<Point> &vec, const size_t expected) {

    // ensure vector has elements
    const size_t size = vec.size();
    if (size == 0) return false;

    // check if every Point has the same amount of dimensions
    for ( size_t i = 1 ; i < size; ++i) {
        if ( vec.at(i).cords().size() != expected ) return false;
    }

    return true;
}

std::vector<std::unordered_map<std::string, std::string>> Utils::processCsv(std::string path, int& counter, const int limit) {
    /*
     * first line of file must be a header
     */

    // open file
    std::ifstream csvFile(path);
    if ( !csvFile.is_open() ) throw std::runtime_error(std::format("[ERROR] Could not open file: {}", path));

    std::vector< std::unordered_map<std::string,std::string> >response;

    std::string line;
    std::vector<std::string> header;

    // read header
    if (getline(csvFile, line)) {
        header = split(line, ',');
    }
    else throw std::invalid_argument("[ERROR] cannot read header.");

    // read other lines
    std::vector<std::string> parts;

    while ( getline(csvFile,line) ) {

        counter++;

        parts = split(line,',');

        // create map
        std::unordered_map<std::string, std::string> temp;
        for ( size_t i = 0; i < header.size(); ++i ) {
            temp.insert({header[i], parts[i] });
        }

        response.push_back(temp);

        if ( limit != -1 && counter == limit) break; // limit lines
    }

    return response;
}

std::vector<std::string> Utils::split(const std::string &str, const char delimiter) {

    std::vector<std::string> tokens;
    std::stringstream ss(str); // create a stream from str
    std::string token;

    while (std::getline(ss,token,delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> Utils::findNumericFields(std::unordered_map<std::string, std::string> const& data) {

    std::vector<std::string> response;

    // iterate over each pairs
    for (const auto&[fst, snd]: data) {
        try {
            // try to convert to double
            std::stod(snd);
            response.push_back(fst);
        } catch (const std::invalid_argument&) {}
    }

    return response;
}

void Utils::pointsFromMap(std::vector<Point> &vector,
    const std::vector<std::unordered_map<std::string, std::string>> &data,
    const std::vector<std::string> &fields) {
    /*
     * vector -> vector where Point objects should be created.
     * data -> map with all fields.
     * fields -> fields that should be used to create the point.
     * [!!!] make sure every field contains double.
     */

    // Calculate min and max for each field across all data
    std::unordered_map<std::string, double> min_vals;
    std::unordered_map<std::string, double> max_vals;

    // Iterate over data to find the min and max values for each field
    for (auto& entry : data) {
        for (auto& field : fields) {
            double val;

            try { val =  std::stod(entry.at(field)); }
            catch (std::invalid_argument&) { val = 0; }

            // check if min
            if (!min_vals.contains(field) || val < min_vals[field]) {
                min_vals[field] = val;
            }

            // check if max
            if (!max_vals.contains(field) || val > max_vals[field]) {
                max_vals[field] = val;
            }
        }
    }


    // iterate over each entry of data
    for ( auto& entry : data ) {

        std::vector<double> temp;

        // iterate over fields
        for ( auto& field : fields) {
            double val;

            try { val =  std::stod(entry.at(field)); }
            catch (std::invalid_argument&) { val = 0; }

            const double min_val = min_vals[field];
            const double max_val = max_vals[field];

            // min-max normalization
            double den = max_val-min_val;
            if ( den == 0) den+= 1e-6;
            double normalizedVal = (val - min_val) / (den);

            temp.push_back(normalizedVal);
        }

        // add to the vector
        vector.emplace_back(temp);
    }
}

std::vector<int> Utils::findLonelyClusters(const std::vector<Point> &points, const int num_clusters) {
    std::vector<int> response;
    std::unordered_set<int> seenClusters;  // To track unique cluster IDs

    // Collect all clusters that exist in the given points
    for (const auto& point : points) {
        seenClusters.insert(point.cluster());
    }

    // Identify missing clusters
    for (int i = 0; i < num_clusters; ++i) {
        if (!seenClusters.contains(i)) {
            response.push_back(i);
        }
    }

    return response;
}

std::map<int,std::vector<int>> Utils::groupByClusters(const std::vector<Point> &points) {

    std::map<int,std::vector<int>>  response;

    // iterate over each point
    for ( int i  = 0; i < static_cast<int> (points.size()) ; ++i ) {

        Point point;
        point = points[i];

        // retrieve cluster_id
        const int cluster_id = point.cluster();

        if ( response.contains(cluster_id)) {
            // already has an entry
            response[cluster_id].push_back(i);
            continue;
        }

        // new entry
        response[cluster_id] = {i};

    }

    return response;
}

void Utils::toFile(const std::map<int, std::vector<int>> &map,
    const std::vector<std::unordered_map<std::string,std::string>>& info,
    std::ostream& out,
    const std::vector<std::string>& fields) {


    for ( const auto& [cluster_id, points_id] : map ) {
        out << std::format("Cluster {}: \n",cluster_id);

        for ( const int i : points_id) {

            const auto& allTrackInfo = info.at(i);

            // name,album,artists
            std::vector<std::string> temp;
            for ( auto& field : fields) temp.push_back(std::format("{};" ,allTrackInfo.at(field)));
            Utils::displayVector(out,temp);
            out.flush();
        }

        out << std::endl;
    }
}