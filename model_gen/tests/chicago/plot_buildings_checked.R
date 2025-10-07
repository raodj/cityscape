# This script assumes that the number of buildings checked has been  
# extracted to a file named buildings_checked.csv using the command below:
# grep "Assign " stats_job37646649_rank*.txt | cut -d" " -f20 > "buildings_checked.txt"

library(vioplot)

bldChecked <- scan("buildings_checked.txt")
summary(bldChecked)

pdf("blds_checked.pdf", width=2.25, height=4)
par(mai=c(0.1, 0.4, 0.1, 0.1))

boxplot_obj <- boxplot(bldChecked, log="y", col="olivedrab1", outcol="gray")
title(ylab="Number of Buildings Checked\n(log scale)", line=-2.25, font.lab=2)

text(1.3, 40000, sprintf("Mean: %.1f", mean(bldChecked)), cex=0.75)
text(1.3, 25000, sprintf("Median: %.1f", median(bldChecked)), cex=0.75)

# vioplot(bldChecked, names=c(""), col="gold")

dev.off()
print(paste("Number of outliers: ", length(boxplot_obj$out)))

# End of script

