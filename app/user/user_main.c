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
	0x00,0xaa,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,
	0x00, 0x00,  //Sequence number, cleared by espressif
	0x82, 0x66,	 //"Mysterious OLPC stuff"
	0x82, 0x66, 0x00, 0x00, //????
	
};

void prom(uint8 *buf, uint16 len)
{
	count3++;

}


extern uint8 system_option[250];
//#define deep_sleep_option system_option[241]
bool ICACHE_FLASH_ATTR deep_sleep_set_option(uint8 option);
void user_uart_wait_tx_fifo_empty(uint32 n, uint32 x);
int pm_rtc_clock_cali_proc(void);
void pm_set_sleep_time(int timer_us_arg);
void system_deep_sleep_local_2(void);

void ninjasleep(int timer_arg) // В ранних SDK _sys_deep_sleep_timer()
{
	deep_sleep_set_option(deep_sleep_option);
	*((uint16_t*)0x3FF20DE0) = 0x3333;
	ets_delay_us(20);
	user_uart_wait_tx_fifo_empty(0, 500000);
	user_uart_wait_tx_fifo_empty(1, 500000);
	IO_RTC_0 = 0;	// 0x60000700 = 0
	IO_RTC_0 &= ~BIT14;
	IO_RTC_0 |= 0x30;
	RTC_BASE[17] = 4; //0x60000744 = 4
	IO_RTC_3 = 0x10010; // 	HWREG(PERIPHS_RTC_BASEADDR, 0x0C) = 0x10010;
	RTC_BASE[18] = (RTC_BASE[18] &  0xFFFF01FF) | 0xFC00; // HWREG(PERIPHS_RTC_BASEADDR, 0x48) = (HWREG(PERIPHS_RTC_BASEADDR,0x48) & 0xFFFF01FF) | 0xFC00;
	RTC_BASE[18] = (RTC_BASE[18] &  0xE00) | 0x80; // HWREG(PERIPHS_RTC_BASEADDR, 0x48) = (HWREG(PERIPHS_RTC_BASEADDR, 0x48) & 0xE00) | 0x80;
	IO_RTC_4 = 0; //0x60000710 = 0
	IO_RTC_4 = 0; //0x6000071C = 0
	IO_RTC_SLP_VAL = IO_RTC_SLP_CNT_VAL + 136 + 256; //	HWREG(PERIPHS_RTC_BASEADDR, 0x04) = HWREG(PERIPHS_RTC_BASEADDR, 0x1C) + 0x88;
	IO_RTC_6 = 8; // HWREG(PERIPHS_RTC_BASEADDR, 0x18) = 8;
	IO_RTC_2 = 0x100000; // HWREG(PERIPHS_RTC_BASEADDR, 0x08) = 0x100000;
	ets_delay_us(200);
	RTC_GPI2_CFG = 0x11; //	HWREG(PERIPHS_RTC_BASEADDR, 0x9C) = 0x11;
	IO_PAD_XPD_DCDC_CONF = 0x03; // HWREG(PERIPHS_RTC_BASEADDR, 0xA0) = 0x03;
	IO_RTC_3 = 0x640C8; // HWREG(PERIPHS_RTC_BASEADDR, 0x0C) = 0x640C8;
	IO_RTC_0 &= 0xFCF; // HWREG(PERIPHS_RTC_BASEADDR, 0x00) &= 0xFCF;
	RTC_GPI2_CFG = 0x11; // HWREG(PERIPHS_RTC_BASEADDR, 0x9C) = 0x11;
	IO_PAD_XPD_DCDC_CONF = 0x03; // HWREG(PERIPHS_RTC_BASEADDR, 0xA0) = 0x03;

	INTC_EDGE_EN &= 0x7E; // HWREG(PERIPHS_DPORT_BASEADDR, 4) &= 0x7E; // WDT int off
	ets_isr_mask(1<<8); // Disable WDT isr

	DPORT_BASE[0] = (DPORT_BASE[0]&0x60)|0x0e; // nmi int
	while(DPORT_BASE[0]&1);

	RTC_BASE[16] = 0xFFF; // HWREG(PERIPHS_RTC_BASEADDR, 0x40) = 0xFFF;
	RTC_BASE[17] = 0x20; // HWREG(PERIPHS_RTC_BASEADDR, 0x44) = 0x20;

	uint32 clpr = pm_rtc_clock_cali_proc();
	pm_set_sleep_time(timer_arg);

//	IO_RTC_4 = 0; // HWREG(PERIPHS_RTC_BASEADDR, 0x10) = 0x00;

	if(clpr == 0) {
		IO_RTC_6 = 0; //	HWREG(PERIPHS_RTC_BASEADDR, 0x18) = 0x00;
	}
	else {
		IO_RTC_6 = 8;	//	HWREG(PERIPHS_RTC_BASEADDR, 0x18) = 0x08;
	}
	system_deep_sleep_local_2();
}


void system_deep_sleep_instant(void *timer_arg); // В ранних SDK _sys_deep_sleep_timer()

int hit_timer = 0;
void idle(void * v) 
{
	if( hit_timer )	ninjasleep(1000000);
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
#if 0
	os_printf("Channel: %d PHY: %d\n", pkt->rx_ctl.channel, wifi_get_phy_mode());
	int i;
	for( i = 0 ; i < len; i++ )
	{
		os_printf( "%02x (%c)", pkt->data[i], (pkt->data[i]>31)?pkt->data[i]:' ' );
	}
#endif
#endif
}

void myTimer( )
{
	//wifi_send_raw_packet(packet, sizeof packet);
	os_printf( "%d/%d/%d/%d\n", counter, count2, count3, wifi_get_opmode() );
	hit_timer = 1;
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
}

void ICACHE_FLASH_ATTR user_init(void) {

	ets_idle_cb = idle;
//	system_deep_sleep_set_option(2);
	system_deep_sleep_set_option(0);

	wifi_set_opmode(2);
	wifi_set_channel(6);

	//wifi_set_event_handler_cb(wifi_handle_event_cb);
	system_init_done_cb(init_done_cb);
}
#endif


