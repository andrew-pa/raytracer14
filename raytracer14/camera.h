#pragma once
#include "cmmn.h"

namespace raytracer14
{
	class camera
	{
	public:
		vec3 position, look, up, right;
		float w;

		camera(vec3 pos, vec3 targ, vec3 _up = vec3(0, 1, 0))
			: position(pos), look(normalize(targ-position)), up(_up), w(2.5f)
		{
			right = normalize(cross(look, up));
		}

		inline void look_at(vec3 targ)
		{
			look = normalize(targ - position);
			right = normalize(cross(look, vec3(0, 1, 0)));
			up = normalize(cross(look, right));
		}

		inline const ray& make_ray(vec2 uv) const
		{
			return ray(position,
				normalize(w*look + uv.x*right + -1.f*uv.y*up));
		}
	};
}