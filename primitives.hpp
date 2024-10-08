#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include <vector>
namespace SummarizedCat{
	class scalar{ //Single-dimension magnitude
		double m;
		public:
			scalar operator+(const scalar &o) const;
			scalar operator-() const;
			scalar operator-(const scalar &o) const;
			scalar operator*(const scalar &o) const;
			scalar operator/(const scalar &o) const;

			scalar(const double magnitude);
			scalar(const scalar &rhs);

			operator double() const;
	};

	class SpatialVector{ //Multi-dimension set of magnitudes (magnitude and direction)
		public:
			std::vector<scalar> components;

			SpatialVector operator+(const SpatialVector &o) const;
			SpatialVector operator-() const;
			SpatialVector operator-(const SpatialVector &o) const;
			SpatialVector operator*(const scalar &rhs) const;
			SpatialVector operator/(const scalar &o) const;

			SpatialVector(const scalar argX, const scalar argY, const scalar argZ);
			SpatialVector(const SpatialVector &rhs);
			SpatialVector(std::vector<scalar>);

			scalar squaredMagnitude() const;
			scalar dot(const SpatialVector &o) const;
			SpatialVector cross(const SpatialVector &o) const;
			unsigned int dimension() const; // Highest dimension component (0,0,1) -> 3
	};
}

#endif
