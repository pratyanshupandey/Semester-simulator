simulator: simulator.h simulator.cpp main.cpp
	g++ -o simulator simulator.cpp main.cpp

psne_calculator: pne.cpp
	g++ -o psne_calculator pne.cpp

clean:
	rm simulator
	rm psne_calculator