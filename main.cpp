#include <format>
#include <fstream>
#include <iostream>
#include "vector"
#include "src/Point.h"
#include "src/Kmeans.h"
#include "src/Utils.h"

#define LIMIT 1000
#define MAX_ITERATIONS 1000
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    int lines = 0;
    constexpr int k = 10; // amount of clusters in kmeans

    // read from file
    const auto data = Utils::processCsv("../data/tracks/cleaned_tracks_features.csv", lines, LIMIT);
    std::cout << "Read "<<lines<<" lines."<<std::endl;

    // get numeric fields
    const std::vector<std::string> numeric_fields = Utils::findNumericFields(data.at(0));
    const int dimensions = static_cast<int> ( numeric_fields.size() );

    // create points representing the tracks
    std::vector<Point> tracks;
    Utils::pointsFromMap(tracks,data,numeric_fields);

    Kmeans km(10,dimensions,MAX_ITERATIONS);
    km.fit(tracks);

    std::cout << "Lonely clusters: ";
    const auto lonely = Utils::findLonelyClusters(tracks, k);
    Utils::displayVector(std::cout,lonely);

    const auto result = Utils::groupByClusters(tracks);
    for ( const auto& [cluster_id, points_id] : result ) {
        std::cout << std::format("Cluster {}: ",cluster_id);
        Utils::displayVector(std::cout,points_id);
    }

    const std::vector<std::string> desired_fields = { "name", "album","artists"};

    const auto path = "output.txt";
    std::ofstream out(path);
    if ( !out.is_open() ) {
        throw std::runtime_error(std::format("[ERROR] Could not create file: {}", path));
    }

    Utils::toFile(result,data, out, desired_fields);
    out.close();
    return 0;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.