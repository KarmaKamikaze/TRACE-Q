#include "trajectory.h"
#include <iostream>
#include <vector>

struct Location {
    double longitude;
    double latitude;
};

struct Trajectory {
    int id;
    std::vector<Location> locations;
};

void trajectorytest () {
    Location location1;
    location1.longitude = 3.446;
    location1.latitude = 53.345;

    Location location2;
    location2.longitude = 59.344;
    location2.latitude = 21.433;

    Trajectory trajectory1;
    trajectory1.id = 1;
    trajectory1.locations.push_back(location1);
    trajectory1.locations.push_back(location2);


    std::cout << "id: " << trajectory1.id << " long1, lat1: " << trajectory1.locations[0].longitude << " " << trajectory1.locations[0].latitude << std::endl;
}


