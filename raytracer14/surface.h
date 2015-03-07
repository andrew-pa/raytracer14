#pragma once
#include "cmmn.h"
#include "bxdf.h"

namespace raytracer14
{
	
	struct hit_record
	{
		interval t;
		vec3 p;
		vec3 nn;
		vec2 uv;
		const class surface* surf;

		vec3 dpdu, dpdv;
		vec3 dndu, dndv;

		hit_record(float maxt)
			: t(0, maxt)
		{
		}

		hit_record(const interval& tt, const vec4& pp, const vec4& xdpdu, const vec4& xdpdv, const vec4& xdndu, const vec4& xdndv, vec2 uuvv, const surface* s)
			: t(tt), p(pp), dpdu(xdpdu), dpdv(xdpdv), dndu(xdndu), dndv(xdndv),
			nn(0.), uv(uuvv), surf(s) 
		{
			nn = normalize(cross(dpdu, dpdv));
		}
	};

/*	struct material
	{
		virtual bxdf* bsdf(const hit_record& hr) const = 0;
	};
	*/
	struct surface
	{
		mat4 world_transform; //object->world
		mat4 object_transform;
		surface(const mat4& wmx) : world_transform(wmx), object_transform(inverse(wmx)) {}
		virtual bool hit(const ray& r, hit_record& hr) const = 0;
		virtual bool hitp(const ray& r, interval& tt) const = 0;
		virtual aabb bounds() const = 0;
		aabb world_bounds() const { return bounds().transform(object_transform); }
		virtual float area() const { return 0.f; /*shape with no shape has no area*/ }

		inline void set_world_transform(const mat4& w)
		{
			world_transform = w;
			object_transform = inverse(w);
		}
	};

	class sphere : public surface
	{
	public:
		float radius, phimax;
		interval z, theta;
		sphere(const mat4& wmx, float rad, float z0, float z1, float pm)
			: surface(wmx), radius(rad), z(clamp(glm::min(z0, z1), -rad,rad), clamp(glm::max(z0, z1),-rad,rad)), 
			theta(acosf(clamp(z.min/rad, -1.f, 1.f)), acosf(clamp(z.max/rad, -1.f, 1.f))), phimax(radians(clamp(pm, 0.f, 360.f))) 
		{

		}

		bool hit(const ray& r, hit_record& hr) const override
		{
			ray rt = r*world_transform;
			float A = dot(rt.d, rt.d);
			float B = 2 * dot(rt.d, rt.o);
			float C = dot(rt.o, rt.o) - (radius*radius);
			pair<float,float> t;
			if (!quadradic(A, B, C, t))
				return false;
			if (t.first > hr.t.max || t.second < hr.t.min) return false;
			float thit = t.first;
			if(t.first < hr.t.min)
			{
				thit = t.second; 
				if (thit > hr.t.max) return false;
			}
			
			vec3 phit = rt(thit);
			if (phit.x == 0.f && phit.y == 0.f) phit.x = 1e-5f * radius;
			float phi = atan2f(phit.x, phit.y);
			if (phi < 0.f) phi += 2.f*pi<float>();
			if((z.min > -radius && phit.z < z.min) || (z.max < radius && phit.z > z.max) || phi > phimax)
			{
				if (thit == t.second) return false;
				if (t.second > hr.t.max) return false;
				thit = t.second;
				phit = rt(thit);
				if (phit.x == 0.f && phit.y == 0.f) phit.x = 1e-5f * radius;
				float phi = atan2f(phit.x, phit.y);
				if (phi < 0.f) phi += 2.f*pi<float>();
				if ((z.min > -radius && phit.z < z.min) ||
					(z.max < radius && phit.z > z.max) || phi > phimax) return false;
			}
			
			float u = phi / phimax;
			float thetav = acosf(clamp(phit.z / radius, -1.f, 1.f));
			float v = (thetav - theta.min) / theta.length();

			float zrad = sqrtf(phit.x*phit.x + phit.y*phit.y);
			float invzrad = 1.f / zrad;
			float cosphi = phit.x * invzrad;
			float sinphi = phit.y * invzrad;
			vec3 dpdu(-phimax * phit.y, phimax*phit.x, 0);
			vec3 dpdv = theta.length() * vec3(phit.z*cosphi, phit.z*sinphi, -radius*sinf(thetav));

			vec3 d2pduu = -phimax*phimax*vec3(phit.x, phit.y, 0);
			vec3 d2pduv = theta.length()*phit.z*phimax*vec3(-sinphi, cosphi, 0);
			vec3 d2pdvv = -theta.length()*theta.length()*phit;

			float E = dot(dpdu, dpdu);
			float F = dot(dpdu, dpdv);
			float G = dot(dpdv, dpdv);
			vec3 N = normalize(cross(dpdu, dpdv));
			float e = dot(N, d2pduu);
			float f = dot(N, d2pduv);
			float g = dot(N, d2pdvv);
			float invEGF2 = 1.f / (E*G - F*F);
			vec3 dndu = vec3(	(f*F - e*G) * invEGF2 * dpdu +
								(e*F - f*E) * invEGF2 * dpdv);
			vec3 dndv = vec3(	(g*F - f*G) * invEGF2 * dpdu +
								(f*F - g*E) * invEGF2 * dpdv);

			hr = hit_record(interval(thit, hr.t.max), object_transform*vec4(phit,1.f), 
				object_transform*vec4(dpdu,1.f), object_transform*vec4(dpdv,1.f), 
				object_transform*vec4(dndu,0.f), object_transform*vec4(dndv,0.f), vec2(u, v), this);
			return true;
		}
	
		bool hitp(const ray& r, interval& tt) const override
		{
			ray rt = r*world_transform;
			float A = dot(rt.d, rt.d);
			float B = 2 * dot(rt.d, rt.o);
			float C = dot(rt.o, rt.o) - (radius*radius);
			pair<float, float> t;
			if (!quadradic(A, B, C, t))
				return false;
			if (t.first > tt.max || t.second < tt.min) return false;
			float thit = t.first;
			if (t.first < tt.min)
			{
				thit = t.second;
				if (thit > tt.max) return false;
			}

			vec3 phit = rt(thit);
			if (phit.x == 0.f && phit.y == 0.f) phit.x = 1e-5f * radius;
			float phi = atan2f(phit.x, phit.y);
			if (phi < 0.f) phi += 2.f*pi<float>();
			if ((z.min > -radius && phit.z < z.min) || (z.max < radius && phit.z > z.max) || phi > phimax)
			{
				if (thit == t.second) return false;
				if (t.second > tt.max) return false;
				thit = t.second;
				phit = rt(thit);
				if (phit.x == 0.f && phit.y == 0.f) phit.x = 1e-5f * radius;
				float phi = atan2f(phit.x, phit.y);
				if (phi < 0.f) phi += 2.f*pi<float>();
				if ((z.min > -radius && phit.z < z.min) ||
					(z.max < radius && phit.z > z.max) || phi > phimax) return false;
			}
			tt.min = thit;
			return true;
		}
		aabb bounds() const override
		{
			return aabb(vec3(-radius), vec3(radius));
		}
		float area() const override
		{
			return phimax*radius*z.length();
		}
	};

	class cylinder : public surface
	{
	public:
		float radius, phimax;
		interval z;
		cylinder(const mat4& wmx, float r, float z0, float z1, float pm)
			: surface(wmx), radius(r), phimax(radians(clamp(pm, 0.f, 360.f))), z(z0, z1) {}

		aabb bounds() const override
		{
			return aabb(vec3(-radius, -radius, z.min), vec3(radius, radius, z.max));
		}

		bool hit(const ray& r, hit_record& hr) const override
		{
			float A = r.d.x*r.d.x + r.d.y*r.d.y;
			float B = 2.f * (r.d.x*r.o.x + r.d.y*r.o.y);
			float C = r.o.x*r.o.x + r.o.y*r.o.y - radius*radius;
			ray rt = r*world_transform;
			pair<float, float> t;
			if (!quadradic(A, B, C, t))
				return false;

			if (t.first > hr.t.max || t.second < hr.t.min) return false;
			float thit = t.first;
			if (t.first < hr.t.min)
			{
				thit = t.second;
				if (thit > hr.t.max) return false;
			}

			vec3 phit = rt(thit);
			float phi = atan2f(phit.y, phit.x);
			if (phi < 0.) phi += two_pi<float>();
			if(phit.z < z.min || phit.z > z.max || phi > phimax) 
			{
				if (thit == t.second) return false;
				thit = t.second;
				if (t.second > hr.t.max) return false;
				phit = rt(thit);
				phi = atan2f(phit.y, phit.x);
				if (phi < 0.) phi += two_pi<float>();
				if (phit.z < z.min || phit.z > z.max || phi > phimax) return false;
			}

			float u = phi / phimax;
			float v = (phit.z - z.min) / z.length();
			vec3 dpdu(-phimax*phit.y, phimax*phit.x, 0);
			vec3 dpdv(0, 0, z.length());

			vec3 d2pduu = -phimax * phimax * vec3(phit.x, phit.y, 0);
			vec3 d2pduv(0.f), d2pdvv(0.f);

			float E = dot(dpdu, dpdu);
			float F = dot(dpdu, dpdv);
			float G = dot(dpdv, dpdv);
			vec3 N = normalize(cross(dpdu, dpdv));
			float e = dot(N, d2pduu);
			float f = dot(N, d2pduv);
			float g = dot(N, d2pdvv);
			float invEGF2 = 1.f / (E*G - F*F);
			vec3 dndu = vec3((f*F - e*G) * invEGF2 * dpdu +
				(e*F - f*E) * invEGF2 * dpdv);
			vec3 dndv = vec3((g*F - f*G) * invEGF2 * dpdu +
				(f*F - g*E) * invEGF2 * dpdv);

			hr = hit_record(interval(thit, hr.t.max), object_transform*vec4(phit, 1.f),
				object_transform*vec4(dpdu, 1.f), object_transform*vec4(dpdv, 1.f),
				object_transform*vec4(dndu, 0.f), object_transform*vec4(dndv, 0.f), vec2(u, v), this);
			return true;
		}

		bool hitp(const ray& r, interval& tt) const override
		{
			float A = r.d.x*r.d.x + r.d.y*r.d.y;
			float B = 2.f * (r.d.x*r.o.x + r.d.y*r.o.y);
			float C = r.o.x*r.o.x + r.o.y*r.o.y - radius*radius;
			ray rt = r*world_transform;
			pair<float, float> t;
			if (!quadradic(A, B, C, t))
				return false;

			if (t.first > tt.max || t.second < tt.min) return false;
			float thit = t.first;
			if (t.first < tt.min)
			{
				thit = t.second;
				if (thit > tt.max) return false;
			}

			vec3 phit = rt(thit);
			float phi = atan2f(phit.y, phit.x);
			if (phi < 0.) phi += two_pi<float>();
			if (z.contains(phit.z) || phi > phimax)
			{
				if (thit == t.second) return false;
				thit = t.second;
				if (t.second > tt.max) return false;
				phit = rt(thit);
				phi = atan2f(phit.y, phit.x);
				if (phi < 0.) phi += two_pi<float>();
				if (z.contains(phit.z) || phi > phimax) return false;
			}
			tt.min = thit;
			return true;
		}
		
		float area() const override
		{
			return z.length()*phimax*radius;
		}
	};

	class disk : public surface
	{
	public:
		float height, radius, inner_radius, phimax;
		disk(const mat4& wmx, float h, float r, float ir = 0.f, float p = 360.f)
			: surface(wmx), height(h), radius(r), inner_radius(ir), phimax(p) {}

		bool hit(const ray& r, hit_record& hr) const override
		{
			ray rt = r*world_transform;
			if (fabsf(rt.d.z) < 1e-7) return false;
			float thit = (height - rt.o.z)/rt.d.z;
			if (thit < hr.t.min || thit > hr.t.max) return false;
			vec3 phit = rt(thit);
			float dist2 = phit.x*phit.x + phit.y*phit.y;
			if (dist2 > radius*radius || dist2 < inner_radius*inner_radius)
				return false;
			float phi = atan2f(phit.y, phit.x);
			if (phi < 0) phi += two_pi<float>();
			if (phi > phimax) return false;
			float u = phi / phimax;
			float v = 1.f - ((sqrtf(dist2) - inner_radius) / (radius - inner_radius));
			vec3 dpdu(-phimax*phit.y, phimax*phit.x, 0.f);
			vec3 dpdv(-phit.x / (1.f - v), -phit.y / (1 - v), 0.f);
			dpdu *= phimax*one_over_two_pi<float>();
			dpdv *= (radius - inner_radius) / radius;
			hr = hit_record(interval(thit, hr.t.max), object_transform*vec4(phit, 1.f),
				object_transform*vec4(dpdu, 1.f), object_transform*vec4(dpdv, 1.f),
				vec4(0.f), vec4(0.f), vec2(u, v), this);
			return true;
		}

		bool hitp(const ray& r, interval& tt) const override
		{
			ray rt = r*world_transform;
			if (fabsf(rt.d.z) < 1e-7) return false;
			float thit = (height - rt.o.z) / rt.d.z;
			if (thit < tt.min || thit > tt.max) return false;
			vec3 phit = rt(thit);
			float dist2 = phit.x*phit.x + phit.y*phit.y;
			if (dist2 > radius*radius || dist2 < inner_radius*inner_radius)
				return false;
			float phi = atan2f(phit.y, phit.x);
			if (phi < 0) phi += two_pi<float>();
			if (phi > phimax) return false;
			return true;
		}

		aabb bounds() const override
		{
			return aabb(vec3(-radius, -radius, height),vec3(radius,radius,height));
		}

		float area() const override
		{
			return phimax*0.5f*(radius*radius - inner_radius*inner_radius);
		}
	};

	/*class box : public surface
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
		box(vec3 c, vec3 e, material* mat = nullptr)
			: bound(c-e, c+e), surface(mat) {}
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
	};*/
}