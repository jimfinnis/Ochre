/**
 * @file font.h
 * @brief  Brief description of file.
 *
 */

#ifndef __FONT_H
#define __FONT_H

class Font {
    TTF_Font *font;
public:
    Font(const char *file, int size);
    /// render text to the current region
    void render(float x,float y,float h,const char *s,...);
};

#endif /* __FONT_H */
