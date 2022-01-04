default: toyVM

toyVM: toyVM.cpp
	c++ --std=c++11 toyVM.cpp -o toyVM

clean:
	-rm toyVM

