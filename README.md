# TRACE-Q: Trajectory Reduction using Accuracy Control for Reliable Querying

TRACE-Q is a cutting-edge research project that addresses the critical need for efficient trajectory data processing 
in the realm of spatiotemporal databases. At its core, TRACE-Q employs the MRPA (Multi-Resolution Polygonal 
Approximation) algorithm to effectively simplify trajectory data. Unlike conventional simplification algorithms, 
TRACE-Q innovatively incorporates query accuracy as a pivotal factor in the simplification process. This ensures that 
the resulting simplified trajectories are not only compact but also retain high usability for subsequent queries. Our 
major contribution lies in this paradigm shift where the accuracy of post-simplification queries directly influences 
the simplification algorithm itself, paving the way for more precise and reliable spatiotemporal analyses.

## Prerequisites
Before running the application, ensure that you have downloaded the necessary project dependencies. Specifically, 
ensure that [PostgreSQL](https://www.postgresql.org/) is installed and added to your PATH variables.

### Windows
For Windows users, navigate to the root directory and execute the following PowerShell script to set up the project 
dependencies:
```sh
./setup.ps1
```

### Linux
For Linux users, navigate to the root directory and utilize the accompanying bash script to set up the project 
dependencies:
```sh
./setup.sh
```
*Optional: The loading of datasets can be skipped by using the `-skipdatasets` flag.*

### Manual Dependency Installation (if none of the above options are appealing)
For manual setup, follow the steps below to prepare the environment and datasets.

#### Step 1: Acquire datasets
Ensure you have the following datasets:
1. [T-Drive](https://1drv.ms/u/s!AsWQUIUFkRXPgsN6OCa9OB-qziGfhA) - A comprehensive taxi trajectory dataset.
2. [Geolife](https://www.microsoft.com/en-us/download/details.aspx?id=52367) - A rich GPS trajectory dataset collected 
in a wide range of scenarios.

#### Step 2: Organize Directories
Create a structured directory within `TRACE-Q/external/datasets`:
1. `/t-drive` - For storing the T-Drive dataset.
2. `/geolife` - For storing the Geolife dataset.

#### Step 3: Prepare Datasets
Position the unpacked datasets in their respective directories:
1. Place the T-Drive dataset into `/t-drive`.
2. Insert the Geolife dataset into `/geolife`.

#### Step 4: Create PostgreSQL database
Run the following commands to create and configure the required database:
```sh
createdb -U postgres traceq
psql -U postgres -d traceq -c 'CREATE TABLE original_trajectories (id BIGSERIAL PRIMARY KEY, trajectory_id  INTEGER NOT NULL, coordinates POINT NOT NULL, time BIGINT);' -c 'CREATE INDEX original_trajectories_index_coords ON original_trajectories USING GIST (coordinates);' -c 'CREATE INDEX original_trajectories_index_time ON original_trajectories (time);'
psql -U postgres -d traceq -c 'CREATE TABLE simplified_trajectories (id BIGSERIAL PRIMARY KEY, trajectory_id  INTEGER NOT NULL, coordinates POINT NOT NULL, time BIGINT);' -c 'CREATE INDEX simplified_trajectories_index_coords ON simplified_trajectories USING GIST (coordinates);' -c 'CREATE INDEX simplified_trajectories_index_time ON simplified_trajectories (time);'
```

## Building and Running TRACE-Q
TRACE-Q is designed to be built using CMake. To build and run TRACE-Q, follow these steps:
### Building the Application
1. **Install CMake:**: Ensure that you have CMake installed on your system. You can download it from 
[CMake’s official website](https://cmake.org/download/).

2. **Clone the Repository**: Clone the TRACE-Q repository to your local machine using the following command:
```sh
git clone https://github.com/KarmaKamikaze/TRACE-Q.git
```

3. **Navigate to the Project Directory**: Change into the cloned repository’s directory:
```sh
cd TRACE-Q
```

4. **Create a Build Directory**: Create a new directory for the build files:
```sh
mkdir build && cd build
```

5. **Generate Build System**: Run CMake to generate the build system:
```sh
cmake ..
```

6. **Compile the Project**: Compile the project using the generated build system:
```sh
cmake --build .
```

After successfully compiling the project, you will find the TRACE_Q executable in the build directory.

### Running the Application

1. **Start the Backend API Server**: Execute TRACE_Q to start the backend API server:
```sh
./TRACE_Q
```
This will launch a REST API server that listens for requests on the defined endpoints.

2. **Clone the Frontend Repository (Optional)**: Clone the accompanying 
[TRACE-Q frontend repository](https://github.com/KarmaKamikaze/TRACE-Q-UI):
```sh
git clone https://github.com/KarmaKamikaze/TRACE-Q-UI.git
```

3. **Build the Frontend (Optional)**: Navigate to the frontend directory and follow the build instructions provided in 
the repository’s README.

4. **Interact with the API**: With the backend server running, you can now interact with the API through the available 
endpoints. Here are some of the endpoints you can use:

- `/insert`: Insert new trajectory data.
- `/load_from_id`: Load trajectory data by ID.
- `/load_from_id_date`: Load trajectory data by ID and date.
- `/run`: Execute the TRACE-Q algorithm on the imported trajectories.
- `/status`: Check the status of the simplification process.
- `/db_range_query`: Perform a range query on the simplified database. 
- `/knn_query`: Execute a k-nearest neighbors query on the simplified database.
- `/reset`: Reset the database state.
- `/get_dates_from_id`: Retrieve available dates for a given trajectory ID.

By following these instructions, you can build and run TRACE-Q on your system, enabling you to perform efficient and 
accurate trajectory queries for your spatiotemporal research needs.

Feel free to adjust the URLs and commands to match your repository and system specifics. Good luck with your project!
