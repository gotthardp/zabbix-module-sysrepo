/*
 * Zabbix Module for Sysrepo Monitoring
 * Copyright (C) 2022  Petr Gotthard <petr.gotthard@centrum.cz>
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
 */

#include <sysrepo.h>
#include "module.h"

// the variable keeps timeout setting for item processing
static int item_timeout = 0;
// connection to sysrepo
static sr_conn_ctx_t *connection = NULL;
static sr_session_ctx_t *session = NULL;

static int sysrepo_get(AGENT_REQUEST *request, AGENT_RESULT *result);

// return version number of the module interface
int
zbx_module_api_version(void)
{
    return ZBX_MODULE_API_VERSION;
}

// set timeout [seconds] value for processing of items
void
zbx_module_item_timeout(int timeout)
{
    item_timeout = timeout;
}

// return list of item keys supported by the module
ZBX_METRIC
*zbx_module_item_list(void)
{
    static ZBX_METRIC keys[] =
    {
        {"sysrepo.get", CF_HAVEPARAMS, sysrepo_get, "/ietf-system:system-state/platform/os-name"},
        {NULL}
    };

    return keys;
}

static int
sysrepo_get(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    const char *xpath;
    sr_val_t *val;
    int rc;

    if (1 != request->nparam) {
        SET_MSG_RESULT(result, strdup("Invalid number of parameters."));
        return SYSINFO_RET_FAIL;
    }

    xpath = get_rparam(request, 0);

    if ((rc = sr_get_item(session, xpath, 0, &val)) != SR_ERR_OK) {
        SET_MSG_RESULT(result, strdup(sr_strerror(rc)));
        return SYSINFO_RET_FAIL;
    }

    switch(val->type) {
#define CASE_STR_TYPE(TYPE, FIELD) \
    case TYPE: \
        SET_STR_RESULT(result, strdup(val->data.FIELD)); \
        break;
    CASE_STR_TYPE(SR_BINARY_T, binary_val)
    CASE_STR_TYPE(SR_ENUM_T, enum_val)
    CASE_STR_TYPE(SR_STRING_T, string_val)

#define CASE_DBL_TYPE(TYPE, FIELD) \
    case TYPE: \
        SET_DBL_RESULT(result, val->data.FIELD); \
        break;
    CASE_DBL_TYPE(SR_INT8_T, int8_val)
    CASE_DBL_TYPE(SR_INT16_T, int16_val)
    CASE_DBL_TYPE(SR_INT32_T, int32_val)
    CASE_DBL_TYPE(SR_UINT8_T, uint8_val)
    CASE_DBL_TYPE(SR_UINT16_T, uint16_val)
    CASE_DBL_TYPE(SR_UINT32_T, uint32_val)

    case SR_UINT64_T:
        SET_UI64_RESULT(result, val->data.uint64_val);
        break;
    }

    sr_free_val(val);
    return SYSINFO_RET_OK;
}

// called on agent startup
int
zbx_module_init(void)
{
    int rc;

    if ((rc = sr_connect(SR_CONN_DEFAULT, &connection)))
        return ZBX_MODULE_FAIL;

    if ((rc = sr_session_start(connection, SR_DS_OPERATIONAL, &session))) {
        sr_disconnect(connection);
        return ZBX_MODULE_FAIL;
    }

    return ZBX_MODULE_OK;
}

// called on agent shutdown
int
zbx_module_uninit(void)
{
    sr_disconnect(connection);

    return ZBX_MODULE_OK;
}

// export historical data
ZBX_HISTORY_WRITE_CBS
zbx_module_history_write_cbs(void)
{
    static ZBX_HISTORY_WRITE_CBS callbacks = { NULL };

    return callbacks;
}
