SRC = main.cpp Random.cpp
EXE = exponential

all: $(SRC) $(EXE)

$(EXE): $(SRC)
	g++ -O3 -ffast-math -o $(EXE) $(SRC)

cleanup: 
	rm -f  $(EXE) 
