#pragma once
#include "cmmn.h"
#include "surface.h"

/*namespace raytracer14
{
	class bvh_node : public surface
	{
	public:
		surface* left;
		surface* right;
		aabb bounds_;

		bvh_node(surface* l, surface* r)
			: left(l), right(r), bounds_(l->bounds(), r->bounds()){}

		bvh_node(vector<surface*>& objs, uint axis = 0);

		bool hit(const ray& r, hit_record& hr) const override;

		inline aabb bounds() const override { return bounds_; }

		~bvh_node()
		{
			if(left) delete left;
			if(right) delete right;
			left = nullptr, right = nullptr;
		}
	};
}*/