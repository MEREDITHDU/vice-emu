/*
 * console.c - Console access interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textwin.h"

#include "console.h"
#include "ui.h"
#include "uimsgwin.h"
#include "utils.h"
#include "video.h"


static FILE *mon_input, *mon_output;

static int WimpCmdBlock[64];



int console_init(void)
{
    return 0;
}

console_t *console_open(const char *id)
{
    console_t *console;

    mon_input = stdin; mon_output = stdout;

    console = xmalloc(sizeof(console_t));

    console->console_xres = 80;
    console->console_yres = 25;
    console->console_can_stay_open = 0;

    if (FullScreenMode == 1)
    {
      Wimp_CommandWindow((int)"Vice Monitor");
    }
    else
    {
      ui_message_window_open(msg_win_monitor, "Vice Monitor", "WIMPLIB LINE EDITOR", console->console_xres, console->console_yres);
    }

    return console;
}

int console_close(console_t *log)
{
    if (!ui_message_window_is_open(msg_win_monitor))
    {
      Wimp_CommandWindow(-1);
    }
    else
    {
      ui_message_window_close(msg_win_monitor);
    }

    free(log);

    return 0;
}

int console_out(console_t *log, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    if (!ui_message_window_is_open(msg_win_monitor))
    {
      vfprintf(stdout, format, ap);
    }
    else
    {
      text_window_t *tw = ui_message_get_text_window(msg_win_monitor);

      if (tw != NULL)
      {
        static char buffer[1024];

        vsprintf(buffer, format, ap);
        if (textwin_add_text(tw, buffer) > 0)
        {
          textwin_caret_to_end(tw);
          while (ui_poll_core(WimpCmdBlock) != 0) ;
        }
      }
    }
    return 0;
}

char *console_in(console_t *log)
{
    char *p = (char*)xmalloc(1024);

    fflush(mon_output);
    fgets(p, 1024, mon_input);

    /* Remove trailing newlines.  */
    {
        int len;

        for (len = strlen(p);
             len > 0 && (p[len - 1] == '\r'
                         || p[len - 1] == '\n');
             len--)
            p[len - 1] = '\0';
    }

    return p;
}

int console_close_all(void)
{
    return 0;
}

