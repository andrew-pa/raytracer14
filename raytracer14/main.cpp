#include "cmmn.h"
#include "texture.h"
using namespace raytracer14;

int main() 
{
	texture2d tx{ ivec2(640, 48) };
	tx.draw_text(ivec2(0, 0), "HELLO, WORLD!", vec3(1.f, 0.5f, 0.f));

	ostringstream fnstr;
	fnstr << "images\\img_" << time(nullptr) << ".bmp";
	tx.write_bmp(fnstr.str());
	return 0;
}