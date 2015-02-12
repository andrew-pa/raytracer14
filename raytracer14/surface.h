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

		sphere(vec3 c = vec3(0.f), float r = 1.f)
			: center(c), radius(r) {}

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

	class box : public surface
	{
		inline vec3 normal(vec3 p) const
		{
			static const vec3 axs[] = 
			{
				vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)
			};
			vec3 n = vec3(0.f);
			float m = numeric_limits<float>::max();
			float dist;
			vec3 center = (bound.min + bound.max) * .5f; vec3 extent = (bound.max - center);
			vec3 np = p - center;
			for (int i = 0; i < 3; ++i) 
			{
				dist = abs(extent[i] - abs(np[i]));
				if(dist < m) 
				{
					m = dist;
					n = sign(np[i])*axs[i];
				}
			}
			return n;
		}
	public:
		aabb bound;
		box(vec3 m, vec3 x)
			: bound(m, x) {}
		//box(vec3 c, vec3 e)
		//	: bound(c-e, c+e) {}

		bool hit(const ray& r, hit_record& hr) const override
		{
			auto t = bound.hit_retint(r);
			auto tmax = t.second; auto tmin = t.first;
			if (tmax < tmin || tmin < 0 || tmin > hr.t) return false;
			hr.surf = this;
			hr.t = tmin;
			auto p = r(tmin);
			hr.norm = normal(p);
			hr.texcoord = vec2(p.x, p.y);
		}

		inline aabb bounds() const override { return bound; }
	};
}