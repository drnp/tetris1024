/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 HereweTech Co.LTD
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file block.c
 * @author Dr.NP <conan.np@gmail.com>
 * @since 10/19/2021
 */

#include "tetris.h"

// Draw tile
int tile_block(enum block_type_e type, enum block_direction_e dir)
{
    int ret = 0;
    switch(type)
    {
        case BLOCK_L :
            ret = block_L[dir];
            break;
        case BLOCK_S :
            ret = block_S[dir];
            break;
        case BLOCK_J :
            ret = block_J[dir];
            break;
        case BLOCK_I :
            ret = block_I[dir];
            break;
        case BLOCK_Z :
            ret = block_Z[dir];
            break;
        case BLOCK_O :
            ret = block_O[dir];
            break;
        case BLOCK_T :
            ret = block_T[dir];
            break;
    };

    return ret;
}

BLOCK * new_block()
{
    BLOCK *b = malloc(sizeof(BLOCK));
    memset(b, 0, sizeof(BLOCK));
    b->type = (get_random() % 7) + 1;
    b->color = (get_random() % 6) + 25;
    b->direction = BLOCK_DIR_0;
    b->pos.x = (PLAYGROUND_WIDTH - 4) / 2;
    b->pos.y = 0;
    b->tile = tile_block(b->type, b->direction);
    b->dropped = FALSE;

    return b;
}

void del_block(BLOCK *b)
{
    if (b != NULL)
    {
        free(b);
    }

    b = NULL;
    
    return;
}

// Try rotate block
int try_rotate_block(BLOCK *b, bool clockwise, enum block_direction_e *dir)
{
    if (b == NULL)
    {
        return 0;
    }

    int m_dir = (int)b->direction;
    if (clockwise)
    {
        if (m_dir >= BLOCK_DIR_270)
        {
            m_dir = BLOCK_DIR_0;
        }
        else
        {
            m_dir ++;
        }
    }
    else
    {
        if (m_dir <= BLOCK_DIR_0)
        {
            m_dir = BLOCK_DIR_270;
        }
        else
        {
            m_dir --;
        }
    }

    int m_tile = tile_block(b->type, m_dir);
    *dir = m_dir;

    return m_tile;
}

// Check solid
bool check_block_solid(BLOCK *b, int m_tile, int off_y, int off_x, int y, int x)
{
    if (b == NULL)
    {
        return FALSE;
    }

    int tx = x - (b->pos.x + off_x);
    int ty = y - (b->pos.y + off_y);
    if (ty < 0 || ty > 3 || tx < 0 || tx > 3)
    {
        return FALSE;
    }

    if (0 == m_tile)
    {
        return (1 << (ty * 4 + (3 - tx)) & b->tile) > 0;
    }
    else
    {
        return (1 << (ty * 4 + (3 - tx)) & m_tile) > 0;
    }
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
