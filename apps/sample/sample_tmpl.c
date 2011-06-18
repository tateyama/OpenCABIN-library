/* OpenCABIN Library

   Copyright (C) 2006,2007 KGT Inc.

    This file is part of the OpenCABIN Library developed
    under CnC project sponsored by National Institute of
    Information and Communications Technology (NICT).

    The OpenCABIN Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    The OpenCABIN Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.
*/
//== master ================================================
// Please return master proc data (mprocdata), or 0
void *
minit(int ac, char **av)
{
    return 0;
}

void
midle(void *mprocdata)
{
}

void
mrepeat(void *mprocdata)
{
}

void
mcleanup(void *mprocdata)
{
}

//== renderer ================================================
// Please return procdata, or 0
void *
rinit(int ac, char **av)
{
    return 0;
}

void
ginit(void *procdata)
{
}

void
gdraw(void *procdata)
{
}

void
gidle(void *procdata)
{
}

void
rcleanup(void *procdata)
{
}
