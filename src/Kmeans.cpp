//
// Created by vcnt on 3/23/25.
//

#include "Kmeans.h"

#include <algorithm>
#include <future>
#include <stdexcept>
#include <random>

#include "ThreadPool.h"
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

void Kmeans::fit(std::vector<Point> &points, ThreadPool& pool) {
    if (!Utils::validate(points, _point_dimensions)) {
        throw std::invalid_argument("[ERROR] Points don't have an matching number of dimensions.");
    }
    std::random_device rd;
    std::mt19937 gen(rd());

    std::ranges::shuffle(points, gen);
    for (int i = 0; i < _k; ++i) {
        _centers[i] = points[i];
    }

    for (int iter = 0; iter < _max_iterations; ++iter) {
        std::atomic<bool> converged = true;
        std::vector<std::future<void>> futures;

        // group using threads
        const size_t batchSize = points.size() / pool.numThreads();
        for (size_t start = 0; start < points.size(); start += batchSize) {

            futures.push_back(pool.enqueue([this, &points, &converged, start, batchSize]() {
                const size_t end = std::min(start + batchSize, points.size());

                for (size_t i = start; i < end; i++) {
                    int closest_cluster = static_cast<int>(findClosestCluster(points[i]));
                    if (closest_cluster != points[i].cluster()) {
                        points[i].setCluster(closest_cluster);
                        converged = false;
                    }
                }
            }));
        }
        // Wait for all tasks to finish.
        for (auto &future : futures) {
            future.get();
        }

        updateCenters(points, pool);

        if (converged) {
            std::cout << "Finished earlier due to convergence. Total iterations: " << iter + 1 << std::endl;
            break;
        }
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

void Kmeans::updateCenters(const std::vector<Point> &points, ThreadPool &pool) {
    /*
     * move cluster centers to the mean between all the points
    */

    if ( ! Utils::validate(points,_point_dimensions) ) {
        throw std::invalid_argument( "[ERROR] Points don't have an matching number of dimensions.");
    }

    const size_t totalPoints = points.size();
    const size_t numThreads = pool.numThreads();
    const size_t batch_size = totalPoints / numThreads;

    std::vector< std::future< std::pair<std::vector<int> ,std::vector<std::vector<double>>>>> futures;

    //-- compute using batches and threads
    for ( size_t start = 0; start < totalPoints; start+=batch_size ) {
        futures.push_back( pool.enqueue(
                [this,&points,start,batch_size,totalPoints]()->std::pair<std::vector<int>,std::vector<std::vector<double>>> {

                    // need to save local data and return it with future
                    std::vector<int> local_counts(_k, 0);
                    std::vector<std::vector<double>> local_sums(_k, std::vector<double>(_point_dimensions, 0.0));


                    const size_t end = std::min(start+batch_size, totalPoints);

                    // iterate over the batch
                    for ( size_t i = start; i < end; ++i ) {
                        int cluster_id = points[i].cluster();
                        const auto& point_coordinates = points[i].cords();

                        // sum to local
                        for ( size_t d = 0; d < _point_dimensions; ++d) {
                            // sum to the correct dimension
                            local_sums[cluster_id][d] += point_coordinates[d];
                        }

                        // increment counter
                        local_counts[cluster_id]++;
                    }
                    return std::make_pair(local_counts,local_sums);
            }));

    }

    //-- sum all local counts and sums
    std::vector<int> cluster_counts(_k, 0);
    std::vector<std::vector<double>> total_sums(_k, std::vector<double>(_point_dimensions, 0.0));
    for ( auto& future: futures ) {
        auto [partial_counts, partial_sums] = future.get();
        for ( size_t cluster_id = 0; cluster_id < _k; ++ cluster_id) {

            // update counter
            cluster_counts[cluster_id] += partial_counts[cluster_id];

            // update all coordinates sum
            for ( size_t d = 0; d < _point_dimensions; ++d) {
                total_sums[cluster_id][d] += partial_sums[cluster_id][d];
            }
        }
    }


    //-- update centers
    std::vector<Point> new_centers(_k);
    for ( size_t cluster_id = 0; cluster_id < _k; ++cluster_id ) {

        if ( cluster_counts[cluster_id] == 0) {
            // no point is near this center.
            // keep its position
            new_centers[cluster_id] = _centers[cluster_id];
            continue;
        }

        // compute the average of the coordinates
        for ( size_t d = 0 ; d < _point_dimensions; ++d) {
            total_sums[cluster_id][d] /=cluster_counts[cluster_id];
        }

        // create
        new_centers[cluster_id] = Point(total_sums[cluster_id]);
    }

    // update centers
    _centers = new_centers;

}
