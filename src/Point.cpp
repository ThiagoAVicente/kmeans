//
// Created by vcnt on 3/23/25.
//

#include "Point.h"
#include "Utils.h"

Point::Point(std::vector<double> cords) {
    this->_cords = std::move(cords);
    this->_cluster = -1;
}

std::vector<double> Point::cords() const { return  this->_cords;}
int Point::cluster() const { return this->_cluster; }

void Point::setCluster(const int c) {
    _cluster = c;
}

void Point::display() const {
    std::cout << "Point: "<< this->cluster() <<std::endl;
    Utils::displayVector(std::cout,_cords);
}


