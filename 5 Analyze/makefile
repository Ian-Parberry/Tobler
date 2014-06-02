SRC = main.cpp CPUtime.cpp
EXE = exponential

all: $(SRC) $(EXE)

$(EXE): $(SRC)
	g++ -m64 -O3 -ffast-math -o $(EXE) $(SRC)

cleanup: 
	rm -f  $(EXE) 
