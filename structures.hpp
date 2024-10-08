#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include "primitives.hpp"
#include <vector>
#include <optional>

namespace SummarizedCat{
	class IStructure{
		private:
			
		public:
			virtual ~IStructure();
			
			const SpatialVector absolutePosition;
			const SpatialVector absoluteVelocity;
			const scalar mass;

			IStructure();
			IStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass);
	};

	class DiscreteStructure : virtual public IStructure{
		private:
		public:
			~DiscreteStructure();
			DiscreteStructure(const SpatialVector argAbsolutePosition, const SpatialVector argAbsoluteVelocity, const scalar argMass);
	};

	class StructureSummary : virtual public IStructure{ // absolutePosition represents center of mass, absoluteVelocity represents average velocity, mass represents total mass
		private:
		public:
			std::optional<std::vector<IStructure*>> childStructures;

			StructureSummary(const std::vector<IStructure> &argChildStructures);
	};

	class StructureCollection : virtual public IStructure{
		private:
			const std::vector<std::vector<StructureSummary>> detailMap; // Mipmap-like representation of existing structures on different detail levels (detaillevel-spatialposition)
				StructureCollection(const std::vector<DiscreteStructure> &argChildStructures); //Generate a collection describing the given discrete structures
		public:
			const unsigned int maxDetailDepth;
			const unsigned int dimension;

			StructureCollection(const unsigned int argMaxDetailDepth, const unsigned int argDimension);
			const StructureSummary getSummary(const unsigned int detailDepth, const unsigned int x, const unsigned int y);
	};
}

#endif
