library(data.table)
setwd("~/Documents/SSI/Cycloon") # change to current working directory
iridium <- fread("iridium.csv")
transmissions <- fread("transmissions.csv")

setkey(iridium, id)
setkey(transmissions, id)
joined <- merge(iridium, transmissions, all=TRUE)
fwrite(joined, "joinedData.csv")