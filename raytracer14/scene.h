#pragma once
#include "camera.h"
#include "surface.h"

namespace raytracer14
{
	struct light
	{
		virtual vec3 sample_L(vec3 p, vec3& wi, float& pdf) const = 0;
	};

	struct scene
	{
		camera cam;
		surface* surf;
		vector<light*> lights;
		scene() : cam(vec3(0.f), vec3(0.f)) {}
		scene(const camera& c, surface* sf) : cam(c), surf(sf) {}
	};
}