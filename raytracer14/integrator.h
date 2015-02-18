#include "cmmn.h"
#include "scene.h"

namespace raytracer14
{
	struct integrator
	{
		virtual vec3 Li(const class renderer* rdr, const ray& r, const scene& scene, const hit_record& hr) const = 0;
	};

	struct whitted_integrator : public integrator
	{
		uint max_depth;
		whitted_integrator(uint mdph) : max_depth(mdph) {}
		vec3 Li(const class renderer* rdr, const ray& r, const scene& scene, const hit_record& hr) const override;
	};
}