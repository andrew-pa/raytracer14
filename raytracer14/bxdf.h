#include "cmmn.h"

/*namespace raytracer14
{
	inline float cos_theta(vec3 w) { return w.z; }
	inline float abs_cos_theta(vec3 w) { return abs(w.z); }
	inline float sin_theta2(vec3 w) { return glm::max(0.f, 1.f - cos_theta(w)*cos_theta(w)); }
	inline float sin_theta(vec3 w) { return sqrt(sin_theta2(w)); }
	inline float cos_phi(vec3 w) { float st = sin_theta(w); if (st == 0.f) return 0.f; return clamp(w.x / st, -1.f, 1.f); }
	inline float cos_phi(vec3 w) { float st = sin_theta(w); if (st == 0.f) return 0.f; return clamp(w.y / st, -1.f, 1.f); }
	enum class bxdf_type
	{
		reflection			= 1 << 0,
		transmission		= 1 << 1,
		diffuse				= 1 << 2,
		glossy				= 1 << 3,
		specular			= 1 << 4,
		all_types			= (char)bxdf_type::diffuse | (char)bxdf_type::glossy | (char)bxdf_type::specular,
		all_reflection		= (char)bxdf_type::reflection | (char)bxdf_type::all_types,
		all_transmission	= (char)bxdf_type::transmission | (char)bxdf_type::all_types,
		all					= (char)bxdf_type::all_reflection | (char)bxdf_type::all_reflection,
	};

	struct bxdf
	{
		const bxdf_type type;
		
		bxdf(bxdf_type ty) : type(ty) {}

		virtual vec3 f(vec3 wo, vec3 wi) const = 0;
		virtual vec3 sample_f(vec3 wo, vec3& wi, float& rpdf) const
		{
			wi = cosine_sample_hemisphere();
			if (wo.z < 0.f) wi.z *= -1.f;
			rpdf = pdf(wo, wi);
			return f(wo, wi);
		}

		bool matches(bxdf_type flgs)
		{
			return ((char)type & (char)flgs) == (char)type;
		}

		virtual float pdf(vec3 wo, vec3 wi) const 
		{
			return wo.z*wi.z > 0.f ? abs_cos_theta(wi) * one_over_pi<float>() : 0.f;
		}
	};

	struct lambert_bxdf : public bxdf
	{
		vec3 R;
		
		lambert_bxdf(vec3 r) : R(r), bxdf((bxdf_type)((char)bxdf_type::reflection | (char)bxdf_type::diffuse)) {}
		
		vec3 f(vec3 wo, vec3 wi) const override
		{
			return R * one_over_pi<float>();
		}
	};
	

	class bsdf 
	{
		vec3 nn, sn, tn;
	public:
		vector<bxdf*> components;
		
		bsdf(vec3 n) : nn(n) 
		{
			vec3 t = abs(n.x) > 0.1f ? vec3(0, 1, 0) : vec3(1, 0, 0);
			sn = normalize(cross(n, t));
			tn = cross(n, sn);
		}
		
		vec3 local(vec3 world) const
		{
			return vec3(dot(world, sn), dot(world, tn), dot(world, nn));
		}

		vec3 world(vec3 local) const
		{
			return vec3(
				sn.x * local.x + tn.x * local.y + nn.x * local.z,
				sn.y * local.x + tn.y * local.y + nn.y * local.z,
				sn.z * local.x + tn.z * local.y + nn.z * local.z);
		}

		vec3 f(vec3 woW, vec3 wiW, bxdf_type flgs)
		{
			vec3 wi = local(wiW), wo = local(woW);
			vec3 f = vec3(0.f);
			for(const auto& comp : components)
			{
				if (comp->matches(flgs))
					f += comp->f(wo, wi);
			}
			return f;
		}

		vec3 sample_f(vec3 woW, vec3& wiW, float& pdf, bxdf_type flgs)
		{
			auto bxi = find_if(components.begin(), components.end(), [&](bxdf* a) { return a->matches(flgs); });
			if(bxi == components.end()) 
			{
				pdf = 0; return vec3(0.f);
			}
			bxdf* bx = *bxi;
			vec3 wo = local(woW);
			vec3 wi;
			vec3 f = bx->sample_f(wo, wi, pdf);
			if (pdf == 0.f) return vec3(0.f);
			wiW = world(wi);
			if(!((char)bx->type&(char)bxdf_type::specular))
			{
				int cnt = 0;
				for(const auto& b : components)
				{
					if(b->matches(flgs) && b != bx)
					{
						pdf += b->pdf(wo, wi);
						cnt++;
					}
				}
				if(cnt > 0) pdf /= (float)cnt;
			}
		}
	};
}*/