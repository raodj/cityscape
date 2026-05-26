# Plot the distribution of traffic density across roads, using the
# per-way visit counts produced by WayRiskAnalyzer.
#
# Input  : ways_summary_schedules.tsv (column 3 = Kind, column 9 = #Visits)
# Output : way_density_hist.pdf
#
# Run as : Rscript plot_way_density.R

inputFile  <- "ways_summary_schedules.tsv"
outputFile <- "way_density_hist.pdf"

# Way names can contain characters (#, embedded quote-like bytes) that
# trip up read.delim row splitting. Pre-extract just the two columns
# we need with awk so parsing is unambiguous.
awkCmd <- sprintf("awk -F'\\t' 'BEGIN{OFS=\"\\t\"} !/^#/ {print $3, $9}' %s",
                  shQuote(inputFile))
ways <- read.delim(pipe(awkCmd), header = FALSE, sep = "\t",
                   quote = "", stringsAsFactors = FALSE)

kind   <- ways[[1]]
visits <- as.numeric(ways[[2]])

# Drop rows where #Visits could not be parsed (defensive).
ok     <- !is.na(visits)
kind   <- kind[ok]
visits <- visits[ok]

cat(sprintf("Loaded %d ways from %s\n", length(visits), inputFile))

pdf(outputFile, width = 8, height = 6)

# --- Page 1: overall histogram (linear scale) -----------------------
# The #Visits distribution is extremely heavy-tailed (max can be ~1000x
# the median), so a raw linear histogram lumps essentially every way
# into the leftmost bin. Clip to the 99th percentile for the linear
# view; the full range is still visible on page 2 (log scale).
par(mar = c(4.5, 4.2, 3, 1))
clipHi <- as.numeric(quantile(visits, 0.99))
visitsClip <- visits[visits <= clipHi]
nClipped <- length(visits) - length(visitsClip)
h <- hist(visitsClip, breaks = 50, col = "cornflowerblue",
          xlab = "Traffic density (#Visits per way)",
          main = sprintf("Road traffic density distribution (clipped at 99th pct = %.0f)",
                         clipHi))

# Annotate with summary stats (mirrors style of plot_times.R).
xPos <- max(h$breaks) * 0.55
yMax <- max(h$counts)
yStep <- yMax * 0.06
text(xPos, yMax - 0 * yStep, sprintf("N    : %d",      length(visits)), adj = 0)
text(xPos, yMax - 1 * yStep, sprintf("Mean : %.2f",    mean(visits)),   adj = 0)
text(xPos, yMax - 2 * yStep, sprintf("SD   : %.2f",    sd(visits)),     adj = 0)
text(xPos, yMax - 3 * yStep, sprintf("Median: %.0f",   median(visits)), adj = 0)
text(xPos, yMax - 4 * yStep, sprintf("Max  : %d",      max(visits)),    adj = 0)
text(xPos, yMax - 5 * yStep, sprintf("Min  : %d",      min(visits)),    adj = 0)
text(xPos, yMax - 6 * yStep,
     sprintf("Clipped: %d ways > %.0f", nClipped, clipHi), adj = 0)

# --- Page 2: log-scaled view (heavy tail) ---------------------------
logVisits <- log10(visits + 1)
hist(logVisits, breaks = 50, col = "cornflowerblue",
     xlab = expression(log[10](`#Visits` + 1)),
     main = "Road traffic density (log scale)")

# --- Page 3: faceted by road kind -----------------------------------
kinds <- sort(unique(kind))
# Keep only kinds with enough ways to plot meaningfully.
kindCounts <- table(kind)
kinds <- names(kindCounts)[kindCounts >= 5]
kinds <- sort(kinds)

nKinds <- length(kinds)
if (nKinds > 0) {
  nCols <- if (nKinds >= 6) 3 else 2
  nRows <- ceiling(nKinds / nCols)
  par(mfrow = c(nRows, nCols), mar = c(3.5, 3.5, 2, 0.8),
      mgp = c(2, 0.7, 0))
  for (k in kinds) {
    v <- visits[kind == k]
    # Clip per-panel to the kind's own 99th percentile so a single
    # outlier way doesn't squash the rest of the panel against the
    # y-axis. Fall back to the raw vector if the kind has <100 ways
    # (quantile clipping is too aggressive with small samples).
    if (length(v) >= 100) {
      vHi <- as.numeric(quantile(v, 0.99))
      vClip <- v[v <= vHi]
      mainLbl <- sprintf("%s (N=%d, x <= %.0f)", k, length(v), vHi)
    } else {
      vClip <- v
      mainLbl <- sprintf("%s (N=%d)", k, length(v))
    }
    hist(vClip, breaks = 30, col = "darkorange",
         xlab = "#Visits", main = mainLbl)
  }
  par(mfrow = c(1, 1))
} else {
  plot.new()
  title("No road kinds with >=5 ways")
}

dev.off()
cat(sprintf("Wrote %s\n", outputFile))
