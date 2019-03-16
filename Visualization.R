setwd("~/Documents/SSI/Cycloon-main/cycloon")
library(data.table)
d1 <- fread("64onebit3.5")
d1$ID <- seq.int(nrow(d1))
colnames(d1) <- c("Strength", "Time")

d2 <- fread("64onebit3.95")
d2$ID <- seq.int(nrow(d2))
colnames(d2) <- c("Strength", "Time")

d3 <- fread("64onebit3.8")
d3$ID <- seq.int(nrow(d3))
colnames(d3) <- c("Strength", "Time")

d4 <- fread("64onebit4")
d4$ID <- seq.int(nrow(d4))
colnames(d4) <- c("Strength", "Time")

maxlim <- max(max(d1$Strength), max(d2$Strength), max(d3$Strength), max(d4$Strength))*1.1

plot(Strength ~ Time, data=d1[1:50,], type='l', xlab="Time", ylab="Reported Signal Strength at f=1/4", main="Filter Length = 64, Actual f=1/3.5", ylim=c(0,maxlim))
plot(Strength ~ Time, data=d3[1:50,], type='l', xlab="Time", ylab="Reported Signal Strength at f=1/4", main="Filter Length = 64, Actual f=1/3.8", ylim=c(0,maxlim))
plot(Strength ~ Time, data=d2[1:50,], type='l', xlab="Time", ylab="Reported Signal Strength at f=1/4", main="Filter Length = 64, Actual f=1/3.95", ylim=c(0,maxlim))
plot(Strength ~ Time, data=d4[1:50,], type='l', xlab="Time", ylab="Reported Signal Strength at f=1/4", main="Filter Length = 64, Actual f=1/4", ylim=c(0,maxlim))

## More plots

onebit <- data.table()
for(f in c(3.5, 3.8, 3.9, 3.95, 4, 4.05, 4.1, 4.2, 4.5, 5, 6, 7,
           7.5, 7.8, 7.9, 7.95, 8, 8.05, 8.1, 8.2, 8.5, 9)) {
  dat <- fread(paste0("64onebit",f))
  yval <- mean(dat$V1)
  onebit <- rbind(onebit, cbind(f=f, strength=yval))
}
zerobit <- data.table()
for(f in c(3.5, 3.8, 3.9, 3.95, 4, 4.05, 4.1, 4.2, 4.5, 5, 6, 7,
            7.5, 7.8, 7.9, 7.95, 8, 8.05, 8.1, 8.2, 8.5, 9)) {
  dat <- fread(paste0("64zerobit",f))
  yval <- mean(dat$V1)
  zerobit <- rbind(zerobit, cbind(f=f, strength=yval))
} 

plot(strength~f, data=onebit, type='l', col='red', ylim=c(0, max(zerobit$strength)), xlab="1/f", ylab="Detected Strength", main="Filter Bandwidth for 64-bit Filter")
lines(strength~f, data=zerobit, type='l', col='blue')
legend("topleft", legend=c("One Bit", "Zero Bit"), col=c("red", "blue"), lty=1)

onebit <- data.table()
for(f in c(3.5, 3.8, 3.9, 3.95, 4, 4.05, 4.1, 4.2, 4.5, 5, 6, 7,
           7.5, 7.8, 7.9, 7.95, 8, 8.05, 8.1, 8.2, 8.5, 9)) {
  dat <- fread(paste0("128onebit",f))
  yval <- mean(dat$V1)
  onebit <- rbind(onebit, cbind(f=f, strength=yval))
}
zerobit <- data.table()
for(f in c(3.5, 3.8, 3.9, 3.95, 4, 4.05, 4.1, 4.2, 4.5, 5, 6, 7,
           7.5, 7.8, 7.9, 7.95, 8, 8.05, 8.1, 8.2, 8.5, 9)) {
  dat <- fread(paste0("128zerobit",f))
  yval <- mean(dat$V1)
  zerobit <- rbind(zerobit, cbind(f=f, strength=yval))
} 

plot(strength~f, data=onebit, type='l', col='red', ylim=c(0, max(zerobit$strength)), xlab="1/f", ylab="Detected Strength", main="Filter Bandwidth for 128-bit Filter")
lines(strength~f, data=zerobit, type='l', col='blue')
legend("topleft", legend=c("One Bit", "Zero Bit"), col=c("red", "blue"), lty=1)

dataaa <- data.table()
for(i in 1:31) {
  d <- fread(paste0("offset",i))
  names(d) <- c("y1","y2")
  dataaa <- rbind(dataaa, cbind(offset=i, d))
}
dataaa$ratio <- dataaa$y2/dataaa$y1
plot(y1 ~ offset, data=dataaa, type='l', col='red', ylim=c(0, max(dataaa$y2)))
lines(y2 ~ offset, data=dataaa, col='blue')
plot(log(ratio) ~ offset, data=dataaa, type='l')

