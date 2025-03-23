//
// Created by vcnt on 3/23/25.
//

#include "Kmeans.h"

#include <algorithm>
#include <stdexcept>
#include <random>
#include "Utils.h"

Kmeans::Kmeans(const int k, const int dimensions, const int max_iterations) :
                _k(k),
                _max_iterations(max_iterations),
                _point_dimensions(dimensions)
{
    if ( k <= 0 ) throw std::invalid_argument("[ERROR] Number of clusters must be positive.");
    this->_centers.resize(k);
}

std::vector<Point> Kmeans::centers() const { return this->_centers; }

void Kmeans::fit(std::vector<Point> &points) {

    if ( ! Utils::validate(points,_point_dimensions) ) {
        throw std::invalid_argument( "[ERROR] Points don't have an matching number of dimensions.");
    }
    std::random_device rd;
    std::mt19937 gen(rd());

    std::ranges::shuffle(points, gen);
    for (int i = 0; i < _k; ++i) {
        _centers[i] = points[i];
    }

    for ( int iter = 0; iter < _max_iterations; ++iter ) {
        bool converged = true;

        // group all points
        for ( auto& point : points ) {
            const int closest_cluster = static_cast<int> ( findClosestCluster(point) );

            if ( closest_cluster == point.cluster() ) continue;

            // closest cluster has changed
            point.setCluster(closest_cluster);
            converged = false;

        }

        // move centers
        updateCenters(points);

        // nothing has changed so break
        //if ( converged ) {
        //    std::cout << "Finished earlier due to conversion. Total interactions : "<< iter+1 <<std::endl;
        //    break;
        //}
    }
}

size_t Kmeans::findClosestCluster(const Point &point) const {

    // compute distance to the first center
    double minDistance = Utils::euclideanDistance(point, _centers.at(0));
    size_t closestCluster = 0;

    // find min distance
    for ( size_t i = 1; i < _centers.size(); ++i ) {

        if (const double distance = Utils::euclideanDistance(point,_centers.at(i)); distance < minDistance ) {
            closestCluster = i;
            minDistance = distance;
        }

    }

    return closestCluster;
}

void Kmeans::updateCenters(const std::vector<Point> &points) {
    /*
     * move cluster centers to the mean between all the points
    */

    if ( ! Utils::validate(points,_point_dimensions) ) {
        throw std::invalid_argument( "[ERROR] Points don't have an matching number of dimensions.");
    }

    std::vector<int> cluster_sizes(_k,0); // keep track of the amount of points near each center
    std::vector<std::vector<double>> new_centers_coordinates(_k,std::vector<double>(_point_dimensions, 0.0));
    std::vector<Point> new_centers(_k);

    for ( auto& point : points ) {
        const int cluster_id = point.cluster();

        // sum each coordinate
        auto point_coordinates = point.cords();
        for ( size_t i  = 0; i < _point_dimensions; ++i ) {
            new_centers_coordinates[cluster_id][i] += point_coordinates[i];
        }

        // increment amount of point near this cluster
        cluster_sizes[cluster_id]++;

    }

    // update centers
    for ( size_t cluster_id = 0; cluster_id < _k; ++cluster_id ) {

        if ( cluster_sizes.at(cluster_id) == 0 ) {
            // if there are no point nearby then don't move the point
            new_centers[cluster_id] = _centers[cluster_id];
            continue;
        }

        // move the point to the mean coordinate
        for ( size_t i = 0 ; i < _point_dimensions; ++i ) {
            new_centers_coordinates[cluster_id][i] /= cluster_sizes[cluster_id];
        }
        new_centers[cluster_id] = Point(new_centers_coordinates[cluster_id]);

    }

    // update centers
    _centers = new_centers;

}
