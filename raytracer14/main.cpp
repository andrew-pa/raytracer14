#include "cmmn.h"
#include "texture.h"
#include "camera.h"
#include "surface.h"
#include "primitive.h"
#include "aggregate.h"
using namespace raytracer14;

#include <glm/gtc/matrix_transform.hpp>

/*class diffuse_material : public material
{
	lambert_bxdf* b;
public:
	vec3 col;
	diffuse_material(vec3 c) : col(c) { b = new lambert_bxdf(c); }
	bxdf* bsdf(const hit_record& hr) const override
	{
		b->R = col;
		return b;
	}
};*/

#ifdef _DEBUG
#define COMPM "DEBUG"
#elif NDEBUG
#define COMPM "RELEASE"
#else
#define COMPM "UNKNOWN"
#endif

int main() 
{
	texture2d tx{ ivec2(640, 480) };

	camera cam{ vec3(0, 0, 10), vec3(0.f) };

	//sphere sp = sphere(mat4(1), 1.f, 1.f, -1.f, 360.f);
	//cylinder cy = cylinder(rotate(mat4(1), radians(45.f), vec3(1.f, 0.f, 0.f)), 1.f, .0f, .5f, 300.f);
	//disk dk = disk(mat4(1), 1.f, 1.f);
	
	vector<shared_ptr<primitive>> prims;
	prims.push_back(make_shared<geometric_primitive>(make_shared<sphere>(translate(mat4(1), vec3(0.75f, 0.f, 0.f)), .5f, 1.f, -1.f, 360.f)));
	prims.push_back(make_shared<geometric_primitive>(make_shared<sphere>(translate(mat4(1), vec3(-0.75f, 0.f, 0.f)), .5f, 1.f, -1.f, 360.f)));
	//prims.push_back(make_shared<geometric_primitive>(make_shared<sphere>(translate(mat4(1), vec3(0.0f, 0.f, 0.f)), .5f, 1.f, -1.f, 360.f)));

	//prims.push_back(make_shared<geometric_primitive>(make_shared<disk>(mat4(1), 1.f, 1.f)));
	
	aggregate* ag = new bvh_accel(prims, bvh_accel::split_type::middle);

	auto start = chrono::system_clock::now();
	for (int y = 0; y < tx.size.y; ++y) 
	{
		for (int x = 0; x < tx.size.x; ++x)
		{
			vec2 uv = ((vec2(x, y) / (vec2)tx.size) * vec2(2.f)) - vec2(1.f);
			auto r = cam.make_ray(uv);
			intersection hr{ hit_record(10000), nullptr, mat4(1), mat4(1) };
			if(ag->hit(r, hr))
			{
				tx.pixel(ivec2(x, y)) = glm::max(0.3f, dot(hr.nn, vec3(0.f, -.5f, .5f))) * vec3(1.f, 0.5f, 0.f);
			}
			else tx.pixel(ivec2(x, y)) = vec3(0.f);
		}
		cout << "Scanline " << y << endl;
	}
	auto end = chrono::system_clock::now();

	auto render_time = chrono::duration_cast<chrono::milliseconds>(end - start);

	ostringstream outtx;
	outtx << "RENDER TIME: " << render_time.count() << "MS [" << COMPM << "]";
	tx.draw_text(ivec2(8), outtx.str(), vec3(1.f));

	ostringstream fnstr;
	fnstr << "images\\img_" << time(nullptr) << ".bmp";
	tx.write_bmp(fnstr.str());
	return 0;
}