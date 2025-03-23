//
// Created by vcnt on 3/23/25.
//

#ifndef UTILS_H
#define UTILS_H

#include "Point.h"
#include <unordered_map>
#include <map>
#include <string>
#include <iostream>


class Utils {
public:
    [[nodiscard]] static double euclideanDistance( const Point& p1, const Point& p2);

    [[nodiscard]] static bool validate(const std::vector<Point> & vec);

    [[nodiscard]] static bool validate(const std::vector<Point> &vec, size_t expected);

    [[nodiscard]] static std::vector<std::unordered_map<std::string, std::string>> processCsv(std::string path, int &counter, int limit);

    [[nodiscard]] static std::vector<std::string> split(const std::string &str, char delimiter);

    static std::vector<std::string> findNumericFields(std::unordered_map<std::string, std::string> const& data);

    static void pointsFromMap(std::vector<Point> & vector,
        const std::vector<std::unordered_map<std::string, std::string>> & data,
        const std::vector<std::string> & fields);

    static std::vector<int> findLonelyClusters(const std::vector<Point> &points, int num_clusters);

    static std::map<int, std::vector<int>> groupByClusters(const std::vector<Point> &points);

    template<class T>
    static void displayVector(std::ostream& os,const std::vector<T>& s) {
        for (const auto& item : s) {
            os << item << " ";
        }
        os << std::endl;
    }

    template<class T,class P>
    static void displayMap(std::ostream& os,const std::unordered_map<T,P>& s) {
        for (const auto& [fst,scd] : s) {
            os << fst<< " : " << scd << std::endl;
        }
        os << std::endl;
    }

    static void toFile(const std::map<int, std::vector<int>> &map, const std::vector<std::unordered_map<std::string, std::string>> &info,
                       std::ostream& out, const std::vector<std::string> &fields);

    static std::string getExecutablePath();
};


#endif //UTILS_H
