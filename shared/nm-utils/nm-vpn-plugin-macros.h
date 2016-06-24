/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright 2016 Red Hat, Inc.
 */

#ifndef __NM_VPN_PLUGIN_MACROS_H__
#define __NM_VPN_PLUGIN_MACROS_H__

#include <syslog.h>

static inline const char *
nm_utils_syslog_to_str (int syslog_level)
{
	/* Maps the levels the same way as NetworkManager's nm-logging.c does */
	if (syslog_level >= LOG_DEBUG)
		return "<trace>";
	if (syslog_level >= LOG_INFO)
		return "<debug>";
	if (syslog_level >= LOG_NOTICE)
		return "<info>";
	if (syslog_level >= LOG_WARNING)
		return "<warn>";
	return "<error>";
}

#endif /* __NM_VPN_PLUGIN_MACROS_H__ */

