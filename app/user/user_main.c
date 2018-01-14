/******************************************************************************
 * FileName: user_main.c
 * Test meSDK :)
 *******************************************************************************/

#include "user_config.h"
#include "bios.h"
#include "sdk/add_func.h"
#include "hw/esp8266.h"
#include "user_interface.h"
#include "sdk/rom2ram.h"
#include "esp_rawsend.h"
#if 0 // для расчета ипользования памяти meSDK = 0, для примера = 1
/******************************************************************************
 * FunctionName : wifi_handle_event_cb
 * Description  :
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt)
{
	int i;
	os_printf("WiFi event %x\n", evt->event);
	switch (evt->event) {
		case EVENT_SOFTAPMODE_PROBEREQRECVED:
			os_printf("Probe Request (MAC:" MACSTR ", RSSI:%d)\n",
					MAC2STR(evt->event_info.ap_probereqrecved.mac),
					evt->event_info.ap_probereqrecved.rssi);
			break;
		case EVENT_STAMODE_CONNECTED:
			os_printf("Connect to ssid %s, channel %d\n",
					evt->event_info.connected.ssid,
					evt->event_info.connected.channel);
			break;
		case EVENT_STAMODE_DISCONNECTED:
			os_printf("Disconnect from ssid %s, reason %d\n",
					evt->event_info.disconnected.ssid,
					evt->event_info.disconnected.reason);
			break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
			os_printf("New AuthMode: %d -> %d\n",
					evt->event_info.auth_change.old_mode,
					evt->event_info.auth_change.new_mode);
			break;
		case EVENT_STAMODE_GOT_IP:
			os_printf("Station ip:" IPSTR ", mask:" IPSTR ", gw:" IPSTR "\n",
					IP2STR(&evt->event_info.got_ip.ip),
					IP2STR(&evt->event_info.got_ip.mask),
					IP2STR(&evt->event_info.got_ip.gw));
			break;
		case EVENT_SOFTAPMODE_STACONNECTED:
			i = wifi_softap_get_station_num(); // Number count of stations which connected to ESP8266 soft-AP
			os_printf("Station[%u]: " MACSTR " join, AID = %d\n",
					i,
					MAC2STR(evt->event_info.sta_connected.mac),
					evt->event_info.sta_connected.aid);
			break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
			i = wifi_softap_get_station_num();
			os_printf("Station[%u]: " MACSTR " leave, AID = %d\n",
					i,
					MAC2STR(evt->event_info.sta_disconnected.mac),
					evt->event_info.sta_disconnected.aid);
			break;
		case EVENT_STAMODE_DHCP_TIMEOUT:
			os_printf("DHCP timeot\n");
			break;
/*		default:
			break; */
		}
}

/******************************************************************************
 * FunctionName : init_done_cb
 * Description  :
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void ICACHE_FLASH_ATTR init_done_cb(void)
{
    os_printf("\nSDK Init - Ok\nCurrent 'heap' size: %d bytes\n", system_get_free_heap_size());
	os_printf("Set CPU CLK: %u MHz\n", ets_get_cpu_frequency());
}

extern uint32 _lit4_start[]; // addr start BSS in IRAM
extern uint32 _lit4_end[]; // addr end BSS in IRAM

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void ICACHE_FLASH_ATTR user_init(void) {
	if(eraminfo.size > 0) os_printf("Found free IRAM: base: %p, size: %d bytes\n", eraminfo.base,  eraminfo.size);
	os_printf("System memory:\n");
    system_print_meminfo();
    os_printf("bssi  : 0x%x ~ 0x%x, len: %d\n", &_lit4_start, &_lit4_end, (uint32)(&_lit4_end) - (uint32)(&_lit4_start));
    os_printf("free  : 0x%x ~ 0x%x, len: %d\n", (uint32)(&_lit4_end), (uint32)(eraminfo.base) + eraminfo.size, (uint32)(eraminfo.base) + eraminfo.size - (uint32)(&_lit4_end));
    os_printf("Start 'heap' size: %d bytes\n", system_get_free_heap_size());
	os_printf("Set CPU CLK: %u MHz\n", ets_get_cpu_frequency());
	system_deep_sleep_set_option(0);
	wifi_set_event_handler_cb(wifi_handle_event_cb);
	system_init_done_cb(init_done_cb);
}
#else



ETSTimer rawsend_timer DATA_IRAM_ATTR; 


int counter, count2, count3;


uint8_t mypacket[30+256] = {  //256 = max size of additional payload
	0x08, //Frame type, 0x80 = beacon, Tried data, but seems to have been filtered on RX side by other ESP
	0x00, 0x00, 0x00, 
	0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,
	0x00, 0x00,  //Sequence number, cleared by espressif
	0x82, 0x66,	 //"Mysterious OLPC stuff"
	0x82, 0x66, 0x00, 0x00, //????
	
};

void prom(uint8 *buf, uint16 len)
{
	count3++;

}

void idle(void * v) 
{
}


void sent_freedom_cb(uint8 status)
{
/*
	int b = 0;
	for( b = 0; b < MAX_BUFFERS; b++ )
	{
		if( bufferinuse[b] == 0 ) break;
	}
	if( b == MAX_BUFFERS ) return;
*/
	count2++;
}


void  __attribute__ ((noinline)) rx_func( struct RxPacket * pkt, void ** v )
{
	counter++;
#if 1
	int len = pkt->rx_ctl.legacy_length;
	os_printf("Recv callback #%d: %d bytes\n", counter++, len);
	os_printf("Channel: %d PHY: %d\n", pkt->rx_ctl.channel, wifi_get_phy_mode());
	int i;
	for( i = 0 ; i < len; i++ )
	{
		os_printf( "%02x (%c)", pkt->data[i], (pkt->data[i]>31)?pkt->data[i]:' ' );
	}
#endif
}

void myTimer( )
{
	//Do something fancy like send a raw packet.
	//char packet[100];
#if 0
	packet[0] = '\xde';
	packet[1] = '\xad';
	packet[2] = '\xbe';	/* This will become \x00 */
	packet[3] = '\xef';	/* This too. */
	ets_sprintf(packet + 4, "%s%s%s%s%s", "HELLO WORLD!", "HELLO WORLD!", "HELLO WORLD!", "HELLO WORLD!", "HaLLO w0RLD!");
#endif

		//printf( "%d\n", debugccount );
		//uart0_sendStr("k");
	int txpakid = 0;
		ets_strcpy( mypacket+30, "ESPEED" );
		txpakid++;
		mypacket[36] = txpakid>>24;
		mypacket[37] = txpakid>>16;
		mypacket[38] = txpakid>>8;
		mypacket[39] = txpakid>>0;
		mypacket[40] = 0;
		mypacket[41] = 0;
		mypacket[42] = 0;
		mypacket[43] = 0;


	wifi_send_pkt_freedom( mypacket,  30 + 16, true) ; 
	//wifi_send_raw_packet(packet, sizeof packet);
	os_printf( "%d/%d/%d/%d\n", counter, count2, count3, wifi_get_opmode() );

}

static os_timer_t some_timer;

void ICACHE_FLASH_ATTR init_done_cb(void)
{

	wifi_set_raw_recv_cb( rx_func );

	wifi_register_send_pkt_freedom_cb( sent_freedom_cb );
//	wifi_promiscuous_enable(1);
//	wifi_set_promiscuous_rx_cb(prom
	wifi_set_channel(6);

	os_timer_disarm(&some_timer);
	os_timer_setfn(&some_timer, (os_timer_func_t *)myTimer, NULL);
	os_timer_arm(&some_timer, 100, 1); //The underlying API expects it's slow ticks to average out to 50ms.

	os_printf( "Init done\n" );
}

void ICACHE_FLASH_ATTR user_init(void) {

	ets_idle_cb = idle;
	system_deep_sleep_set_option(0);

	wifi_set_opmode(2);
	wifi_set_channel(6);

	//wifi_set_event_handler_cb(wifi_handle_event_cb);
	system_init_done_cb(init_done_cb);
}
#endif


