#include "structures.hpp"
#include "primitives.hpp"
#include <algorithm>
#include <cmath>
#include <execution>
#include <mutex>
#include <numeric>
#include <shared_mutex>
#include <stdio.h>

namespace SummarizedCat{

	IStructure::IStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass):absolutePosition(argAbsolutePosition),absoluteVelocity(argAbsoluteVelocity),mass(argMass){}
	IStructure::IStructure():mass(0){}

	IStructure::~IStructure() = default;

	DiscreteStructure::DiscreteStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass):IStructure(argAbsolutePosition, argAbsoluteVelocity, argMass){}

	StructureSummary::StructureSummary(){};
	StructureSummary::StructureSummary(const StructureSummary &o): StructureSummary(o.childStructures){}
	StructureSummary::StructureSummary(const std::vector<const IStructure*> &argChildStructures){

		SpatialVector positionSum = std::accumulate(argChildStructures.begin(), argChildStructures.end(), SpatialVector(), [&](const SpatialVector &a, const IStructure *b){ //dies inside
			return a + b->absolutePosition*b->mass;
		});

		SpatialVector velocitySum = std::accumulate(argChildStructures.begin(), argChildStructures.end(), SpatialVector(), [&](const SpatialVector &a, const IStructure *b){
			return a + b->absoluteVelocity*b->mass;
		});

		scalar totalMass = std::accumulate(argChildStructures.begin(), argChildStructures.end(), 0, [&](const scalar &a, const IStructure *b){
			return a + b->mass;
		});

		absolutePosition = positionSum / mass;
		absoluteVelocity = velocitySum / mass;
		mass = totalMass;
	}
	StructureSummary::StructureSummary(const StructureSummary &old, const IStructure *append) : childStructures(old.childStructures.size() + 1){
		mass = old.mass + append->mass;
		absolutePosition = old.absolutePosition + append->absolutePosition*(append->mass/old.mass);
		absoluteVelocity = old.absoluteVelocity + append->absoluteVelocity*(append->mass/old.mass);

		childStructures.insert(childStructures.end(), old.childStructures.begin(), old.childStructures.end());
		childStructures.push_back(append);
	}

	StructureCollection::StructureCollection():maxDetailDepth(0),dimension(0),lowestPosition(0),highestPosition(0){}

	StructureCollection::StructureCollection(std::vector<DiscreteStructure> &argChildStructures):StructureCollection(){
		maxDetailDepth = 5; //TODO: Calculate this based on arguments

		std::for_each(argChildStructures.begin(), argChildStructures.end(), [&](DiscreteStructure &childStructure){
			for(unsigned int d=0; d<childStructure.absolutePosition.dimension();d++){
				const scalar &component = childStructure.absolutePosition.components[d];
				if(component < lowestPosition){
					lowestPosition = component;
				}
				else if(component > highestPosition){ highestPosition = component; }
			}

			if(childStructure.absolutePosition.dimension() > dimension){ dimension = childStructure.absolutePosition.dimension(); }
			if(childStructure.absoluteVelocity.dimension() > dimension){ dimension = childStructure.absoluteVelocity.dimension(); }
		}); //Surely there is a faster way to do this?

		scalar sideLength = highestPosition-lowestPosition;

		detailMap.resize(maxDetailDepth+1);
		const unsigned int highestReserve = (double)pow(2,maxDetailDepth*dimension);
		std::vector<std::mutex> mutexMap(highestReserve);

		for(int detailDepth = maxDetailDepth; detailDepth > 0; detailDepth--){
			unsigned int gridWidth = pow(2, detailDepth);
			unsigned int reserve = (double)pow(gridWidth, dimension);
			detailMap[detailDepth].resize(reserve); //TODO: Fix this giving an error with high detail depths?
			std::vector<std::pair<std::vector<const IStructure*>, unsigned int>> writtenChildLists;
			writtenChildLists.resize(reserve);
			
			if(detailDepth == maxDetailDepth){
				std::for_each(std::execution::par,argChildStructures.begin(), argChildStructures.end(), [&](DiscreteStructure &childStructure){ //TODO: Manually divide execution here because SIMD is slow
					unsigned int gridIdx = 0;
					for(unsigned int d=0;d<dimension;d++){
						unsigned int dimensionalGridPos;
						if(childStructure.absolutePosition.components[d] >= highestPosition){dimensionalGridPos = gridWidth-1;}
						else{ dimensionalGridPos = floor((scalar)gridWidth * ((childStructure.absolutePosition.components[d]-lowestPosition)/(sideLength))); } //TODO: Increase performance here!
						gridIdx += dimensionalGridPos*pow(gridWidth, d);
					}
					
					std::optional<StructureSummary> &current = detailMap.at(detailDepth).at(gridIdx);
					std::lock_guard<std::mutex> posLock(mutexMap[gridIdx]);
					writtenChildLists[gridIdx].first.push_back(&childStructure);
				});

				std::for_each(std::execution::par, writtenChildLists.begin(), writtenChildLists.end(), [&](const std::pair< std::vector<const IStructure*>, unsigned int > &cachedChildren){
					detailMap[detailDepth][cachedChildren.second].emplace(cachedChildren.first);
				});
				continue;
			}
			
			//TODO: use SIMD here
#pragma openmp parallel for
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

				std::vector<const IStructure*> childSummaries;
				childSummaries.reserve((double)pow(2,dimension));
				std::for_each(std::execution::seq, childIdxs.begin(), childIdxs.end(), [&](const unsigned int childIdx){
					if(!detailMap.at(detailDepth+1).at(childIdx).has_value()){return;}
					childSummaries.push_back(&detailMap[detailDepth+1][childIdx].value());
				});

				//Mutex is not necessary here because of 1:1 calling
				detailMap.at(detailDepth).at(gridIdx).emplace(childSummaries);
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
