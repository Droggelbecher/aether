
#ifndef _COLORMAPS_H_
#define _COLORMAPS_H_

#include <SDL.h>
#include <cmath>

uint32_t test(double v, SDL_PixelFormat *_pixel_format) {
	return SDL_MapRGBA(
		_pixel_format,
		255 * (v * v),
		255 * (v),
		255 * (v * (1-v)),
		255
	);
}

#endif // _COLORMAPS_H_


