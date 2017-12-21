/*
 * date.c -- functions for dealing with dates.
 *
 * Copyright (C)1999-2018 Mark Simpson <damned@theworld.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can either send email to this
 * program's maintainer or write to: The Free Software Foundation,
 * Inc.; 59 Temple Place, Suite 330; Boston, MA 02111-1307, USA.
 *
 */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

#include "date.h"
#include "util.h"

/* Array of days of the week for translating a date */
const char* day_of_week[] = { "Sun", "Mon", "Tue",
                              "Wed", "Thu", "Fri", "Sat" };

extern const char *
dow_str(int dow)
{
    assert (dow >= 0 && dow <= 6);
    return day_of_week[dow];
}

const char *
date_to_str (struct date *dt)
{
    static char buf[32];
    snprintf (buf, sizeof(buf), "%s %04d/%02d/%02d %02d:%02d:%02d",
             dow_str(dt->dow),
             dt->year, dt->month, dt->day,
             dt->hour, dt->min, dt->sec);
    buf[sizeof(buf)-1] = '\0';
    return buf;
}

void
date_read (struct date *dt, const unsigned char *buf)
{
    size_t i = 0;
    unsigned char *tmp = (unsigned char *)buf;
    dt->year = GETINT16 (tmp + i); i += sizeof (uint16);
    dt->month = GETINT16 (tmp + i); i += sizeof (uint16);
    dt->day = GETINT16 (tmp + i); i += sizeof (uint16);
    dt->hour = GETINT16 (tmp + i); i += sizeof (uint16);
    dt->min = GETINT16 (tmp + i); i += sizeof (uint16);
    dt->sec = GETINT16 (tmp + i); i += sizeof (uint16);
    dt->dow = GETINT16 (tmp + i); i += sizeof (uint16);
}
