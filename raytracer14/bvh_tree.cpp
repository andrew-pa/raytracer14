#include "bvh_tree.h"
/*
namespace raytracer14
{
	bvh_node::bvh_node(vector<surface*>& objs, uint axis)
		: bounds_(vec3(0.f), vec3(0.f))
	{
		if(objs.size() == 1)
		{
			left = objs[0];
			right = nullptr;
			bounds_ = left->bounds();
		}
		else if(objs.size() == 2)
		{
			left = objs[0];
			right = objs[1];
			bounds_ = aabb(left->bounds(), right->bounds());
		}
		else
		{
			sort(objs.begin(), objs.end(), [&](surface* a, surface* b)
			{
				float pac = a->bounds().center()[axis];
				float pbc = a->bounds().center()[axis];
				return pac > pbc;
			});
			auto hlf = objs.size() / 2;
			auto lhf = vector<surface*>(objs.begin(), objs.begin() + hlf);
			auto rhf = vector<surface*>(objs.begin()+hlf, objs.end());
			if (lhf.size() == 1) left = lhf[0];
			else left = new bvh_node(lhf, (axis + 1) % 3);
			if (rhf.size() == 1) right = rhf[0];
			else right = new bvh_node(rhf, (axis + 1) % 3);
			bounds_ = aabb(left->bounds(), right->bounds());
		}
	}

	bool bvh_node::hit(const ray& r, hit_record& hr) const
	{
		if (!bounds_.hit(r)) return false;
		hit_record lhr = hr, rhr = hr;
		bool lhit = left ? left->hit(r, lhr) : false, rhit = right ? right->hit(r, rhr) : false;
		if(lhit && rhit)
		{
			if (lhr.t < rhr.t)
				hr = lhr;
			else
				hr = rhr;
			return true;
		}
		else if(lhit)
		{
			hr = lhr;
			return true;
		}
		else if(rhit)
		{
			hr = rhr;
			return true;
		}
		return false;
	}
}*/