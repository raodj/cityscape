#!/bin/bash

#SBATCH --job-name=chicago_trvl_time_stats
#SBATCH --time=12:00:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=4g
#SBATCH --mail-user=raodm@miamioh.edu
#SBATCH --mail-type=END,FAIL
#SBATCH -A PMIU0110

# This script is used to generate aggregate statistics on travel time based
# on 50k randomly generated pairs of buildings
MODEL="chicago_model.txt"
OUTPUT="rnd_path_test_50k.txt"
CSV_FILE="rnd_path_test_50k_results.csv"

export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
/usr/bin/time ../../pathfinder --model "${MODEL}" --best-time --search-dist 0.25 --search-scale 0.1 --rnd-test 50000 > "${OUTPUT}"

# The following command disables the distance and scale optimization
# /usr/bin/time ../../pathfinder --model "${MODEL}" --best-time --search-dist -1 --search-scale -1 --rnd-test 50000 > "${OUTPUT}"

# Next extract the raw timings and number of nodes in path from the output
# to plot the violin plot and the histogram
echo "#nodes, time, trvl_time_mins, trvl_dist_miles, explored, haversine_dist_miles" > "${CSV_FILE}"
tail -n +3 rnd_path_test_50k.txt | grep -v "retry" | cut -d" " -f6,9,12,15,18,23 | tr -d ,\( | tr " " "," >> "${CSV_FILE}"

# Extract just the analysis times (in seconds) and number of nodes for
# plotting with R into seperate files
tail -n +2 "${CSV_FILE}" | cut -d"," -f2 > "times.txt"
tail -n +2 "${CSV_FILE}" | cut -d"," -f1 > "node_counts.txt"

# Plot the above two files using an R script
Rscript plot_times.R

# Plot correllogram using an R script
Rscript ../chicago/plot_trvl_time_correlation.R

# End of script

