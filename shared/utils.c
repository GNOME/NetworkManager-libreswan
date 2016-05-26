/* NetworkManager-libreswan -- Network Manager Libreswan plugin
 *
 * Dan Williams <dcbw@redhat.com>
 * Avesh Agarwal <avagarwa@redhat.com>
 * Lubomir Rintel <lkundrak@v3.sk>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2010 - 2015 Red Hat, Inc.
 */

#include "nm-default.h"

#include "utils.h"

#include <unistd.h>
#include <string.h>

gboolean debug = FALSE;

void
nm_libreswan_config_write (gint fd,
                           NMConnection *connection,
                           const char *bus_name,
                           gboolean openswan)
{
	NMSettingVpn *s_vpn = nm_connection_get_setting_vpn (connection);
	const char *con_name;
	const char *props_username;
	const char *default_username;
	const char *phase1_alg_str;
	const char *phase2_alg_str;
	const char *leftid;

	/* We abuse the presence of bus name to decide if we're exporting
	 * the connection or actually configuring Pluto. */
	if (bus_name)
		con_name = nm_connection_get_uuid (connection);
	else
		con_name = nm_connection_get_id (connection);

	g_assert (fd >= 0);
	g_assert (s_vpn);
	g_assert (con_name);

	leftid = nm_setting_vpn_get_data_item (s_vpn, NM_LIBRESWAN_LEFTID);

	write_config_option (fd, "conn %s", con_name);
	if (leftid) {
		write_config_option (fd, " aggrmode=yes");
		write_config_option (fd, " leftid=@%s", leftid);
	}
	write_config_option (fd, " authby=secret");
	write_config_option (fd, " left=%%defaultroute");
	write_config_option (fd, " leftxauthclient=yes");
	write_config_option (fd, " leftmodecfgclient=yes");

	if (bus_name)
		write_config_option (fd, " leftupdown=\"" NM_LIBRESWAN_HELPER_PATH " --bus-name %s\"", bus_name);

	default_username = nm_setting_vpn_get_user_name (s_vpn);
	props_username = nm_setting_vpn_get_data_item (s_vpn, NM_LIBRESWAN_LEFTXAUTHUSER);
	if (props_username && strlen (props_username))
		write_config_option (fd, " leftxauthusername=%s", props_username);
	else if (default_username && strlen (default_username))
		write_config_option (fd, " leftxauthusername=%s", default_username);

	write_config_option (fd, " right=%s", nm_setting_vpn_get_data_item (s_vpn, NM_LIBRESWAN_RIGHT));
	write_config_option (fd, " remote_peer_type=cisco");
	write_config_option (fd, " rightxauthserver=yes");
	write_config_option (fd, " rightmodecfgserver=yes");
	write_config_option (fd, " modecfgpull=yes");
	write_config_option (fd, " rightsubnet=0.0.0.0/0");

	phase1_alg_str = nm_setting_vpn_get_data_item (s_vpn, NM_LIBRESWAN_IKE);
	if (!phase1_alg_str || !strlen (phase1_alg_str))
		write_config_option (fd, " ike=aes-sha1");
	else
		write_config_option (fd, " ike=%s", phase1_alg_str);

	phase2_alg_str = nm_setting_vpn_get_data_item (s_vpn, NM_LIBRESWAN_ESP);
	if (!phase2_alg_str || !strlen (phase2_alg_str))
		write_config_option (fd, " esp=aes-sha1;modp1024");
	else
		write_config_option (fd, " esp=%s", phase2_alg_str);

	write_config_option (fd, " rekey=yes");
	write_config_option (fd, " salifetime=24h");
	write_config_option (fd, " ikelifetime=24h");
	write_config_option (fd, " keyingtries=1");
	if (!openswan && g_strcmp0 (nm_setting_vpn_get_data_item (s_vpn, NM_LIBRESWAN_VENDOR), "Cisco") == 0)
		write_config_option (fd, " cisco-unity=yes");

	/* openswan requires a terminating \n (otherwise it segfaults) while
	 * libreswan fails parsing the configuration if you include the \n.
	 * WTF?
	 */
	write_config_option_newline (fd, (openswan || !bus_name), " auto=add");

}
