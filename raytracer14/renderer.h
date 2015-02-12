#pragma once
#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"

namespace raytracer14
{
	struct scene
	{
		camera cam;
		surface* surf;
		scene(): cam(vec3(0.f), vec3(0.f)) {}
		scene(const camera& c, surface* sf): cam(c), surf(sf) {}
	};

	class renderer
	{
	protected:
		vec3 ray_color(const scene& scene, const ray& r) const; //provide std rendering but different acclerators
	public:
		virtual void render(texture2d* rtx, const scene& scene) const = 0;
	};

	class simple_renderer : public renderer
	{
	public:
		void render(texture2d* rtx, const scene& scene) const override
		{
			ivec2 p(0, 0);
			const vec2 scl = 2.f / (vec2)rtx->size;
			for (; p.y < rtx->size.y; ++p.y)
			{
				for (; p.x < rtx->size.x; ++p.x)
				{
					auto r = scene.cam.make_ray(((vec2)p)*scl - vec2(1.f));
					vec3 c = ray_color(scene,  r);
					rtx->pixel(p) = c;
				}
				p.x = 0;
			}
		}
	};

	class std_multi_thread_renderer : public renderer
	{
	public:
		ivec2 tile_size;
		std_multi_thread_renderer(ivec2 ts = ivec2(32))
			: tile_size(ts) {}
		void render(texture2d* rtx, const scene& scene) const override;
	};
}