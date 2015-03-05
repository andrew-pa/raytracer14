#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>
#include <memory>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/orthonormalize.hpp>
using namespace glm;

namespace raytracer14
{
	struct interval
	{
		float min, max;
		interval(float a, float b)
			: min(glm::min(a,b)), max(glm::max(a,b)) {}
		interval()
			: min(numeric_limits<float>::max()), max(numeric_limits<float>::min()){}
		interval(float m, float x, bool force) 
			: min(m), max(x) {}

		inline float length() const { return max - min; }

		inline bool outside(const interval& o) const
		{
			//		  O2 T        T O1
			// --------]-[--------]-[--------
			return min > o.max || max < o.min;
		}

		inline bool contains(float v) const
		{
			//			 M       X
			// ----------[---P---]----------
			return v > min && v < max;
		}
	};
	struct ray
	{
		vec3 o, d;
		ray(vec3 _o, vec3 _d)
			: o(_o), d(_d) {}
		inline vec3 operator() (float t) const
		{
			return o + d*t;
		}
		inline const ray& operator* (const mat4& t) const
		{
			return ray(vec3(t*vec4(o,1.f)), vec3(t*vec4(d, 0.f)));
		}
	};

	struct ray_diff : public ray
	{
		vec3 dxo, dyo;
		vec3 dxd, dyd;
		ray_diff(const ray& r) : ray(r) {}
		void scale(float s)
		{
			dxo = o + (dxo - o)*s;
			dyo = o + (dyo - o)*s;
			dxd = d + (dxd - d)*s;
			dyd = d + (dyd - d)*s; 
		}
	};

	struct aabb
	{
		vec3 min, max;

		aabb() {}

		aabb(vec3 m, vec3 x)
			: min(m), max(x) {}
		
		aabb(const aabb& a, const aabb& b)
			: min(numeric_limits<float>::max()), max(numeric_limits<float>::min()) 
		{
			add_point(a.min);
			add_point(a.max);
			add_point(b.min);
			add_point(b.max);
		}

		inline void add_point(vec3 p)
		{
			if (p.x > max.x) max.x = p.x;
			if (p.y > max.y) max.y = p.y;
			if (p.z > max.z) max.z = p.z;

			if (p.x < min.x) min.x = p.x;
			if (p.y < min.y) min.y = p.y;
			if (p.z < min.z) min.z = p.z;
		}

		inline aabb transform(const mat4& m) const
		{
			vec3 min, max;
			min = vec3(m[3][0], m[3][1], m[3][2]);
			max = min;

			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
				{
					if (m[i][j] > 0)
					{
						min[i] += m[i][j] * min[j];
						max[i] += m[i][j] * max[j];
					}
					else
					{
						min[i] += m[i][j] * max[j];
						max[i] += m[i][j] * min[j];
					}
				}
			return aabb(min, max);
		}
		
		inline bool contains(vec3 p) const
		{
			if (p.x >= min.x && p.x <= max.x &&
				p.y >= min.y && p.y <= max.y &&
				p.z >= min.z && p.z <= max.z)
				return true;
			return false;
		}

		inline bool hit(const ray& r) const
		{
			if (contains(r.o)) return true;

			vec3 rrd = 1.f / r.d;

			vec3 t1 = (min - r.o) * rrd;
			vec3 t2 = (max - r.o) * rrd;

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

		inline pair<float, float> hit_retint(const ray& r) const
		{
			vec3 rrd = 1.f / r.d;

			vec3 t1 = (min - r.o) * rrd;
			vec3 t2 = (max - r.o) * rrd;

			vec3 m12 = glm::min(t1, t2);
			vec3 x12 = glm::max(t1, t2);

			float tmin = m12.x;
			tmin = glm::max(tmin, m12.y);
			tmin = glm::max(tmin, m12.z);

			float tmax = x12.x;
			tmax = glm::min(tmax, x12.y);
			tmax = glm::min(tmax, x12.z);


			return pair<float, float>(tmin, tmax);
		}

		inline vec3 center() const
		{
			return (min + max) * .5f;
		}

		inline int max_extent() const
		{
			vec3 diag = max - min;
			if (diag.x > diag.y && diag.x > diag.z)
				return 0;
			else if (diag.y > diag.z)
				return 1;
			else
				return 2;
		}

		inline float surface_area() const
		{
			vec3 d = max - min;
			return 2.f*(d.x*d.y + d.x*d.z + d.y*d.z);
		}
	};

	inline float max_comp(vec3 v)
	{
		return glm::max(v.x, glm::max(v.y, v.z));
	}

	

	inline bool quadradic(float A, float B, float C, pair<float,float>& x)
	{
		float d = B*B - 4.f*A*C;
		if (d <= 0.f) return false;
		d = sqrtf(d);
		float q;
		if (B < 0) q = -.5f * (B - d);
		else       q = -.5f * (B + d);
		x = make_pair(q / A, C / q);
		return true;
	}

	inline vec3 cosine_sample_hemisphere()
	{
		vec2 dsk = diskRand(1.f);
		return vec3(dsk, sqrt(glm::max(0.f, 1.f - dsk.x*dsk.x - dsk.y*dsk.y)));
	}
}