#include "structures.hpp"
#include "primitives.hpp"
#include <algorithm>
#include <numeric>

namespace SummarizedCat{

	IStructure::IStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass):absolutePosition(argAbsolutePosition),absoluteVelocity(argAbsoluteVelocity),mass(argMass){}

	DiscreteStructure::DiscreteStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass):IStructure(argAbsolutePosition, argAbsoluteVelocity, argMass){}

	StructureSummary::StructureSummary(const std::vector<IStructure> &argChildStructures){
		SpatialVector positionSum = std::accumulate(argChildStructures.begin(), argChildStructures.end(), SpatialVector(0,0,0), [](const IStructure a, const IStructure b){
			return a.absolutePosition*a.mass + b.absolutePosition*b.mass;
		});

		SpatialVector velocitySum = std::accumulate(argChildStructures.begin(), argChildStructures.end(), SpatialVector(0,0,0), [](const IStructure a, const IStructure b){
			return a.absoluteVelocity*a.mass + b.absoluteVelocity*b.mass;
		});

		scalar mass = std::accumulate(argChildStructures.begin(), argChildStructures.end(), 0, [](const IStructure a, const IStructure b){
			return a.mass + b.mass;
		});

		IStructure(positionSum / mass, velocitySum / mass, mass);
	}

	StructureCollection::StructureCollection(const unsigned int argMaxDetailDepth, const unsigned int argDimension):maxDetailDepth(argMaxDetailDepth),dimension(argDimension){}

	StructureCollection::StructureCollection(const std::vector<DiscreteStructure> &argChildStructures):StructureCollection(0,0){

		//TODO: Implement this
	}
}
