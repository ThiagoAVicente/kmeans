//
// Created by vcnt on 3/23/25.
//

#ifndef KMEANS_H
#define KMEANS_H

#include "Point.h"
#include "ThreadPool.h"
#include "vector"

class Kmeans {

    int _k;
    int _max_iterations;
    std::vector<Point> _centers;
    int _point_dimensions;


public:
    Kmeans(int k, int dimensions, int max_iterations);
    void fit(std::vector<Point> &points, ThreadPool &pool);
    [[nodiscard]] std::vector<Point> centers()const;

private:
    [[nodiscard]] size_t findClosestCluster(const Point &point) const;
    void updateCenters(const std::vector<Point> &points, ThreadPool &pool);
};



#endif //KMEANS_H
