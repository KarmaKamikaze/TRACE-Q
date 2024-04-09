#ifndef TRACE_Q_TRAJECTORY_STRUCTURE_H
#define TRACE_Q_TRAJECTORY_STRUCTURE_H

#include <vector>
#include <string>
namespace data_structures {

    struct Location {
        int order{};
        long timestamp{};
        double longitude{};
        double latitude{};

        bool operator==(const Location& other) const {
            if (this->order == other.order && this->longitude == other.longitude
            && this->latitude == other.latitude && this->timestamp == other.timestamp) {
                return true;
            }
            else {
                return false;
            }
        }

        friend std::ostream& operator<<(std::ostream& os, const Location& p) {
            os << std::to_string(p.order);

            return os;
        }
    };

    struct Trajectory {
        unsigned id{};
        std::vector<Location> locations{};

        Location& operator[](const size_t index) {
            return locations[index];
        }

        const Location& operator[](const size_t index) const {
            return locations[index];
        }

        [[nodiscard]] size_t size() const {
            return locations.size();
        }
    };

}

#endif //TRACE_Q_TRAJECTORY_STRUCTURE_H