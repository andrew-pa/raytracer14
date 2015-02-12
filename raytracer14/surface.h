#pragma once
#include "cmmn.h"

namespace raytracer14
{
	
	struct hit_record
	{
		float t;
		vec3 norm;
		vec2 texcoord;
		const struct surface* surf;
		hit_record(float _t = 10000.f) : t(_t), surf(nullptr) {}
	};

	struct surface
	{
		virtual bool hit(const ray& r, hit_record& hr) const = 0;
		virtual aabb bounds() const = 0;
	};

	class sphere : public surface
	{
	public:
		vec3 center;
		float radius;

		bool hit(const ray& r, hit_record& hr) const override
		{
			vec3 v = r.o - center;
			float b = -dot(v, r.d);
			float det = (b*b) - dot(v, v) + radius*radius;
			if(det > 0) //real roots
			{
				det = glm::sqrt(det);
				float i1 = b - det;
				float i2 = b + det;
				if(i2 > 0) 
				{
					if(i1 < 0) 
					{
						if (i2 < hr.t) 
						{
							return false;
						}
					}
					else 
					{
						if(i1 < hr.t) 
						{
							hr.t = i1;
							hr.surf = this;
							hr.norm = normalize(r(i1) - center);

							float phi = acos(-dot(hr.norm, vec3(0, 1, 0)));
							hr.texcoord.y = phi * one_over_pi<float>();
							float theta = acos(dot(vec3(0, 0, -1), hr.norm) / sin(phi)) * two_over_pi<float>();
							if (dot(vec3(1, 0, 0), hr.norm) >= 0.f) hr.texcoord.x = (1 - theta);
							else hr.texcoord.x = theta;
							
							return true;
						}
					}
				}
			}
			return false;
		}

		inline aabb bounds() const override
		{
			return aabb(center - vec3(radius), center + vec3(radius));
		}
	};
}