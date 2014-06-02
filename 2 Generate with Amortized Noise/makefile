SRC = main.cpp CPUtime.cpp ExponentialHash.cpp InfiniteAmortizedNoise2D.cpp MurmurHash3.cpp TerrainGenerator.cpp
EXE = pack

all: $(SRC) $(EXE)

$(EXE): $(SRC)
	g++ -O3 -ffast-math -o $(EXE) $(SRC)

cleanup: 
	rm -f  $(EXE) 
