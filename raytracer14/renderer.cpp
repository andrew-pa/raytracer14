#include "renderer.h"
#include <queue>
#include <mutex>

/*namespace raytracer14
{
	vec3 renderer::ray_color(const scene& scene, const ray& r) const
	{
		hit_record hr;
		if(scene.surf->hit(r, hr))
		{
			return surfitrg->Li(this, r, scene, hr);
		}
		return vec3(0.f);
	}

	
	void std_multi_thread_renderer::render(texture2d* rtx, const scene& scene) const
	{
		vector<thread> workers;
		queue<ivec2> tiles;
		mutex tiles_mut;
		ivec2 p;
		tiles_mut.lock();
		for (; p.y < rtx->size.y; p.y += tile_size.y)
		{
			for (; p.x < rtx->size.x; p.x += tile_size.x)
				tiles.push(p);
			p.x = 0;
		}
		tiles_mut.unlock();
		for (int i = 0; i < thread::hardware_concurrency(); ++i)
		{
			workers.push_back(thread([&]
			{
				while(true)
				{
					ivec2 ptile;
					tiles_mut.lock();
					if (tiles.empty())
					{
						tiles_mut.unlock();
						break;
					}
					ptile = tiles.front(); tiles.pop();
					tiles_mut.unlock();
					
					ivec2 ptile_e = ptile + (ivec2)tile_size;
					ivec2 p = ptile;
					vec2 dv = 2.f / (vec2)rtx->size;
					for (; p.y < rtx->size.y && p.y < ptile_e.y; ++p.y)
					{
						for (; p.x < rtx->size.x && p.x < ptile_e.x; ++p.x)
						{
							vec2 uv = ((vec2)p * dv) - vec2(1.f);
							auto r = scene.cam.make_ray(uv);
							rtx->pixel(p) = ray_color(scene, r);
						}
						p.x = ptile.x;
					}
				}
			}));
		}
		for (auto& t : workers)
		{
			t.join();
		}
	}
	
}*/