/**
 ******************************************************************************
 *
 * @file       %FILENAME%
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2014.
 * @addtogroup [Group]
 * @{
 * @addtogroup %CLASS%
 * @{
 * @brief [Brief]
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef UBX_AUTOCONFIG_H_
#define UBX_AUTOCONFIG_H_
#include "UBX.h"
#include <stdint.h>
#include <stdbool.h>

// defines
// TODO: NEO8 max rate is for Rom version, flash is limited to 10Hz, need to handle that.
#define UBX_MAX_RATE_VER8 18
#define UBX_MAX_RATE_VER7 10
#define UBX_MAX_RATE      5

// reset to factory (and 9600 baud), and baud changes are not acked
// it could be 31 (full PIOS buffer) + 60 (gnss) + overhead bytes at 240 bytes per second
// = .42 seconds for the longest sentence to be fully transmitted
// and then it may have to do something like erase flash before replying...

// at low baud rates and high data rates the ubx gps simply must drop some outgoing data
// and when a lot of data is being dropped, the MON VER reply often gets dropped
// on the other hand, uBlox documents that some versions discard data that is over a second old
// implying that it could be over 1 second before a reply is received
// later versions dropped this and drop data when the send buffer is full and that could be even longer
// rather than have long timeouts, we will let timeouts * retries handle that if it happens

// known to work
#if 0
// timeout for ack reception
#define UBX_REPLY_TIMEOUT             (400 * 1000) 50 has 1 retry 100 has 1 retry 0 retries at 400, 4@200-200@19200 1@400-200@115200-onetime 1@400@19200
// timeout for a settings save, in case it has to erase flash
#define UBX_SAVE_WAIT_TIME            (1000 * 1000)
// max retries in case of timeout
#define UBX_MAX_RETRIES               5
// pause between each verifiably correct configuration step
#define UBX_VERIFIED_STEP_WAIT_TIME   (20 * 1000)
// pause between each unverifiably correct configuration step
#define UBX_UNVERIFIED_STEP_WAIT_TIME (400 * 1000) was 200
#endif

// need about 60 + 8 char times for the largest config packet to be sent and acked
// that is about 35ms at 19200 baud, figure 40ms
// but then there is the hairy problem of the dropped packets that occur
// when there is more data to send than there is bandwidth
// we generally avoid turning on the OP data packets for configurations of 9600 baud or lower

#if 0
// timeout for ack reception
#define UBX_REPLY_TIMEOUT             (500 * 1000)
// timeout for a settings save, in case it has to erase flash
#define UBX_REPLY_TO_SAVE_TIMEOUT     (3000 * 1000)
// max retries in case of timeout
#define UBX_MAX_RETRIES               5
// pause between each verifiably correct configuration step
#define UBX_VERIFIED_STEP_WAIT_TIME   (50 * 1000)
// pause between each unverifiably correct configuration step
#define UBX_UNVERIFIED_STEP_WAIT_TIME (500 * 1000)
#else
// timeouts for ack reception
// 1@265-265@19200
// 0@272-272
#define UBX_REPLY_TIMEOUT             (280 * 1000)
// timeout for a settings save, in case it has to erase flash?
#define UBX_SAVE_WAIT_TIME            (1000 * 1000)
// max retries in case of timeout
#define UBX_MAX_RETRIES               5
// max time for ubx parser to respond to MON_VER
#define UBX_PARSER_TIMEOUT            (950 * 1000)
// pause between each unverifiable (no ack/nak) configuration step
#define UBX_UNVERIFIED_STEP_WAIT_TIME (280 * 1000)
#endif
#define UBX_CFG_CFG_OP_STORE_SETTINGS \
    (UBX_CFG_CFG_SETTINGS_IOPORT | \
     UBX_CFG_CFG_SETTINGS_MSGCONF | \
     UBX_CFG_CFG_SETTINGS_NAVCONF)
#define UBX_CFG_CFG_OP_CLEAR_SETTINGS ((~UBX_CFG_CFG_OP_STORE_SETTINGS) & UBX_CFG_CFG_SETTINGS_ALL)
// don't clear rinv as that is just text as configured by the owner
#define UBX_CFG_CFG_OP_RESET_SETTINGS \
    (UBX_CFG_CFG_SETTINGS_IOPORT | \
     UBX_CFG_CFG_SETTINGS_MSGCONF | \
     UBX_CFG_CFG_SETTINGS_INFMSG | \
     UBX_CFG_CFG_SETTINGS_NAVCONF | \
     UBX_CFG_CFG_SETTINGS_TPCONF | \
     UBX_CFG_CFG_SETTINGS_SFDRCONF | \
     UBX_CFG_CFG_SETTINGS_ANTCONF)

// types
typedef enum {
    UBX_AUTOCONFIG_STATUS_DISABLED = 0,
    UBX_AUTOCONFIG_STATUS_RUNNING,
    UBX_AUTOCONFIG_STATUS_DONE,
    UBX_AUTOCONFIG_STATUS_ERROR
} ubx_autoconfig_status_t;

#define UBX_
typedef struct {
//    bool    autoconfigEnabled;
//    bool    storeSettings;
    GPSSettingsUbxAutoConfigOptions UbxAutoConfig;
    bool    SBASRanging;
    bool    SBASCorrection;
    bool    SBASIntegrity;
    ubx_config_sats_t SBASSats;
    uint8_t SBASChannelsUsed;

    int8_t  navRate;
    ubx_config_dynamicmodel_t dynamicModel;

    bool    enableGPS;
    bool    enableGLONASS;
    bool    enableBeiDou;
//    bool    configStoreAndDisable;
} ubx_autoconfig_settings_t;

// Sent messages for configuration support
typedef struct UBX_CFG_CFG ubx_cfg_cfg_t;
typedef struct UBX_CFG_NAV5 ubx_cfg_nav5_t;
typedef struct UBX_CFG_RATE ubx_cfg_rate_t;
typedef struct UBX_CFG_MSG ubx_cfg_msg_t;
typedef struct UBX_CFG_PRT ubx_cfg_prt_t;
typedef struct UBX_CFG_SBAS ubx_cfg_sbas_t;
typedef struct UBX_CFG_GNSS_CFGBLOCK ubx_cfg_gnss_cfgblock_t;
typedef struct UBX_CFG_GNSS ubx_cfg_gnss_t;
typedef struct UBXSENTHEADER UBXSentHeader_t;
typedef union  UBXSENTPACKET UBXSentPacket_t;

void gps_ubx_autoconfig_run(char * *buffer, uint16_t *bytes_to_send);
void gps_ubx_autoconfig_set(ubx_autoconfig_settings_t *config);
void gps_ubx_reset_sensor_type();

int32_t ubx_autoconfig_get_status();
#endif /* UBX_AUTOCONFIG_H_ */
