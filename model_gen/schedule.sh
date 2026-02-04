#!/bin/bash
#SBATCH --job-name=schedule_generator
#SBATCH --time=48:00:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=32G
#SBATCH --mail-user=yourusername@miamioh.edu
#SBATCH --mail-type=END,FAIL
#SBATCH -A PMIU0110

./schedule_generator --shapes chicago/boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp \
                     --dbfs chicago/boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf \
                     --xfig outputTest.fig \
                     --model chicago/chicago_model.txt \
                     --out-trvl-est trvl_est.txt \
                     --lm-num-samples 5000