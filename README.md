# P8-Project

## Prerequisites
Before running the application, ensure that you have downloaded the necessary project dependencies.
### Windows
If you are on Windows, run the PowerShell script to install the project dependencies:
```sh
./setup.ps1
```
### Linux
If you are on Linux, run the bash script to install the project dependencies:
```sh
./setup.sh
```
### Manual dependency installation

#### Step1: Get datasets:
1. T-Drive: https://www.kaggle.com/datasets/arashnic/tdriver
2. Geolife: https://www.microsoft.com/en-us/download/details.aspx?id=52367

#### Step 2: Create folders:
1. `TRACE-Q/external/datasets`
   1. `/t-drive`
   2. `/geolife`

#### Step 3: Move datasets into folders:
1. T-Drive dataset goes into `/t-drive`
2. Geolife dataset goes into `/geolife`
3. `TRACE-Q/`
   1. Create `trajectory.db` file
