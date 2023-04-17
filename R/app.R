library(readr)

track <- "sardegna"
dat <- readr::read_csv("data/sardegna/data.csv")

plot(dat$x, dat$z)
