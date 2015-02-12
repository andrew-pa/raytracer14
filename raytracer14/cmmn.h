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
	struct ray
	{
		vec3 o, d;
		ray(vec3 _o, vec3 _d)
			: o(_o), d(_d) {}
		inline vec3 operator() (float t) const
		{
			return o + d*t;
		}
	};

	struct aabb
	{
		vec3 min, max;

		aabb(vec3 m, vec3 x)
			: min(m), max(x) {}
		
		aabb(const aabb& a, const aabb& b)
			: min(), max() 
		{
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
	};

	inline float max_comp(vec3 v)
	{
		return glm::max(v.x, glm::max(v.y, v.z));
	}
}