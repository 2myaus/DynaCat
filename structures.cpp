#include "structures.hpp"
#include "primitives.hpp"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>

namespace SummarizedCat{

	IStructure::IStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass):absolutePosition(argAbsolutePosition),absoluteVelocity(argAbsoluteVelocity),mass(argMass){}
	IStructure::IStructure():mass(0){}

	IStructure::~IStructure() = default;

	DiscreteStructure::DiscreteStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass):IStructure(argAbsolutePosition, argAbsoluteVelocity, argMass){}

	StructureSummary::StructureSummary() = default;
	StructureSummary::StructureSummary(const std::vector<IStructure> &argChildStructures){

		SpatialVector positionSum = std::accumulate(argChildStructures.begin(), argChildStructures.end(), SpatialVector(), [&](const SpatialVector &a, const IStructure &b){
			return a + b.absolutePosition*b.mass;
		});

		SpatialVector velocitySum = std::accumulate(argChildStructures.begin(), argChildStructures.end(), SpatialVector(), [&](const SpatialVector &a, const IStructure &b){
			return a + b.absoluteVelocity*b.mass;
		});

		scalar totalMass = std::accumulate(argChildStructures.begin(), argChildStructures.end(), 0, [&](const scalar &a, const IStructure &b){
			return a + b.mass;
		});

		absolutePosition = positionSum / mass;
		absoluteVelocity = velocitySum / mass;
		mass = totalMass;
	}

	StructureCollection::StructureCollection():maxDetailDepth(0),dimension(0),lowestPosition(0),highestPosition(0){}

	StructureCollection::StructureCollection(const std::vector<DiscreteStructure> &argChildStructures):StructureCollection(){
		maxDetailDepth = 5; //TODO: Calculate this based on arguments

		std::for_each(argChildStructures.begin(), argChildStructures.end(), [&](const DiscreteStructure &childStructure){
			for(unsigned int d=0; d<childStructure.absolutePosition.dimension();d++){
				const scalar &component = childStructure.absolutePosition.components[d];
				if(component < lowestPosition){ lowestPosition = component; }
				else if(component > highestPosition){ highestPosition = component; }
			}

			if(childStructure.absolutePosition.dimension() > dimension){ dimension = childStructure.absolutePosition.dimension(); }
			if(childStructure.absoluteVelocity.dimension() > dimension) {dimension = childStructure.absoluteVelocity.dimension(); }
		}); //Surely there is a faster way to do this?
		
		scalar sideLength = highestPosition-lowestPosition;

		std::vector<std::vector<StructureSummary>> detailMap;
		detailMap.resize(maxDetailDepth+1);

		for(int detailDepth = maxDetailDepth; detailDepth > 0; detailDepth--){
			unsigned int gridWidth = pow(2, detailDepth);
			size_t reserve = (double)pow(gridWidth, dimension+1);
			detailMap[detailDepth].resize(reserve); //Doing this without casts gives wrong values!
			
			if(detailDepth == maxDetailDepth){
				std::for_each(argChildStructures.begin(), argChildStructures.end(), [&](const DiscreteStructure &childStructure){
					unsigned int gridIdx = 0;
					for(unsigned int d=0;d<dimension;d++){
						unsigned int dimensionalGridPos = floor((scalar)gridWidth * ((childStructure.absolutePosition.components[d]-lowestPosition)/(sideLength))); //TODO: Increase performance here!
						gridIdx += dimensionalGridPos*pow(gridWidth, d);
					}
					detailMap.at(detailDepth).at(gridIdx).childStructures.push_back(&childStructure);
				});
				continue;
			}
			
			//TODO: This can be multithreaded
			for(unsigned int gridIdx = 0; gridIdx < (double)pow(gridWidth,dimension); gridIdx++){
				const SpatialVector gridPos = StructureCollection::unflattenVecInSpace(gridIdx, gridWidth);
				const SpatialVector dGridPos = gridPos * 2;
				const unsigned int dGridIdx = StructureCollection::flattenVecInSpace(dGridPos, gridWidth*2);
				std::vector<unsigned int> childIdxs;
				childIdxs.reserve((double)pow(2,dimension));

				childIdxs.push_back(dGridIdx);
				for(unsigned int d=0; d<dimension;d++){
					std::for_each(childIdxs.begin(), childIdxs.end(), [&](const unsigned int &childIdx){
						childIdxs.push_back(childIdx+pow(gridWidth,d));
					});
				}

				std::vector<IStructure> childSummaries;
				childSummaries.reserve((double)pow(2,dimension));
				std::transform(childIdxs.begin(), childIdxs.end(), std::back_inserter(childSummaries), [&](const unsigned int &childIdx){
					return detailMap.at(detailDepth+1).at(childIdx);
				});

				detailMap[detailDepth][gridIdx] = StructureSummary(childSummaries);
			}
		}
	}

	const StructureSummary &StructureCollection::getSummary(const unsigned int detailDepth, const SpatialVector &gridPosition){
		//TODO
	}

	const unsigned int StructureCollection::flattenVecInSpace(const SpatialVector argSpatialPosition, const unsigned int gridWidth){
		unsigned int flatPos = 0;
		for(unsigned int d = 0; d < argSpatialPosition.dimension(); d++){
			flatPos += (int)(argSpatialPosition.components.at(d)) * pow(gridWidth, d);
		}

		return flatPos;
	}

	const SpatialVector StructureCollection::unflattenVecInSpace(const unsigned int flattenedPosition, const unsigned int gridWidth){
		/*unsigned*/ int currentFlatPos = flattenedPosition; //This shouldn't be an issue but keep it signed just in case
		SpatialVector vec;

		unsigned int d = 0;
		while(currentFlatPos > 0){
			unsigned int component = currentFlatPos % gridWidth;
			vec.components.push_back(component);
			currentFlatPos -= component;
			currentFlatPos /= gridWidth;
			d++;
		}

		return vec;
	}
}
