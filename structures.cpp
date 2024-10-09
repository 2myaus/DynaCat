#include "structures.hpp"
#include "primitives.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace SummarizedCat{

	IStructure::IStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass):absolutePosition(argAbsolutePosition),absoluteVelocity(argAbsoluteVelocity),mass(argMass){}

	DiscreteStructure::DiscreteStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass):IStructure(argAbsolutePosition, argAbsoluteVelocity, argMass){}

	StructureSummary::StructureSummary(const std::vector<IStructure> &argChildStructures){
		childStructures.reserve(argChildStructures.size());

		std::transform(argChildStructures.begin(), argChildStructures.end(), childStructures.begin(), [](IStructure &struc){ return &struc; } );

		SpatialVector positionSum = std::accumulate(argChildStructures.begin(), argChildStructures.end(), SpatialVector(), [](const IStructure a, const IStructure b){
			return a.absolutePosition*a.mass + b.absolutePosition*b.mass;
		});

		SpatialVector velocitySum = std::accumulate(argChildStructures.begin(), argChildStructures.end(), SpatialVector(), [](const IStructure a, const IStructure b){
			return a.absoluteVelocity*a.mass + b.absoluteVelocity*b.mass;
		});

		scalar totalMass = std::accumulate(argChildStructures.begin(), argChildStructures.end(), 0, [](const IStructure a, const IStructure b){
			return a.mass + b.mass;
		});

		absolutePosition = positionSum / mass;
		absoluteVelocity = velocitySum / mass;
		mass = totalMass;
	}

	StructureCollection::StructureCollection():maxDetailDepth(0),dimension(0),lowestPosition(0),highestPosition(0){}

	StructureCollection::StructureCollection(const std::vector<DiscreteStructure> &argChildStructures):StructureCollection(){
		maxDetailDepth = 15; //TODO: Calculate this based on arguments

		std::for_each(argChildStructures.begin(), argChildStructures.end(), [&](DiscreteStructure childStructure){
			for(unsigned int d=0; d<childStructure.absolutePosition.dimension();d++){
				const scalar &component = childStructure.absolutePosition.components[d];
				if(component < lowestPosition){ lowestPosition = component; }
				else if(component > highestPosition){ highestPosition = component; }
			}

			if(childStructure.absolutePosition.dimension() > dimension){ dimension = childStructure.absolutePosition.dimension(); }
			if(childStructure.absoluteVelocity.dimension() > dimension) {dimension = childStructure.absoluteVelocity.dimension(); }
		}); //Surely there is a faster way to do this?
		
		scalar sideLength = highestPosition-lowestPosition;

		std::vector<std::vector<StructureSummary>> detailMap(maxDetailDepth+1);

		for(int detailDepth = maxDetailDepth; detailDepth > 0; detailDepth--){
			unsigned int gridWidth = pow(2, detailDepth);
			detailMap[detailDepth].reserve(pow(gridWidth, dimension));
			
			if(detailDepth == maxDetailDepth){
				std::for_each(argChildStructures.begin(), argChildStructures.end(), [&](DiscreteStructure childStructure){
					unsigned int gridIdx = 0;
					for(unsigned int d=0;d<dimension;d++){
						unsigned int dimensionalGridPos = floor((scalar)gridWidth * ((childStructure.absolutePosition.components[d]-lowestPosition)/(sideLength)));
						gridIdx += dimensionalGridPos*pow(gridWidth, d);
					}
					detailMap[detailDepth][gridIdx].childStructures.push_back(&childStructure);
				});
				continue;
			}
			
			//TODO: This can be multithreaded
			for(unsigned int gridIdx = 0; gridIdx < pow(gridWidth,dimension); gridIdx++){
				const SpatialVector gridPos = unflattenVecInSpace(gridIdx, gridWidth);
				const SpatialVector dGridPos = gridPos * 2;
				const unsigned int dGridIdx = flattenVecInSpace(dGridPos, gridWidth*2);
				std::vector<unsigned int> childIdxs(pow(2, dimension));

				childIdxs.push_back(dGridIdx);
				for(unsigned int d=0; d<dimension;d++){
					std::for_each(childIdxs.begin(), childIdxs.end(), [&](unsigned int childIdx){
						childIdxs.push_back(childIdx+pow(gridWidth,d));
					});
				}

				std::vector<IStructure> childSummaries(pow(2,dimension));
				std::transform(childIdxs.begin(), childIdxs.end(), childSummaries.begin(), [&](unsigned int childIdx){
					return detailMap[detailDepth+1][childIdx];
				});

				detailMap[detailDepth][gridIdx] = StructureSummary(childSummaries);
			}
		}
	}
	const unsigned int StructureCollection::flattenVecInSpace(const SpatialVector argSpatialPosition, const unsigned int gridWidth){
		unsigned int flatPos = 0;
		for(unsigned int d = 0; d < argSpatialPosition.dimension(); d++){
			flatPos += (int)(argSpatialPosition.components.at(d)) * pow(gridWidth, d);
		}

		return flatPos;
	}

	const SpatialVector unflattenVecInSpace(const unsigned int flattenedPosition, const unsigned int gridWidth){
		unsigned int currentFlatPos = flattenedPosition;
		SpatialVector vec;

		unsigned int d = 0;
		while(flattenedPosition > 0){
			unsigned int component = currentFlatPos % gridWidth;
			vec.components.at(d) = component;
			currentFlatPos -= component;
			currentFlatPos /= gridWidth;
			d++;
		}

		return vec;
	}
}
