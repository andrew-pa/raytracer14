#include "aggregate.h"

namespace raytracer14
{
	bvh_accel::node* bvh_accel::recursive_build(const vector<bvh_primitive_info>& pio, uint s, uint f, uint& tn,
		const vector<shared_ptr<primitive>>& op)
	{
		tn++;
		node* n = nullptr;
		aabb bnd;
		for (int i = s; i < f; ++i)
			bnd = aabb(bnd, pio[i].bounds);
		uint np = f - s;
		if(np == 1)
		{
			n = new node(s, np, bnd);
		}
		else
		{
			aabb centroid_bounds;
			for (uint i = s; i < f; ++i)
				centroid_bounds.add_point(pio[i].center);
			int dim = centroid_bounds.max_extent();
			uint mid = (s+f) / 2;
			if(centroid_bounds.max[dim] == centroid_bounds.min[dim])
			{
				n = new node(s, np, bnd);
			}
			else
			{
				if(_split_type == split_type::middle)
				{
					float pmid = .5f * (centroid_bounds.max[dim] + centroid_bounds.min[dim]);
					auto ptmid = partition(pio.begin(), pio.end(), 
						[&](const bvh_primitive_info& i){ return i.center[dim] < pmid; });
					mid = std::distance(pio.begin(), ptmid);
				}
				else if(_split_type == split_type::equal_counts)
				{
					nth_element(pio.begin(), pio.begin() + mid, pio.end(), [&](const bvh_primitive_info& pa, const bvh_primitive_info& pb){ return pa.center[dim] < pb.center[dim]; });
				}
				else if(_split_type == split_type::sah)
				{
					if(np <= 4)
					{
						nth_element(pio.begin(), pio.begin() + mid, pio.end(), [&](const bvh_primitive_info& pa, const bvh_primitive_info& pb){ return pa.center[dim] < pb.center[dim]; });
					}
					else
					{
						const int nbuckets = 12;
#define BUCKET_COUNT 0
#define BUCKET_BOUND 1
#define BUCKET_COST  2
						vector<tuple<int, aabb, float>> buckets(nbuckets);
						for (uint i = s; i < f; ++i)
						{
							int b = nbuckets * ((pio[i].center[dim] - centroid_bounds.min[dim]) / (centroid_bounds.max[dim] - centroid_bounds.min[dim]));
							if (b == nbuckets) b--;
							get<BUCKET_COUNT>(buckets[b])++;
							get<BUCKET_BOUND>(buckets[b]) = aabb(get<BUCKET_BOUND>(buckets[b]), pio[i].bounds);
						}
						for (int i = 0; i < nbuckets; ++i)
						{
							aabb b0, b1;
							int c0 = 0, c1 = 0;
							for (int j = 0; j <= i; ++j)
							{
								b0 = aabb(b0, get<BUCKET_BOUND>(buckets[j]));
								c0 += get<BUCKET_COUNT>(buckets[j]);
							}
							for (int j = i+1; j < nbuckets; ++j)
							{
								b1 = aabb(b1, get<BUCKET_BOUND>(buckets[j]));
								c1 += get<BUCKET_COUNT>(buckets[j]);
							}
							get<BUCKET_COST>(buckets[i]) = .125f * (c0*b0.surface_area() + c1*b1.surface_area()) / bnd.surface_area();
						}
						float min_cost = get<BUCKET_COST>(buckets[0]);
						uint min_cost_split = 0;
						for (int i = 1; i < nbuckets; ++i)
						{
							if(get<BUCKET_COST>(buckets[i]) < min_cost)
							{
								min_cost = get<BUCKET_COST>(buckets[i]);
								min_cost_split = i;
							}
						}
						if(min_cost < np)
						{
							auto ptmid = partition(pio.begin(), pio.end(), 
							[&](const bvh_primitive_info& p)
							{
								int b = nbuckets * ((p.center[dim] - centroid_bounds.min[dim]) / (centroid_bounds.max[dim] - centroid_bounds.min[dim]));
								if (b == nbuckets) b--;
								return b <= min_cost_split;
							});
							mid = std::distance(pio.begin(), ptmid);
						}
						else
						{
							return new node(s, np, bnd);
						}
					}
				}
				n = new node(dim, recursive_build(pio, s, mid, tn, op), 
								  recursive_build(pio, mid, f, tn, op));
			}
		}
		return n;
	}

	uint bvh_accel::flatten_tree(node* n, uint& off)
	{
		linear_node* lnod = &nodes[off];
		lnod->bounds = n->bounds;
		uint moff = off++;
		if(n->num_prim > 0)
		{
			lnod->primitives_offset = n->first_prim;
			lnod->nprimitives = n->num_prim;
		}
		else
		{
			lnod->axis = n->split_axis;
			lnod->nprimitives = 0;
			flatten_tree(n->children[0], off);
			lnod->second_child_offset = flatten_tree(n->children[1], off);
		}
		return moff;
	}

	bvh_accel::bvh_accel(const vector<shared_ptr<primitive>>& p, split_type tp)
		: primitives(p), _split_type(tp)
	{
		vector<bvh_primitive_info> pio;
		for (const auto& x : p) pio.push_back(bvh_primitive_info(x));
		vector<shared_ptr<primitive>> order_p;
		order_p.reserve(p.size());
		uint tot_nodes = 0;
		node* root = recursive_build(pio, 0, p.size(), tot_nodes, order_p);
		primitives = order_p;
		nodes = new linear_node[tot_nodes];
		uint offset = 0;
		flatten_tree(root, offset);
	}
	
	bool bvh_accel::hit(const ray& r, intersection& intr) const
	{
		if (!nodes) return false;
		bool hit = false;
		vec3 org = r(intr.t.min);
		vec3 invd = 1.f / r.d;
		bool din[3] = { invd.x < 0, invd.y < 0, invd.z < 0 };

		uint tdo = 0, nodenum = 0;
		uint todo[64];
		while(true)
		{
			const linear_node* nod = &nodes[nodenum];
			if(aabbhitp(nod->bounds, r, invd, din))
			{
				if(nod->nprimitives > 0)
				{
					for (uint i = nod->primitives_offset; i < nod->nprimitives; ++i)
					{
						if (primitives[i]->hit(r, intr))
							hit = true;
					}
					if (tdo == 0) break;
					nodenum = todo[--tdo];
				}
				else
				{
					if(din[nod->axis]) 
					{
						todo[tdo++] = nodenum + 1;
						nodenum = nod->second_child_offset;
					}
				}
			}
			else
			{
				if (tdo == 0) break;
				nodenum = todo[--tdo];
			}
		}

		return hit;
	}
	
	bool bvh_accel::hitp(const ray& r, interval& tt) const
	{
	}
}