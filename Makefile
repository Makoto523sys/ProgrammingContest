CC	:=	gcc
FILE	:= solve
CFLAGS	:= -o $(FILE)
LINKS	:= -lm
TSP	:= in_data/att532.tsp
PROGRAM	:= nn.c
CHECKER	:= ./cv
OUTFILE	:= tour.dat

all:
	$(CC) $(CFLAGS) $(PROGRAM) $(LINKS)
	./$(FILE) $(TSP)
	$(CHECKER) $(OUTFILE) $(TSP)
