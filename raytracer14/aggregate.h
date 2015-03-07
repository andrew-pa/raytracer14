#pragma once
#include "cmmn.h"
#include "primitive.h"

namespace raytracer14
{
	class aggregate : public primitive
	{
	};

	class bvh_accel : public aggregate
	{
		struct bvh_primitive_info 
		{
			//shared_ptr<primitive> prim;
			vec3 center;
			aabb bounds;
			uint index;
			bvh_primitive_info(const aabb& bnds, uint idx)
				:  bounds(bnds), index(idx)//, center(bounds.center())
			{
				center = bounds.center();
			}

			/*inline const bvh_primitive_info operator =(const bvh_primitive_info ifo) const
			{
				return *this;
			}*/
		};
		
		struct node
		{
			aabb bounds;
			node* children[2];
			uint split_axis, first_prim, num_prim;

			node() : bounds(vec3(),vec3()) { children[0] = children[1] = nullptr; }
			
			//make leaf
			node(uint f, uint n, const aabb& b)
				: first_prim(f), num_prim(n), bounds(b)
			{}

			//make interior node
			node(uint axis, node* n1, node* n2)
				: split_axis(axis), num_prim(0), bounds(n1->bounds, n2->bounds)
			{
				children[0] = n1;
				children[1] = n2;
			}
		};

		struct linear_node
		{
			aabb bounds;
			union
			{
				uint primitives_offset; //leaf
				uint second_child_offset; //interior
			};
			uint8_t nprimitives;
			uint8_t axis;
			uint8_t pad[2];
		};

		node* recursive_build(vector<bvh_primitive_info>& pio, uint s, uint f, uint& tn,
			vector<shared_ptr<primitive>>& op);

		vector<shared_ptr<primitive>> primitives;

		linear_node* nodes;

		uint flatten_tree(node* n, uint& off);

		static inline bool aabbhitp(const aabb& b, const ray& r, vec3 rrd)
		{
			if (b.contains(r.o)) return true;


			vec3 t1 = (b.min - r.o) * rrd;
			vec3 t2 = (b.max - r.o) * rrd;

			vec3 m12 = glm::min(t1, t2);
			vec3 x12 = glm::max(t1, t2);

			float tmin = m12.x;
			tmin = glm::max(tmin, m12.y);
			tmin = glm::max(tmin, m12.z);

			float tmax = x12.x;
			tmax = glm::min(tmax, x12.y);
			tmax = glm::min(tmax, x12.z);


			return tmax >= tmin;
		}
	public:
		enum class split_type
		{
			sah, middle, equal_counts
		};
		const split_type _split_type;
		bvh_accel(const vector<shared_ptr<primitive>>& p, split_type tp = split_type::sah);
		bool hit(const ray& r, intersection& intr) const override;
		bool hitp(const ray& r, interval& tt) const override;

		aabb bounds() const override { return nodes->bounds; }
	};
}