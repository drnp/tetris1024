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
 * @file tetris.c
 * @author Dr.NP <conan.np@gmail.com>
 * @since 10/17/2021
 */

#include "tetris.h"

struct tetris_scene_t scene;
timer_t timer;

WINDOW *playground_box = NULL;
WINDOW *score_box = NULL;
WINDOW *level_box = NULL;
WINDOW *blocks_box = NULL;
WINDOW *next_box = NULL;
WINDOW *trace_box = NULL;

BLOCK *curr_block = NULL;
BLOCK *next_block = NULL;

int check_window()
{
    // Color support
    if (FALSE == has_colors())
    {
        endwin();
        printf("Your terminal does not support colors\n\n");

        exit(-1);
    }

    // Window size check
    getmaxyx(stdscr, scene.win_height, scene.win_width);
    if (scene.win_height < SCENE_MIN_HEIGHT || scene.win_width < SCENE_MIN_WIDTH)
    {
        endwin();
        printf("Terminal window too small!\n\n");

        exit(-1);
    }

    return 0;
}

void color_pairs()
{
    // Base black bg
    init_pair(0, COLOR_BLACK, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);

    // Base white bg
    init_pair(8, COLOR_BLACK, COLOR_WHITE);
    init_pair(9, COLOR_RED, COLOR_WHITE);
    init_pair(10, COLOR_GREEN, COLOR_WHITE);
    init_pair(11, COLOR_YELLOW, COLOR_WHITE);
    init_pair(12, COLOR_BLUE, COLOR_WHITE);
    init_pair(13, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(14, COLOR_CYAN, COLOR_WHITE);
    init_pair(15, COLOR_WHITE, COLOR_WHITE);

    // Base blue bg
    init_pair(16, COLOR_BLACK, COLOR_BLUE);
    init_pair(17, COLOR_RED, COLOR_BLUE);
    init_pair(18, COLOR_GREEN, COLOR_BLUE);
    init_pair(19, COLOR_YELLOW, COLOR_BLUE);
    init_pair(20, COLOR_BLUE, COLOR_BLUE);
    init_pair(21, COLOR_MAGENTA, COLOR_BLUE);
    init_pair(22, COLOR_CYAN, COLOR_BLUE);
    init_pair(23, COLOR_WHITE, COLOR_BLUE);

    // Colorful bg
    init_pair(24, COLOR_BLACK, COLOR_BLACK);
    init_pair(25, COLOR_BLACK, COLOR_RED);
    init_pair(26, COLOR_BLACK, COLOR_GREEN);
    init_pair(27, COLOR_BLACK, COLOR_YELLOW);
    init_pair(28, COLOR_BLACK, COLOR_BLUE);
    init_pair(29, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(30, COLOR_BLACK, COLOR_CYAN);
    init_pair(31, COLOR_BLACK, COLOR_WHITE);

    // With white
    init_pair(32, COLOR_WHITE, COLOR_BLACK);
    init_pair(33, COLOR_WHITE, COLOR_RED);
    init_pair(34, COLOR_WHITE, COLOR_GREEN);
    init_pair(35, COLOR_WHITE, COLOR_YELLOW);
    init_pair(36, COLOR_WHITE, COLOR_BLUE);
    init_pair(37, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(38, COLOR_WHITE, COLOR_CYAN);
    init_pair(39, COLOR_WHITE, COLOR_WHITE);

    return;
}

void _splash_title_char(WINDOW *win, char **c, int lines,  int y, int x, int color_idx)
{
    if (win == NULL || c == NULL)
    {
        return;
    }

    int l, i;
    wattron(win, A_BOLD);
    wattron(win, COLOR_PAIR(color_idx));
    for (l = 0; l  < lines; l ++)
    {
        for (i = 0; i < strlen(c[l]); i ++)
        {
            if (c[l][i] != '0')
            {
                mvwaddch(win, y + l, x + i, ACS_CKBOARD);
            }
        }
    }

    wattroff(win, COLOR_PAIR(color_idx));
    wattroff(win, A_BOLD);

    return;
}

void _render_boxes()
{
    if (score_box == NULL || level_box == NULL || blocks_box == NULL)
    {
        return;
    }

    static char buf[32];
    memset(buf, 0, 32);
    sprintf(buf, "%07d", scene.score);
    wattron(score_box, A_BOLD);
    wattron(score_box, COLOR_PAIR(2));
    mvwaddstr(score_box, 4, 6, buf);
    wattroff(score_box, COLOR_PAIR(2));
    wattroff(score_box, A_BOLD);

    memset(buf, 0, 32);
    sprintf(buf, "%7d", scene.level);
    wattron(level_box, A_BOLD);
    wattron(level_box, COLOR_PAIR(6));
    mvwaddstr(level_box, 4, 6, buf);
    wattroff(level_box, COLOR_PAIR(6));
    wattroff(level_box, A_BOLD);

    memset(buf, 0, 32);
    sprintf(buf, "%07d", scene.blocks);
    wattron(blocks_box, A_BOLD);
    wattron(blocks_box, COLOR_PAIR(5));
    mvwaddstr(blocks_box, 4, 6, buf);
    wattroff(blocks_box, COLOR_PAIR(5));
    wattroff(blocks_box, A_BOLD);

    wrefresh(score_box);
    wrefresh(level_box);
    wrefresh(blocks_box);

    return;
}

void _render_next()
{
    if (next_block == NULL)
    {
        return;
    }

    int i, j, sign;
    wattron(next_box, A_BOLD);
    for (i = 0; i < 4; i ++)
    {
        for (j = 0; j < 4; j ++)
        {
            sign = (1 << (i * 4 + j)) & next_block->tile;
            if (sign > 0)
            {
                wattron(next_box, COLOR_PAIR(next_block->color));
            }
            else
            {
                wattron(next_box, COLOR_PAIR(24));
            }

            mvwaddch(next_box, 6 - i, 10 - j * 2, ACS_CKBOARD);
            mvwaddch(next_box, 6 - i, 11 - j * 2, ACS_CKBOARD);
        }
    }

    wattroff(next_box, A_BOLD);
    wrefresh(next_box);

    return;
}

void _render_playground()
{
    char dot;
    int i, j, sign;
    int pos_x, pos_y;

    for (i = 0; i < PLAYGROUND_HEIGHT; i ++)
    {
        for (j = 0; j < PLAYGROUND_WIDTH; j ++)
        {
            // Redraw
            if (scene.playground[i][j] == 'c')
            {
                scene.playground[i][j] = 0;
            }

            if (check_block_solid(curr_block,0, 0, 0, i, j))
            {
                scene.playground[i][j] = 'c';
            }
        }
    }

    for (i = 0; i < PLAYGROUND_HEIGHT; i ++)
    {
        for (j = 0; j < PLAYGROUND_WIDTH; j ++)
        {
            dot = scene.playground[i][j];
            if ('c' == dot && curr_block != NULL)
            {
                //dot = 0;
                wattron(playground_box, A_BOLD);
                wattron(playground_box, COLOR_PAIR(curr_block->color));
                mvwaddch(playground_box, PLAYGROUND_HEIGHT - i, j * 2 + 1, ACS_CKBOARD);
                mvwaddch(playground_box, PLAYGROUND_HEIGHT - i, j * 2 + 2, ACS_CKBOARD);
                wattroff(playground_box, COLOR_PAIR(curr_block->color));
                wattroff(playground_box, A_BOLD);
            }
            else if('b' == dot)
            {
                wattron(playground_box, A_DIM);
                wattron(playground_box, COLOR_PAIR(8));
                mvwaddch(playground_box, PLAYGROUND_HEIGHT - i, j * 2 + 1, ' ');
                mvwaddch(playground_box, PLAYGROUND_HEIGHT - i, j * 2 + 2, ACS_DIAMOND);
                wattroff(playground_box,COLOR_PAIR(8));
                wattroff(playground_box, A_DIM);
            }
            else if ('e' == dot)
            {
                wattron(playground_box, A_DIM);
                wattron(playground_box, COLOR_PAIR(16));
                mvwaddch(playground_box, PLAYGROUND_HEIGHT - i, j * 2 + 1, ' ');
                mvwaddch(playground_box, PLAYGROUND_HEIGHT - i, j * 2 + 2, ACS_DIAMOND);
                wattroff(playground_box,COLOR_PAIR(16));
                wattroff(playground_box, A_DIM);
            }
            else if (0 == dot)
            {
                wattron(playground_box, A_DIM);
                wattron(playground_box, COLOR_PAIR(7));
                mvwaddch(playground_box, PLAYGROUND_HEIGHT - i, j * 2 + 1, ' ');
                mvwaddch(playground_box, PLAYGROUND_HEIGHT - i, j * 2 + 2, ACS_DIAMOND);
                wattroff(playground_box, COLOR_PAIR(7));
                wattroff(playground_box, A_DIM);
            }
        }
    }

    static char curr_trace_str[16];
    memset(curr_trace_str, 0, 16);

    wattron(trace_box, COLOR_PAIR(2));
    wattron(trace_box, A_BOLD);
    if (curr_block != NULL)
    {
        sprintf(curr_trace_str, "%d-><%2d : %2d>", curr_block->type, curr_block->pos.y, curr_block->pos.x);
    }

    mvwaddstr(trace_box, 4, 2, curr_trace_str);
    sprintf(curr_trace_str, "Status : %d", scene.status);
    mvwaddstr(trace_box, 5, 2, curr_trace_str);
    wattroff(trace_box, A_BOLD);
    wattroff(trace_box, COLOR_PAIR(2));

    wrefresh(playground_box);
    wrefresh(trace_box);

    return;
}

bool _curr_block_rotate(bool clockwise, enum block_direction_e *dir, int *tile)
{
    if (curr_block == NULL)
    {
        return FALSE;
    }

    enum block_direction_e m_dir;
    int i, j;
    int m_tile = try_rotate_block(curr_block, clockwise, &m_dir);
    for (i = 0; i < PLAYGROUND_HEIGHT; i ++)
    {
        if (check_block_solid(curr_block, m_tile, 0, 0, i, -1))
        {
            return FALSE;
        }

        if (check_block_solid(curr_block, m_tile, 0, 0, i, PLAYGROUND_WIDTH))
        {
            return FALSE;
        }

        for (j = 0; j < PLAYGROUND_WIDTH; j ++)
        {
            if ('b' == scene.playground[i][j] && check_block_solid(curr_block, m_tile, 0, 0, i, j))
            {
                return FALSE;
            }
        }
    }

    *dir = m_dir;
    *tile = m_tile;

    return TRUE;
}

bool _curr_block_left()
{
    if (curr_block == NULL)
    {
        return FALSE;
    }

    // Check edge
    int i, j;
    for (i = 0; i < PLAYGROUND_HEIGHT; i ++)
    {
        if (check_block_solid(curr_block, 0, 0, -1, i, -1))
        {
            return FALSE;
        }

        for (j = 0; j < PLAYGROUND_WIDTH; j ++)
        {
            if ('b' == scene.playground[i][j] && check_block_solid(curr_block,0, 0, -1, i, j))
            {
                return FALSE;
            }
        }
    }

    return TRUE; 
}

bool _curr_block_right()
{
    if (curr_block == NULL)
    {
        return FALSE;
    }

    // Check edge
    int i, j;
    for (i = 0; i < PLAYGROUND_HEIGHT; i ++)
    {
        if (check_block_solid(curr_block,0, 0, 1, i, PLAYGROUND_WIDTH))
        {
            return FALSE;
        }

        for (j = 0; j < PLAYGROUND_WIDTH; j ++)
        {
            if ('b' == scene.playground[i][j] && check_block_solid(curr_block, 0, 0, 1, i, j))
            {
                return FALSE;
            }
        }
    }

    return TRUE; 
}

bool _curr_block_down()
{
    if (curr_block == NULL)
    {
        return FALSE;
    }

    // Check edge
    int i, j;
    for (j = 0; j < PLAYGROUND_WIDTH; j ++)
    {
        if (check_block_solid(curr_block, 0, -1, 0, -1, j))
        {
            return FALSE;
        }

        for (i = 0; i < PLAYGROUND_HEIGHT; i ++)
        {
            if ('b' == scene.playground[i][j] && check_block_solid(curr_block, 0, -1, 0, i, j))
            {
                return FALSE;
            }
        }
    }

    return TRUE; 
}

void _curr_block_drop()
{
    // We do nothing now
    return;
}

void _curr_block_solidify()
{
    if (curr_block == NULL)
    {
        return;
    }

    int i, j;
    int dx, dy;
    for (i = 0; i < 4; i ++)
    {
        for (j = 0; j < 4; j ++)
        {
            dy = curr_block->pos.y + i;
            dx = curr_block->pos.x + j;
            if (dx >=0 && dx < PLAYGROUND_WIDTH && dy >= 0 && dy < PLAYGROUND_HEIGHT)
            {
                if ((1 << (i * 4 + (3 - j)) & curr_block->tile) > 0)
                {
                    scene.playground[dy][dx] = 'b';
                }
            }
        }
    }

    return;
}

void _check_score()
{
    static char tmp[PLAYGROUND_HEIGHT][PLAYGROUND_WIDTH];
    int i, j, ct, copied = 0, e = 0;
    memset(tmp, 0, sizeof(tmp));
    for (i = 0; i < PLAYGROUND_HEIGHT; i ++)
    {
        ct = 0;
        for (j = 0; j < PLAYGROUND_WIDTH; j ++)
        {
            tmp[copied][j] = scene.playground[i][j];
            if (scene.playground[i][j] == 'b')
            {
                ct ++;
            }
        }

        if (ct < PLAYGROUND_WIDTH)
        {
            copied ++;
        }
        else
        {
            e ++;
        }
    }

    memcpy(scene.playground, tmp, sizeof(tmp));
    switch (e)
    {
        case 4:
            scene.score += 10;
            break;
        case 3:
            scene.score += 12;
            break;
        case 2:
            scene.score += 8;
            break;
        case 0:
            break;
        default:
            scene.score += 5;
            break;
    }

    return;
}

// On timer event
void _on_timer()
{
    static unsigned long long int timer_counter = 0;

    // Asset blocks
    if (next_block == NULL)
    {
        next_block = new_block();
        _render_next();
    }

    if (curr_block == NULL)
    {
        curr_block = next_block;
        next_block = new_block();
        _render_next();
        curr_block->pos.x = (PLAYGROUND_WIDTH - 4) / 2;
        curr_block->pos.y = PLAYGROUND_HEIGHT - 4;
        scene.blocks ++;
        scene.score ++;
        if (STATUS_PREPARE == scene.status)
        {
            scene.status = STATUS_PLAYING;
        }

        _render_playground();
    }

    if (curr_block->dropped)
    {
        timer_counter = scene.speed - 1;
    }

    if (timer_counter % scene.speed == scene.speed - 1)
    {
        if (curr_block != NULL)
        {
            if (_curr_block_down())
            {
                curr_block->pos.y --;
            }
            else
            {
                _curr_block_solidify();
                if (PLAYGROUND_HEIGHT - 4 <= curr_block->pos.y)
                {
                    _render_playground();
                    scene.status = STATUS_OVER;
                    timer_delete(timer);

                    return;
                }

                del_block(curr_block);
                curr_block = NULL;

                // Check score
                _check_score();
                if (scene.score >= EGG_SCORE)
                {
                    _render_playground();
                    scene.status = STATUS_EGG;
                    timer_delete(timer);

                    return;
                }
            }
        }

        _render_playground();
    }

    _render_boxes();
    timer_counter ++;

    return;
}

// Main loop of game
void tetris_loop()
{
    struct sigevent evp;
    struct timespec spec;
    struct itimerspec tv;
    int ch;

    evp.sigev_value.sival_ptr = &timer;
    evp.sigev_notify = SIGEV_SIGNAL;
    evp.sigev_signo = SIGUSR1;
    signal(SIGUSR1, _on_timer);
    int ret = timer_create(CLOCK_MONOTONIC, &evp, &timer);
    if (ret)
    {
        perror("timer_create");
    }

    clock_gettime(CLOCK_MONOTONIC, &spec);
    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_nsec = 10000000;
    tv.it_value.tv_sec = spec.tv_sec + 0;
    tv.it_value.tv_nsec = spec.tv_nsec + 1000000;
    ret = timer_settime(timer, CLOCK_MONOTONIC, &tv, NULL);
    if (ret)
    {
        perror("timer_settime");
    }

    while (ch = getch())
    {
        if ('\033' == ch)
        {
            timer_delete(timer);
            break;
        }

        if (STATUS_OVER == scene.status || STATUS_EGG == scene.status)
        {
            break;
        }

        enum block_direction_e dir = BLOCK_DIR_0;
        int tile = 0;
        switch (ch)
        {
            case KEY_LEFT:
            case 'a':
            case 'A':
                // Block left
                if (_curr_block_left())
                {
                    curr_block->pos.x --;
                }

                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
                // Block right
                if (_curr_block_right())
                {
                    curr_block->pos.x ++;
                }

                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                // Block down
                if (_curr_block_down())
                {
                    curr_block->pos.y --;
                }

                break;
            case '\n':
            case ' ':
                // Block drop
                while (_curr_block_down())
                {
                    curr_block->pos.y --;
                }

                if (curr_block != NULL)
                {
                    curr_block->dropped = true;
                }

                break;
            case 'j':
            case 'J':
                // Rotate -90
                if (_curr_block_rotate(FALSE, &dir, &tile))
                {
                    curr_block->direction = dir;
                    curr_block->tile = tile;
                }

                break;
            case 'k':
            case 'K':
                // Rotate + 90
                if (_curr_block_rotate(TRUE, &dir, &tile))
                {
                    curr_block->direction = dir;
                    curr_block->tile = tile;
                }

                break;
            default:
                // Do nothing
                break;
        }

        _render_playground();
    }

    return;
}

void tetris_splash()
{
    WINDOW *splash_box = newwin(
        SPLASH_TITLE_HEIGHT,
        SPLASH_TITLE_WIDTH,
        5,
        (scene.win_width - SPLASH_TITLE_WIDTH) / 2);
    wbkgd(splash_box, COLOR_PAIR(28));
    wattron(splash_box, COLOR_PAIR(23));
    box(splash_box, 0, 0);
    wattroff(splash_box, COLOR_PAIR(23));

    _splash_title_char(splash_box, title_t, 10, 2,  2, 27);
    _splash_title_char(splash_box, title_e, 10, 2, 15, 26);
    _splash_title_char(splash_box, title_t, 10, 2, 28, 27);
    _splash_title_char(splash_box, title_r, 10, 2, 41, 25);
    _splash_title_char(splash_box, title_i, 10, 2, 54, 30);
    _splash_title_char(splash_box, title_s, 10, 2, 67, 29);

    wattron(splash_box, A_BOLD);
    wattron(splash_box, COLOR_PAIR(18));
    mvwaddstr(splash_box, 14, 32, "Tetris console game");
    mvwaddstr(splash_box, 15, 30, "Version ");
    mvwaddstr(splash_box, 15, 38, APP_VERSION);
    wattroff(splash_box, COLOR_PAIR(18));
    wattron(splash_box, COLOR_PAIR(19));
    mvwaddstr(splash_box, 16, 27, "By Dr.NP <conan.np@gmail.com>");
    wattroff(splash_box, COLOR_PAIR(19));
    wattroff(splash_box, A_BOLD);

    wrefresh(splash_box);

    // Help topic
    getch();
    WINDOW *topic_box = newwin(
        TOPIC_BOX_HEIGHT,
        TOPIC_BOX_WIDTH,
        24,
        (scene.win_width - TOPIC_BOX_WIDTH) / 2);
    wbkgd(topic_box, COLOR_PAIR(7));
    wattron(topic_box, COLOR_PAIR(7));
    box(topic_box, 0, 0);
    mvwaddstr(topic_box, 2, 4, "<KEY-LEFT> / 'a' for move left");
    mvwaddstr(topic_box, 3, 4, "<KEY-RIGHT> / 'd' for move right");
    mvwaddstr(topic_box, 4, 4, "<KEY-DOWN> / 's' for move down");
    mvwaddstr(topic_box, 5, 4, "j / k for rotation");
    mvwaddstr(topic_box, 6, 4, "<SPACE> / <ENTER> for drop");
    mvwaddstr(topic_box, 8, 9, "Any key to continue ...");
    wattroff(topic_box, COLOR_PAIR(7));

    wrefresh(topic_box);

    // Hide me
    getch();
    wclear(splash_box);
    wclear(topic_box);
    wbkgd(splash_box, COLOR_PAIR(1));
    wrefresh(splash_box);
    wrefresh(topic_box);
    delwin(splash_box);
    delwin(topic_box);

    return;
}

void tetris_interface()
{
    // Playground
    playground_box = newwin(
        PLAYGROUND_HEIGHT + 2,
        PLAYGROUND_WIDTH * 2 + 2,
        4,
        (scene.win_width - PLAYGROUND_WIDTH * 2) / 2 - 1);
    wbkgd(playground_box, COLOR_PAIR(7));
    wattron(playground_box, COLOR_PAIR(4));
    box(playground_box, 0, 0);
    wattroff(playground_box, COLOR_PAIR(4));

    // Score
    score_box = newwin(
        MSG_BOX_HEIGHT,
        MSG_BOX_WIDTH,
        5,
        (scene.win_width - PLAYGROUND_WIDTH * 2) / 2 - 3 - MSG_BOX_WIDTH);
    wbkgd(score_box, COLOR_PAIR(7));
    wattron(score_box, COLOR_PAIR(4));
    box(score_box, 0, 0);
    wattroff(score_box, COLOR_PAIR(4));
    wattron(score_box, A_BOLD);
    wattron(score_box, COLOR_PAIR(3));
    mvwaddstr(score_box, 1, 3, "SCORE");
    wattroff(score_box, COLOR_PAIR(3));
    wattroff(score_box, A_BOLD);

    // Level
    level_box = newwin(
        MSG_BOX_HEIGHT,
        MSG_BOX_WIDTH,
        6 + MSG_BOX_HEIGHT,
        (scene.win_width - PLAYGROUND_WIDTH * 2) / 2 - 3 - MSG_BOX_WIDTH);
    wbkgd(level_box, COLOR_PAIR(7));
    wattron(level_box, COLOR_PAIR(4));
    box(level_box, 0, 0);
    wattroff(level_box, COLOR_PAIR(4));
    wattron(level_box, A_BOLD);
    wattron(level_box, COLOR_PAIR(3));
    mvwaddstr(level_box, 1, 3, "LEVEL");
    wattroff(level_box, COLOR_PAIR(3));
    wattroff(level_box, A_BOLD);

    // Blocks
    blocks_box = newwin(
        MSG_BOX_HEIGHT,
        MSG_BOX_WIDTH,
        7 + MSG_BOX_HEIGHT * 2,
        (scene.win_width - PLAYGROUND_WIDTH * 2) / 2 - 3 - MSG_BOX_WIDTH);
    wbkgd(blocks_box, COLOR_PAIR(7));
    wattron(blocks_box, COLOR_PAIR(4));
    box(blocks_box, 0, 0);
    wattroff(blocks_box, COLOR_PAIR(4));
    wattron(blocks_box, A_BOLD);
    wattron(blocks_box, COLOR_PAIR(3));
    mvwaddstr(blocks_box, 1, 3, "BLOCKS");
    wattroff(blocks_box, COLOR_PAIR(3));
    wattroff(blocks_box, A_BOLD);

    // Next
    next_box = newwin(
        NEXT_BOX_HEIGHT,
        NEXT_BOX_WIDTH,
        5,
        (scene.win_width + PLAYGROUND_WIDTH * 2) / 2 + 3);
    wbkgd(next_box, COLOR_PAIR(7));
    wattron(next_box, COLOR_PAIR(4));
    box(next_box, 0, 0);
    wattroff(next_box, COLOR_PAIR(4));
    wattron(next_box, A_BOLD);
    wattron(next_box, COLOR_PAIR(3));
    mvwaddstr(next_box, 1, 3, "NEXT");
    wattroff(next_box, COLOR_PAIR(3));
    wattroff(next_box, A_BOLD);

    // Trace
    trace_box = newwin(
        TRACE_BOX_HEIGHT,
        TRACE_BOX_WIDTH,
        6 + NEXT_BOX_HEIGHT,
        (scene.win_width + PLAYGROUND_WIDTH * 2) / 2 + 3);
    wbkgd(trace_box, COLOR_PAIR(7));
    wattron(trace_box, COLOR_PAIR(4));
    box(trace_box, 0, 0);
    wattroff(trace_box, COLOR_PAIR(4));
    wattron(trace_box, A_BOLD);
    wattron(trace_box, COLOR_PAIR(3));
    mvwaddstr(trace_box, 1, 3, "TRACE");
    wattroff(trace_box, COLOR_PAIR(3));
    wattroff(trace_box, A_BOLD);
    
    wrefresh(playground_box);
    wrefresh(score_box);
    wrefresh(level_box);
    wrefresh(blocks_box);
    wrefresh(next_box);
    wrefresh(trace_box);

    return;
}

void tetris_gameover()
{
    wbkgd(playground_box, COLOR_PAIR(23));
    wattron(playground_box, COLOR_PAIR(18));
    wclear(playground_box);
    box(playground_box, 0, 0);
    wattroff(playground_box, COLOR_PAIR(18));

    _splash_title_char(playground_box, smile_failure, 10, 4, PLAYGROUND_WIDTH - 10, 25);

    wattron(playground_box, A_BOLD);
    wattron(playground_box, COLOR_PAIR(18));
    mvwaddstr(playground_box, 16, 2, "You did perfect!");
    mvwaddstr(playground_box, 18, 2, "God programmer wish you");
    mvwaddstr(playground_box, 19, 2, "0 warning(s), 0 error(s).");
    mvwaddstr(playground_box, 21, 2, "HAHAHAHAHA...");
    wattroff(playground_box, COLOR_PAIR(18));
    wattroff(playground_box, A_BOLD);

    wrefresh(playground_box);

    getch();

    return;
}

void tetris_egg()
{
    wbkgd(playground_box, COLOR_PAIR(23));
    wattron(playground_box, COLOR_PAIR(18));
    wclear(playground_box);
    box(playground_box, 0, 0);
    wattroff(playground_box, COLOR_PAIR(18));

    _splash_title_char(playground_box, title_1, 7, 10, PLAYGROUND_WIDTH - 9, 26);
    _splash_title_char(playground_box, title_0, 7, 10, PLAYGROUND_WIDTH + 2, 27);
    _splash_title_char(playground_box, title_2, 7, 18, PLAYGROUND_WIDTH - 9, 30);
    _splash_title_char(playground_box, title_4, 7, 18, PLAYGROUND_WIDTH + 2, 29);
    _splash_title_char(playground_box, title_heart, 8, 2, PLAYGROUND_WIDTH - 8, 25);

    wattron(playground_box, A_BOLD);
    wattron(playground_box, COLOR_PAIR(19));
    mvwaddstr(playground_box, 27, 4, "Romanticism of PROGRAMMERS");
    wattroff(playground_box, COLOR_PAIR(19));
    wattron(playground_box, COLOR_PAIR(22));
    mvwaddstr(playground_box, 28, 2, "By Dr.NP <conan.np@gmail.com>.");
    wattroff(playground_box, COLOR_PAIR(22));
    wattroff(playground_box, A_BOLD);

    wrefresh(playground_box);

    getch();

    return;
}

void tetris_quit()
{
    wbkgd(playground_box, COLOR_PAIR(7));
    wattron(playground_box, COLOR_PAIR(2));
    wclear(playground_box);
    box(playground_box, 0, 0);
    wattroff(playground_box, COLOR_PAIR(2));
    wattron(playground_box, A_BOLD);
    wattron(playground_box, COLOR_PAIR(3));
    mvwaddstr(playground_box, 3, 2, "And God created");
    mvwaddstr(playground_box, 4, 2, "a perfect world.");
    mvwaddstr(playground_box, 5, 2, "And it was good");
    mvwaddstr(playground_box, 6, 2, "so good etc etc etc...");
    mvwaddstr(playground_box, 9, 2, "And God created");
    mvwaddstr(playground_box, 10, 2, "man in his own image.");
    mvwaddstr(playground_box, 11, 2, "And nothings");
    mvwaddstr(playground_box, 12, 2, "ever been good since.");
    mvwaddstr(playground_box, 15, 2, "And was this");
    mvwaddstr(playground_box, 16, 2, "God’s First Mistake?");
    mvwaddstr(playground_box, 17, 2, "No Satan had already");
    mvwaddstr(playground_box, 18, 2, "been created");
    mvwaddstr(playground_box, 19, 2, "an angel then known by");
    mvwaddstr(playground_box, 20, 2, "another name.");
    wattroff(playground_box, COLOR_PAIR(3));
    wattron(playground_box, COLOR_PAIR(6));
    mvwaddstr(playground_box,24, 12, "GOODBYE...");
    wattroff(playground_box, COLOR_PAIR(6));
    wattroff(playground_box, A_BOLD);

    wrefresh(playground_box);

    getch();

    return;
}

void tetris_main()
{
    bkgd(COLOR_PAIR(0));
    attron(COLOR_PAIR(7));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(7));
    refresh();

    return;
}

extern char *optarg;

int main(int argc, char *argv[])
{
    memset(&scene, 0, sizeof(struct tetris_scene_t));

    int c;
    int level = DEFAULT_TETRIS_LEVEL;
    while (-1 != (c = getopt(argc, argv, "l:h")))
    {
        switch (c)
        {
            case 'l' :
                level = atoi(optarg);
                if (level > MAX_TETRIS_LEVEL)
                {
                    level = MAX_TETRIS_LEVEL;
                }

                if (level < MIN_TETRIS_LEVEL)
                {
                    level = MIN_TETRIS_LEVEL;
                }

                break;
            case 'h' :
                // Help topic
                printf("%s - %s\n\n", APP_NAME, APP_VERSION);
                printf("<KEY-LEFT / w> <KEY-RIGHT / d> <KEY-DOWN / s> for block movment\n");
                printf("<j> <k> for block rotation\n");
                printf("<KEY-SPACE> <KEY-ENTER> for fall off\n");
                printf("<KEY-ESC> to quit game\n");

                printf("\t-s : Game level [1 - 9], default <%d>\n", DEFAULT_TETRIS_LEVEL);
                printf("\t-h : Print this topic\n");

                exit(0);

                break;
            default :
                break;
        }
    }

    scene.level = level;
    scene.speed = calculate_speed(level);
    scene.status = STATUS_PREPARE;

    initscr();
    check_window();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(-1);
    start_color();
    color_pairs();

    // Draw scene
    tetris_main();
    tetris_splash();
    tetris_interface();

    // Play loop
    _render_boxes();
    _render_playground();
    tetris_loop();
    if (STATUS_OVER == scene.status)
    {
        tetris_gameover();
    }
    else if (STATUS_EGG == scene.status)
    {
        tetris_egg();
    }
    else
    {
        tetris_quit();
    }

    curs_set(2);
    echo();
    //nocbreak();
    endwin();

    return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
