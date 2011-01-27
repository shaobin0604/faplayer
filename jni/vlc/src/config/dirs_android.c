/*****************************************************************************
 * dirs_android.c: android directories configuration
 *****************************************************************************
 * Copyright (C) 2001-2007 the VideoLAN team
 * Copyright © 2007-2009 Rémi Denis-Courmont
 *
 * Authors: Gildas Bazin <gbazin@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#warning "this module needs more work!"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>

#include "../libvlc.h"
#include <vlc_charset.h>
#include "config/configuration.h"

/**
 * Determines the shared data directory
 *
 * @return a nul-terminated string or NULL. Use free() to release it.
 */
char *config_GetDataDirDefault (void)
{
    return strdup (DATA_PATH);
}

/**
 * Determines the architecture-dependent data directory
 *
 * @return a string (always succeeds).
 */
const char *config_GetLibDir (void)
{
    return PKGLIBDIR;
}

/**
 * Determines the system configuration directory.
 *
 * @return a string (always succeeds).
 */
const char *config_GetConfDir( void )
{
    return SYSCONFDIR;
}

static char *config_GetHomeDir (void)
{
    return strdup("/sdcard");
}

static char *config_GetAppDir (const char *xdg_name, const char *xdg_default)
{
    char *ret;

    ret = (char*) malloc(260);
    if (!ret)
        return ret;
    sprintf(ret, "/data/data/%s/cache/%s", PACKAGENAME, xdg_default);
    return ret;
}

static char *config_GetTypeDir (const char *xdg_name)
{
    char *ret;

    ret = (char*) malloc(260);
    if (!ret)
        return ret;
    sprintf(ret, "/sdcard/%s", xdg_name);
    return ret;
}


char *config_GetUserDir (vlc_userdir_t type)
{
    switch (type)
    {
        case VLC_HOME_DIR:
            break;
        case VLC_CONFIG_DIR:
            return config_GetAppDir (NULL, "etc");
        case VLC_DATA_DIR:
            return config_GetAppDir (NULL, "share");
        case VLC_CACHE_DIR:
            return config_GetAppDir (NULL, "cache");

        case VLC_DESKTOP_DIR:
            return config_GetTypeDir ("DESKTOP");
        case VLC_DOWNLOAD_DIR:
            return config_GetTypeDir ("DOWNLOAD");
        case VLC_TEMPLATES_DIR:
            return config_GetTypeDir ("TEMPLATE");
        case VLC_PUBLICSHARE_DIR:
            return config_GetTypeDir ("PUBLIC");
        case VLC_DOCUMENTS_DIR:
            return config_GetTypeDir ("DOCUMENT");
        case VLC_MUSIC_DIR:
            return config_GetTypeDir ("MUSIC");
        case VLC_PICTURES_DIR:
            return config_GetTypeDir ("PICTURE");
        case VLC_VIDEOS_DIR:
            return config_GetTypeDir ("VIDEO");
    }
    return config_GetHomeDir ();
}
