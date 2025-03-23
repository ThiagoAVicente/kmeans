
# *K-means with ThreadPool Implementation*

## *Description*
This project implements the K-means clustering algorithm with an added [ThreadPool](https://en.wikipedia.org/wiki/Thread_pool) for parallelizing the computation, optimizing the performance for larger datasets. 

## *What is k-means?*
K-means is a clustering algorithm that groups data points into K clusters based on similarity, by iteratively assigning points to the nearest cluster center and updating the centers. [more](https://en.wikipedia.org/wiki/K-means_clustering)
## *Requirements*
- **CMake**: A build system generator.
- **C++**: The project is implemented in C++.

## *Application*
The project was tested with a Spotify dataset, which was obtained from [Kaggle](https://www.kaggle.com/code/sivarazadi/k-means-spotify-song-clustering). The dataset contains various features related to music tracks. I used an python script to remove some columns that were not relevant for the project and kept the numeric data.

The results were satisfying. After listening to a few songs it was clear that the songs inside the same cluster were kinda similar.

The ThreadPool increased by far the performance of the execution with a large amount of data.

##  *How to run*
To build and run the project:
```bash
git clone git@github.com:ThiagoAVicente/kmeans.git
cd kmeans
mkdir build
cd build
cmake ..
make
./kmeans
```

After executing a file named *"output.txt"* can be found in the same folder. It contains the results of the k-means algorithm.

