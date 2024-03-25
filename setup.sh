#!/bin/bash
tmpfile=$(mktemp)
external_dir="./external"
dataset_dir="./external/datasets"
t_drive_dir="./external/datasets/t-drive"
geolife_dir="./external/datasets/geolife"
sqlite_dir="./external/sqlite"
sqlite_version="sqlite-amalgamation-3450100"
sqlite_db_name="trajectory.db"

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

if [ ! -d "$sqlite_dir" ]
then
  echo "Creating $sqlite_dir"
  mkdir -p -- "$sqlite_dir"
  echo "Downloading sqlite3 C/C++ library"
  wget "https://www.sqlite.org/2024/sqlite-amalgamation-3450100.zip" -O "$tmpfile"
  echo "Unpacking sqlite3 library"
  unzip -d "$sqlite_dir" "$tmpfile"
  rm "$tmpfile"
  mv "$sqlite_dir/$sqlite_version/shell.c" "$sqlite_dir/shell.c"
  mv "$sqlite_dir/$sqlite_version/sqlite3.c" "$sqlite_dir/sqlite3.c"
  mv "$sqlite_dir/$sqlite_version/sqlite3.h" "$sqlite_dir/sqlite3.h"
  mv "$sqlite_dir/$sqlite_version/sqlite3ext.h" "$sqlite_dir/sqlite3ext.h"
  rm -d "$sqlite_dir/$sqlite_version/"
  echo "Creating $sqlite_db_name file"
  touch "$sqlite_db_name"
fi

echo "Finished setup!"
