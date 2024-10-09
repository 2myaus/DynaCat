#include "primitives.hpp"

#include <algorithm>

namespace SummarizedCat{
	scalar scalar::operator+(const scalar& o) const{
		return scalar(o.m + m);
	}

	scalar scalar::operator-() const{
		return scalar(-m);
	}

	scalar scalar::operator-(const scalar &o) const{
		return *this + (-o);
	}

	scalar scalar::operator*(const scalar &o) const{
		return scalar(m * o.m);
	}

	scalar scalar::operator/(const scalar &o) const{
		return scalar(m / o.m);
	}

	scalar::scalar(const double magnitude): m(magnitude){};
	scalar::scalar(const scalar &rhs): scalar(rhs.m){};

	scalar::operator double() const{
		return m;
	}

	SpatialVector SpatialVector::operator+(const SpatialVector &o) const{
		const SpatialVector *greaterD = (this);
		const SpatialVector *lesserD = &o;
		if(this->dimension() < o.dimension()){
			greaterD = &o;
			lesserD = this;
		}


		std::vector<scalar> components(greaterD->components);
		std::transform(lesserD->components.begin(), lesserD->components.end(), greaterD->components.begin(), components.begin(), [&](const scalar &lesserComp, const scalar &greaterComp){
			return lesserComp +  greaterComp;
		});

		return SpatialVector(components);
	};

	SpatialVector SpatialVector::operator-() const{
		std::vector<scalar> components;
		components.reserve(this->dimension());
		std::transform(this->components.begin(), this->components.end(), components.begin(), [](const scalar component){
			return -component;
		});

		return SpatialVector(components);
	}

	SpatialVector SpatialVector::operator-(const SpatialVector &o) const{
		// return (*this)+(-o); //Inefficient!
		const SpatialVector *greaterD = (this);
		const SpatialVector *lesserD = &o;
		if(this->dimension() < o.dimension()){
			greaterD = &o;
			lesserD = this;
		}


		std::vector<scalar> components(greaterD->components);
		std::transform(lesserD->components.begin(), lesserD->components.end(), greaterD->components.begin(), components.begin(), [](const scalar &lesserComp, const scalar &greaterComp){
			return lesserComp - greaterComp;
		});

		return SpatialVector(components);
	};

	SpatialVector SpatialVector::operator*(const scalar &o) const{
		std::vector<scalar> components;
		components.reserve(this->dimension());
		std::transform(this->components.begin(), this->components.end(), components.begin(), [&](const scalar &s){ return o*s; });

		return SpatialVector(components);
	}

	SpatialVector SpatialVector::operator/(const scalar &o) const{
		std::vector<scalar> components;
		components.reserve(this->dimension());
		std::transform(this->components.begin(), this->components.end(), components.begin(), [&](const scalar &s){ return s/o; });

		return SpatialVector(components);
	}

	SpatialVector::SpatialVector(const SpatialVector &rhs):SpatialVector(rhs.components){}
	SpatialVector::SpatialVector(const std::vector<scalar> &v):components(v){}
	SpatialVector::SpatialVector(){}

	unsigned int SpatialVector::dimension() const{
		return this->components.size();
	}
}
