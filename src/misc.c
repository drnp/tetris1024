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
 * @file misc.c
 * @author Dr.NP <conan.np@gmail.com>
 * @since 10/20/2021
 */

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int get_random()
{
    static char buf[4];
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
    {
        return 0;
    }

    ssize_t r = read(fd, buf, 4);
    if (r < 0)
    {
        return 0;
    }

    unsigned int ret =
        ((int)buf[0]) << 24 |
        ((int)buf[1]) << 16 |
        ((int)buf[2]) << 8 |
        ((int)buf[3]);
    close(fd);

    return ret;
}

unsigned long long int calculate_speed(int level)
{
    switch (level)
    {
        case 1:
            return 200;
        case 2:
            return 150;
        case 3:
            return 100;
        case 4:
            return 70;
        case 5:
            return 55;
        case 6:
            return 40;
        case 7:
            return 25;
        case 8:
            return 15;
        case 9:
        default:
            return 10;
    };
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
