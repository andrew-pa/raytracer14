#include "cmmn.h"
#include "texture.h"
#include "camera.h"
#include "surface.h"
#include "renderer.h"
using namespace raytracer14;

int main() 
{
	texture2d tx{ ivec2(640*2, 480*2) };

	camera cam{ vec3(0, 0, -5), vec3(0.f) };
	surface* surf = new sphere(vec3(0.f), 1.f);

	renderer* rndr = new std_multi_thread_renderer;

	auto start = chrono::system_clock::now();
	/*for (int y = 0; y < tx.size.y; ++y) 
	{
		for (int x = 0; x < tx.size.x; ++x)
		{
			vec2 uv = ((vec2(x, y) / (vec2)tx.size) * vec2(2.f)) - vec2(1.f);
			auto r = cam.make_ray(uv);
			hit_record hr;
			if(surf->hit(r, hr))
			{
				tx.pixel(ivec2(x, y)) = vec3(1.f, .5f, 0.f)* glm::max(dot(hr.norm, vec3(0,-1,-.5f)), 0.2f);
			}
			else tx.pixel(ivec2(x, y)) = vec3(0.f);
		}
	}*/
	rndr->render(&tx, scene(cam, surf));
	auto end = chrono::system_clock::now();

	auto render_time = chrono::duration_cast<chrono::milliseconds>(end - start);

	ostringstream outtx;
	outtx << "RENDER TIME: " << render_time.count() << "MS";
	tx.draw_text(ivec2(8), outtx.str(), vec3(1.f));

	ostringstream fnstr;
	fnstr << "images\\img_" << time(nullptr) << ".bmp";
	tx.write_bmp(fnstr.str());
	return 0;
}