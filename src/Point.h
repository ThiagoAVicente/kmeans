//
// Created by vcnt on 3/23/25.
//

#ifndef POINT_H
#define POINT_H

#include <vector>


class Point {
        std::vector<double> _cords;
        int _cluster = -1;                                // assigned group

    public:
        Point() = default;

        explicit Point(std::vector<double> cords);
        // getters
        [[nodiscard]] std::vector<double> cords() const;
        [[nodiscard]] int cluster() const;
        void setCluster(int c);

        void display() const;

};



#endif //POINT_H
