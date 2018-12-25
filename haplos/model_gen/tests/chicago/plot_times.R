# This script assumes that raw runtimes have been extracted into a
#  file named times.txt.  Run this script as: Rscript plot_times.R

library(vioplot)
times <- scan("times.txt")
timesMS <- times * 1000

pdf("path_find_time.pdf")
vioplot(timesMS, names=c("Time (milliseconds)"), col="gold")
title("Distribution of route computing times", ylab="Path finding time (milliseconds)", font.lab=2)

# Include mean and sd in plot
text(0.5, 330, sprintf("Mean: %.2f ms", mean(timesMS)), adj=0);
text(0.5, 310, sprintf("SD: %.2f ms",   sd(timesMS)),   adj=0);
text(0.5, 290, sprintf("Max : %.2f ms", max(timesMS)),  adj=0);
text(0.5, 270, sprintf("Min : %.2f ms", min(timesMS)),  adj=0);

# Finsihed
dev.off()

# Now plot histogram of number of nodes in path
pdf("path_hist.pdf")
nodes <- scan("node_counts.txt")
hist(nodes, col="cornflowerblue",  xlab="#Nodes in path",
     main="Number of nodes in path")

# Include mean and sd in plot
text(900, 8000, sprintf("Mean: %.2f", mean(nodes)), adj=0);
text(900, 7600, sprintf("SD: %.2f",   sd(nodes)),   adj=0);
text(900, 7200, sprintf("Max : %d", max(nodes)),  adj=0);
text(900, 6800, sprintf("Min : %d", min(nodes)),  adj=0);

dev.off();
