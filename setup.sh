#!/bin/bash
tmpfile=$(mktemp)
external_dir="./external"
dataset_dir="./external/datasets"
t_drive_dir="./external/datasets/t-drive"
geolife_dir="./external/datasets/geolife"
SQL_CREATE_ORIGINAL=$(<"$(cd "$(dirname "${BASH_SOURCE[0]}")" || exit; pwd -P)"/sql/create_table_original.sql)
SQL_INDEX_ORIGINAL=$(<"$(cd "$(dirname "${BASH_SOURCE[0]}")" || exit; pwd -P)"/sql/create_index_original.sql)
SQL_CREATE_SIMPLIFIED=$(<"$(cd "$(dirname "${BASH_SOURCE[0]}")" || exit; pwd -P)"/sql/create_table_simplified.sql)
SQL_INDEX_SIMPLIFIED=$(<"$(cd "$(dirname "${BASH_SOURCE[0]}")" || exit; pwd -P)"/sql/create_index_simplified.sql)

echo "Checking that folder structure exists"

if [ ! -d "$external_dir" ]
then
  echo "Creating $external_dir"
  mkdir -p -- "$external_dir"
fi

if [ ! -d "$dataset_dir" ]
then
  echo "Creating $dataset_dir"
  mkdir -p -- "$dataset_dir"
fi

if [ ! -d "$t_drive_dir" ]
then
  echo "Creating $t_drive_dir"
  mkdir -p -- "$t_drive_dir"
  echo "Downloading T-Drive trajectory dataset"
  wget --user-agent="Mozilla" "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/06.zip" -O "$tmpfile"
  echo "Unpacking T-Drive dataset part 1"
  unzip -d "$t_drive_dir" "$tmpfile"
  rm "$tmpfile"
  wget --user-agent="Mozilla" "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/07.zip" -O "$tmpfile"
  echo "Unpacking T-Drive dataset part 2"
  unzip -d "$t_drive_dir" "$tmpfile"
  rm "$tmpfile"
  wget --user-agent="Mozilla" "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/08.zip" -O "$tmpfile"
  echo "Unpacking T-Drive dataset part 3"
  unzip -d "$t_drive_dir" "$tmpfile"
  rm "$tmpfile"
  wget --user-agent="Mozilla" "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/09.zip" -O "$tmpfile"
  echo "Unpacking T-Drive dataset part 4"
  unzip -d "$t_drive_dir" "$tmpfile"
  rm "$tmpfile"
  wget --user-agent="Mozilla" "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/010.zip" -O "$tmpfile"
  echo "Unpacking T-Drive dataset part 5"
  unzip -d "$t_drive_dir" "$tmpfile"
  rm "$tmpfile"
  wget --user-agent="Mozilla" "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/011.zip" -O "$tmpfile"
  echo "Unpacking T-Drive dataset part 6"
  unzip -d "$t_drive_dir" "$tmpfile"
  rm "$tmpfile"
  wget --user-agent="Mozilla" "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/012.zip" -O "$tmpfile"
  echo "Unpacking T-Drive dataset part 7"
  unzip -d "$t_drive_dir" "$tmpfile"
  rm "$tmpfile"
  wget --user-agent="Mozilla" "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/013.zip" -O "$tmpfile"
  echo "Unpacking T-Drive dataset part 8"
  unzip -d "$t_drive_dir" "$tmpfile"
  rm "$tmpfile"
  wget --user-agent="Mozilla" "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/014.zip" -O "$tmpfile"
  echo "Unpacking T-Drive dataset part 9"
  unzip -d "$t_drive_dir" "$tmpfile"
  rm "$tmpfile"
fi

if [ ! -d "$geolife_dir" ]
then
  echo "Creating $geolife_dir"
  mkdir -p -- "$geolife_dir"
  echo "Downloading Geolife trajectory dataset"
  wget "https://download.microsoft.com/download/F/4/8/F4894AA5-FDBC-481E-9285-D5F8C4C4F039/Geolife%20Trajectories%201.3.zip" -O "$tmpfile"
  echo "Unpacking Geolife trajectory dataset"
  unzip -d "$geolife_dir" "$tmpfile"
  rm "$tmpfile"
fi

echo "Setting up PostgreSQL database"
createdb traceq -U postgres
psql -U postgres -d traceq -c "$SQL_CREATE_ORIGINAL"
psql -U postgres -d traceq -c "$SQL_INDEX_ORIGINAL"
psql -U postgres -d traceq -c "$SQL_CREATE_SIMPLIFIED"
psql -U postgres -d traceq -c "$SQL_INDEX_SIMPLIFIED"

echo "Finished setup!"
