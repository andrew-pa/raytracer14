#pragma once
#include "cmmn.h"
#include "surface.h"

namespace raytracer14
{
	struct intersection : public hit_record
	{
		const class primitive* prim;
		mat4 w2o, o2w;
		intersection(const hit_record& hr, const class primitive* p, const mat4& _w2o, const mat4& _o2w)
			: hit_record(hr), prim(p), w2o(_w2o), o2w(_o2w)
		{
		}
	};

	class primitive
	{
	public:
		virtual aabb bounds() const = 0;
		virtual bool hit(const ray& r, intersection& intr) const = 0;
		virtual bool hitp(const ray& r, interval& tt) const = 0;
		//virtual bsdf* get_bsdf(const hit_record& hr, const mat4& o2w) const = 0;
	};

	class geometric_primitive : public primitive
	{
	public:
		shared_ptr<surface> shape;
		//shared_ptr<material> material;
		//area_light* light;
		geometric_primitive(shared_ptr<surface> s)
			: shape(s) {}

		bool hit(const ray& r, intersection& intr) const  override
		{
			bool v = shape->hit(r, intr);
			if(v)
			{
				intr.prim = this;
				intr.w2o = shape->world_transform;
				intr.o2w = shape->object_transform;
			}
			return v;
		}

		bool hitp(const ray& r, interval& tt) const override
		{
			return shape->hitp(r, tt);
		}

		aabb bounds() const override
		{
			return shape->world_bounds();
		}
	};
}