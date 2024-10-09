#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include "primitives.hpp"
#include <vector>

namespace SummarizedCat{
	class IStructure{
		private:
			
		public:
			virtual ~IStructure();
			
			SpatialVector absolutePosition;
			SpatialVector absoluteVelocity;
			scalar mass;

			IStructure();
			IStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass);
	};

	class DiscreteStructure : virtual public IStructure{
		private:
		public:
			DiscreteStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass);
	};

	class StructureSummary : virtual public IStructure{ // absolutePosition represents center of mass, absoluteVelocity represents average velocity, mass represents total mass
		private:
		public:
			std::vector<IStructure*> childStructures;

			StructureSummary(const std::vector<IStructure> &argChildStructures);
	};

	class StructureCollection : virtual public IStructure{
		private:
			std::vector<std::vector<StructureSummary>> detailMap; // Mipmap-like representation of existing structures on different detail levels (detaillevel-spatialposition)
			StructureCollection(const std::vector<DiscreteStructure> &argChildStructures); //Generate a collection describing the given discrete structures
		public:
			unsigned int maxDetailDepth;
			scalar lowestPosition; // Lowest position component in any dimension
			scalar highestPosition;
			unsigned int dimension;

			StructureCollection();
			const StructureSummary &getSummary(const unsigned int detailDepth, const SpatialVector &gridPosition);

			static const unsigned int flattenVecInSpace(const SpatialVector argSpatialPosition, const unsigned int gridWidth);
			static const SpatialVector unflattenVecInSpace(const unsigned int flattenedPosition, const unsigned int gridWidth);
	};
}

#endif
