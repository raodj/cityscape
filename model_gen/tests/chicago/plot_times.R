# This script assumes that raw runtimes have been extracted into a
#  file named times.txt.  Run this script as: Rscript plot_times.R

# Uncomment the line below to install vioplot once
# install.packages("vioplot", repos = "http://cran.us.r-project.org")


library(vioplot)
times <- scan("times.txt")
timesMS <- times * 1000

pdf("path_find_time.pdf", width=3, height=3)
par(mai=c(0.1, 0.4, 0.3, 0.1))
#vioplot(timesMS, names=c("Time (milliseconds)"), col="gold")
vioplot(timesMS, names=c(""), col="gold")
title("Route computing times", font.lab=2)
title(ylab="Milliseconds", line=-1.25, font.lab=2)

# Include mean and sd in plot
text(1.10, 210, sprintf("Mean: %.2f ms", mean(timesMS)), adj=0, cex=0.8);
text(1.10, 190, sprintf("SD: %.2f ms",   sd(timesMS)),   adj=0, cex=0.8);
text(1.10, 170, sprintf("Max : %.2f ms", max(timesMS)),  adj=0, cex=0.8);
text(1.10, 150, sprintf("Min : %.2f ms", min(timesMS)),  adj=0, cex=0.8);

# Finsihed
dev.off()

# Now plot histogram of number of nodes in path
pdf("path_hist.pdf")
nodes <- scan("node_counts.txt")
hist(nodes, col="cornflowerblue",  xlab="#Nodes in path",
     main="Number of nodes in path")

# Include mean and sd in plot
text(1000, 5700, sprintf("Mean: %.2f", mean(nodes)), adj=0);
text(1000, 5400, sprintf("SD: %.2f",   sd(nodes)),   adj=0);
text(1000, 5100, sprintf("Max : %d", max(nodes)),  adj=0);
text(1000, 4800, sprintf("Min : %d", min(nodes)),  adj=0);

dev.off();
