$external_dir = ".\external"
$dataset_dir = ".\external\datasets"
$t_drive_dir = ".\external\datasets\t-drive"
$geolife_dir = ".\external\datasets\geolife"
$SQL_CREATE_ORIGINAL = Get-Content -Raw -Path "$(Get-Location)\sql\create_table_original.sql"
$SQL_CREATE_SIMPLIFIED = Get-Content -Raw -Path "$(Get-Location)\sql\create_table_simplified.sql"

Write-Host "Checking that folder structure exists"

if (-not (Test-Path $external_dir -PathType Container)) {
    Write-Host "Creating $external_dir"
    New-Item -Path $external_dir -ItemType Directory | Out-Null
}

if (-not (Test-Path $dataset_dir -PathType Container)) {
    Write-Host "Creating $dataset_dir"
    New-Item -Path $dataset_dir -ItemType Directory | Out-Null
}

if (-not (Test-Path $t_drive_dir -PathType Container)) {
    Write-Host "Creating $t_drive_dir"
    New-Item -Path $t_drive_dir -ItemType Directory | Out-Null
    Write-Host "Downloading T-Drive trajectory dataset"
    $outputPath = Join-Path -Path $t_drive_dir -ChildPath "temp.zip"
    Invoke-WebRequest -Uri "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/06.zip" -OutFile $outputPath
    Write-Host "Unpacking T-Drive dataset part 1"
    Expand-Archive -Path $outputPath -DestinationPath $t_drive_dir
    Remove-Item -Path $outputPath
    Invoke-WebRequest -Uri "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/07.zip" -OutFile $outputPath
    Write-Host "Unpacking T-Drive dataset part 2"
    Expand-Archive -Path $outputPath -DestinationPath $t_drive_dir
    Remove-Item -Path $outputPath
    Invoke-WebRequest -Uri "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/08.zip" -OutFile $outputPath
    Write-Host "Unpacking T-Drive dataset part 3"
    Expand-Archive -Path $outputPath -DestinationPath $t_drive_dir
    Remove-Item -Path $outputPath
    Invoke-WebRequest -Uri "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/09.zip" -OutFile $outputPath
    Write-Host "Unpacking T-Drive dataset part 4"
    Expand-Archive -Path $outputPath -DestinationPath $t_drive_dir
    Remove-Item -Path $outputPath
    Invoke-WebRequest -Uri "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/010.zip" -OutFile $outputPath
    Write-Host "Unpacking T-Drive dataset part 5"
    Expand-Archive -Path $outputPath -DestinationPath $t_drive_dir
    Remove-Item -Path $outputPath
    Invoke-WebRequest -Uri "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/011.zip" -OutFile $outputPath
    Write-Host "Unpacking T-Drive dataset part 6"
    Expand-Archive -Path $outputPath -DestinationPath $t_drive_dir
    Remove-Item -Path $outputPath
    Invoke-WebRequest -Uri "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/012.zip" -OutFile $outputPath
    Write-Host "Unpacking T-Drive dataset part 7"
    Expand-Archive -Path $outputPath -DestinationPath $t_drive_dir
    Remove-Item -Path $outputPath
    Invoke-WebRequest -Uri "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/013.zip" -OutFile $outputPath
    Write-Host "Unpacking T-Drive dataset part 8"
    Expand-Archive -Path $outputPath -DestinationPath $t_drive_dir
    Remove-Item -Path $outputPath
    Invoke-WebRequest -Uri "https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/014.zip" -OutFile $outputPath
    Write-Host "Unpacking T-Drive dataset part 9"
    Expand-Archive -Path $outputPath -DestinationPath $t_drive_dir
    Remove-Item -Path $outputPath
}

if (-not (Test-Path $geolife_dir -PathType Container)) {
    Write-Host "Creating $geolife_dir"
    New-Item -Path $geolife_dir -ItemType Directory | Out-Null
    Write-Host "Downloading Geolife trajectory dataset"
    $outputPath = Join-Path -Path $geolife_dir -ChildPath "temp.zip"
    Invoke-WebRequest -Uri "https://download.microsoft.com/download/F/4/8/F4894AA5-FDBC-481E-9285-D5F8C4C4F039/Geolife%20Trajectories%201.3.zip" -OutFile $outputPath
    Write-Host "Unpacking Geolife trajectory dataset"
    Expand-Archive -Path $outputPath -DestinationPath $geolife_dir
    Remove-Item -Path $outputPath
}

Write-Host "Setting up PostgreSQL database"
createdb -U postgres traceq
psql -U postgres -d traceq -c $SQL_CREATE_ORIGINAL
psql -U postgres -d traceq -c $SQL_CREATE_SIMPLIFIED

Write-Host "Finished setup!"
