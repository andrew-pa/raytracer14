#include "cmmn.h"

namespace raytracer14
{
	class texture2d
	{
		vec3* _px;
	public:
		const ivec2 size;
		texture2d(ivec2 dims)
			: size(dims), _px(new vec3[dims.x*dims.y]) {}

		void write_bmp(const string& fn) const;
		vec3& pixel(ivec2 p) { return _px[p.x + p.y*size.x]; }
		const vec3& pixel(ivec2 p) const { return _px[p.x + p.y*size.x]; }
		vec3& texel(vec2 t) { ivec2 p = (ivec2)floor(t*(vec2)size); return _px[p.x + p.y*size.x]; }
		const vec3& texel(vec2 t) const { ivec2 p = (ivec2)floor(t*(vec2)size); return _px[p.x + p.y*size.x]; }

		void draw_text(ivec2 p, const string& tx, vec3 color);

		~texture2d() { delete _px; }
	};

}