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
			weak_ptr<primitive> prim;
			vec3 center;
			aabb bounds;
			bvh_primitive_info(shared_ptr<primitive> p)
				: prim(p), bounds(p->bounds()), center(bounds.center())
			{}
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

		node* recursive_build(const vector<bvh_primitive_info>& pio, uint s, uint f, uint& tn,
			const vector<shared_ptr<primitive>>& op);

		vector<shared_ptr<primitive>> primitives;

		linear_node* nodes;

		uint flatten_tree(node* n, uint& off);
	public:
		enum class split_type
		{
			sah, middle, equal_counts
		};
		const split_type _split_type;
		bvh_accel(const vector<shared_ptr<primitive>>& p, split_type tp = split_type::sah);
		bool hit(const ray& r, intersection& intr) const override;
		bool hitp(const ray& r, interval& tt) const override;
	};
}