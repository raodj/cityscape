#!/bin/bash

# Model file with person schedules
ModelFile="chicago_model_with_schedules_two.txt"

/usr/bin/time ../../riskanalyzer \
  --model "${ModelFile}" \
  --best-time \
  --node-summary "nodes_summary_schedules.tsv" \
  --way-summary "ways_summary_schedules.tsv" \
  2>"error_logs_schedules.txt"

# Generate heatmap figures (requires shape files)
ShapeFile="boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp"
DbfFile="boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf"

# Traffic volume heatmap (column 8)
../../drawshapes \
  --shape "${ShapeFile}" \
  --dbf "${DbfFile}" \
  --xfig "heatmap_visits.fig" \
  --scale 16384000 \
  --way-risk-tsv "ways_summary_schedules.tsv" \
  --heatmap-col 8

# Histogram of traffic density distribution across ways
Rscript plot_way_density.R
