#pragma once
#include "cmmn.h"
#include "scene.h"

namespace raytracer14
{
	struct point_light : public light
	{
		vec3 color, position;
		point_light(vec3 p, vec3 c) : color(c), position(p) {}
		vec3 sample_L(vec3 p, vec3& wi, float& pdf) const override
		{
			wi = normalize(position - p);
			pdf = 1.f;
			return color / distance2(position, p);
		}
	};
}