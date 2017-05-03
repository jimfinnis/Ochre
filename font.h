/**
 * @file font.h
 * @brief  Brief description of file.
 *
 */

#ifndef __FONT_H
#define __FONT_H
#include <SDL2/SDL_ttf.h>

class Font {
    TTF_Font *font;
public:
    /// init font system
    static void init();
    /// load a font at a given pixel size
    Font(const char *file, int size);
    /// render text to the current region
    void render(float x,float y,float h,const char *s,...);
};

#endif /* __FONT_H */
