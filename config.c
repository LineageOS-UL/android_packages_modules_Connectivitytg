/*
 * Copyright 2011 Daniel Drown
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * config.c - configuration settings
 */

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cutils/config_utils.h>
#include <netutils/checksum.h>
#include <netutils/ifc.h>

#include "clatd.h"
#include "config.h"
#include "getaddr.h"
#include "logging.h"

struct clat_config Global_Clatd_Config;

/* function: ipv6_prefix_equal
 * compares the prefixes two ipv6 addresses. assumes the prefix lengths are both /64.
 *   a1 - first address
 *   a2 - second address
 *   returns: 0 if the subnets are different, 1 if they are the same.
 */
int ipv6_prefix_equal(struct in6_addr *a1, struct in6_addr *a2) { return !memcmp(a1, a2, 8); }

/* function: read_config
 * reads the config file and parses it into the global variable Global_Clatd_Config. returns 0 on
 * failure, 1 on success
 *   file             - filename to parse
 *   uplink_interface - interface to use to reach the internet and supplier of address space
 */
int read_config(const char *file, const char *uplink_interface) {
  cnode *root   = config_node("", "");

  if (!root) {
    logmsg(ANDROID_LOG_FATAL, "out of memory");
    return 0;
  }

  memset(&Global_Clatd_Config, '\0', sizeof(Global_Clatd_Config));

  config_load_file(root, file);
  if (root->first_child == NULL) {
    logmsg(ANDROID_LOG_FATAL, "Could not read config file %s", file);
    goto failed;
  }

  Global_Clatd_Config.default_pdp_interface = strdup(uplink_interface);
  if (!Global_Clatd_Config.default_pdp_interface) goto failed;

  Global_Clatd_Config.ipv4_local_prefixlen = 29;

  return 1;

failed:
  free(root);
  return 0;
}
