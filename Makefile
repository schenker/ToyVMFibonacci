default: toyVM

toyVM: toyVM.cpp
	c++ --std=c++11 toyVM.cpp -O3 -Wall -o toyVM

clean:
	-rm toyVM

