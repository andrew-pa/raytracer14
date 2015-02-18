#include "integrator.h"

namespace raytracer14
{
/*	vec3 whitted_integrator::Li(const class renderer* rdr, const ray& r, const scene& scene, const hit_record& hr) const
	{
		auto bsdf = hr.surf->mat->bsdf(hr);
		auto p = r(hr.t);
		auto n = hr.norm;
		vec3 wo = -r.d;
		vec3 Lf = vec3(0.f);
		for(const auto& L : scene.lights)
		{
			vec3 wi;
			float pdf;
			vec3 Li = L->sample_L(p, wi, pdf);
			if (Li == vec3(0.f) || pdf == 0.f) continue;
			bool vis = scene.surf->hit(ray(p, wi), hit_record());
			vec3 f = bsdf->f(wo, wi);
			if (f != vec3(0.f) && !vis)
				Lf += f * Li * glm::max(0.f, dot(wi, n)) / pdf;
		}
		return Lf;*/
	
}