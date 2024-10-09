#include "primitives.hpp"
#include "structures.hpp"

#include <optional>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>

using namespace SummarizedCat;

int main(){
	printf("hw\n");

	std::vector<DiscreteStructure> particles;

	for(int i = 0; i < 100000; i++){
		particles.push_back(DiscreteStructure(SpatialVector({((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX))}), SpatialVector({((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX))}), 1));
	}

	printf("Ready!\n");
	std::cin.ignore();
	printf("Starting!\n");

	auto begin = std::chrono::steady_clock::now();
	StructureCollection collection(particles);
	auto end = std::chrono::steady_clock::now();

	printf("Took %ld ns!", std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count());

	return 0;
}
