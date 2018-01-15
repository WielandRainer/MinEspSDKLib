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
#include "sdk/libmain.h"
#include "sdk/fatal_errs.h"


ETSTimer rawsend_timer DATA_IRAM_ATTR; 
extern volatile uint32 rtc_ram_[64];	

int counter, count2, count3;


uint8_t mypacket[30+256] = {  //256 = max size of additional payload
	0x88, //Frame type, 0x80 = beacon, Tried data (0x08), but seems to have been filtered on RX side by other ESP;; 0x88 is unused.
	0x00, 0x00, 0x00, 
	0xff,0xff,0xff,0xff,0xff,0xff,
	0x00,0xaa,0xff,0xff,0xff,0xff, //SRC Mac, fill in.
	0xff,0xff,0xff,0xff,0xff,0xff,
	0x00, 0x00,  //Sequence number, cleared by espressif
	0x82, 0x66,	 //"Mysterious OLPC stuff"
	0x82, 0x66, 0x00, 0x00, //????
	
};

int v33;
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
	if( hit_timer )
	{
		ninjasleep(10000000);
	}
}

void wdev_go_sniffer();
void wDevEnableRx();
void wDevDisableRx(); 



void prom_cb( uint8_t * buf, uint16_t len )
{
	counter++;
	os_printf( "PROMCB callback #%d: %d bytes\n", counter, len );
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
//	wdev_go_sniffer(); //Sniff only.  Do not allow responses to packets.

/*
				wDevDisableRx();
				volatile uint32 * preg = (volatile uint32 *)0x3FF20000;
				preg[27] &= 0xFFE;
				preg[27] &= 0xFFD;
				preg[27] &= 0x7B;
				g_ic.c[446]= 1;
				wdev_go_sniffer();
				wDevEnableRx();
*/

	count2++;
	//wifi_set_opmode_current(0);
	wifi_set_promiscuous_rx_cb(prom_cb);
	wifi_promiscuous_enable(1);
}




int  __attribute__ ((noinline)) rx_func( struct RxPacket * pkt, void ** v )
{
	counter++;
#if 1
	int len = pkt->rx_ctl.legacy_length;
	os_printf("rx:%d\n", len);
#if 0
	os_printf("Channel: %d PHY: %d\n", pkt->rx_ctl.channel, wifi_get_phy_mode());
	int i;
	for( i = 0 ; i < len; i++ )
	{
		os_printf( "%02x (%c)", pkt->data[i], (pkt->data[i]>31)?pkt->data[i]:' ' );
	}
#endif
#endif
	return 1; //don't pass packet on.
}

void myTimer( )
{
	//wifi_send_raw_packet(packet, sizeof packet);
	//os_printf( "%d/%d/%d/%d\n", counter, count2, count3, wifi_get_opmode() );
	hit_timer = 1;
}

static os_timer_t some_timer;
uint32_t runid = 0;
void wDev_SetMacAddress( int i, void * v );

void init_done_cb(void)
{
	wifi_set_raw_recv_cb( rx_func );

	wifi_register_send_pkt_freedom_cb( sent_freedom_cb );
//	wifi_promiscuous_enable(1);
//	wifi_set_promiscuous_rx_cb(prom

	os_timer_disarm(&some_timer);
	os_timer_setfn(&some_timer, (os_timer_func_t *)myTimer, NULL);
	os_timer_arm(&some_timer, 16, 1); // wait 20ms before shutting back down.

	read_macaddr_from_otp( &mypacket[10] );
	mypacket[10] |= 2; //Locally administered.


	v33 = system_get_vdd33();
	ets_strcpy( mypacket+30, "VOLTTE" );
	mypacket[36] = runid>>24;
	mypacket[37] = runid>>16;
	mypacket[38] = runid>>8;
	mypacket[39] = runid>>0;
	mypacket[40] = v33>>8;
	mypacket[41] = v33;
	mypacket[42] = 0;
	mypacket[43] = 0;
	rtc_ram_[56] = runid+1;
	{
		wifi_set_user_fixed_rate( 3, PHY_RATE_6 );  //Use enum FIXED_RATE
		wifi_send_pkt_freedom( mypacket,  30 + 16, true) ; 
	}
}

void ICACHE_FLASH_ATTR user_init(void) {

	ets_idle_cb = idle;

	struct softap_config sac;
	wifi_softap_get_config(&sac);
	sac.channel=7;
	sac.ssid_len = 0;
	sac.beacon_interval = 60000;
	sac.authmode = 0;
	sac.ssid_hidden = 1;
	sac.max_connection = 0;
	wifi_softap_set_config_current(&sac);

	wifi_set_user_fixed_rate( 3, PHY_RATE_6 );  //Use enum FIXED_RATE
	wifi_set_opmode_current(2);
	//wifi_set_event_handler_cb(wifi_handle_event_cb);
	system_init_done_cb(init_done_cb);
}



void user_rf_pre_init(uint8_t * init_data)
{
	struct rst_info * rst_inf = (struct rst_info *)&RTC_MEM(0);

	if( rst_inf->reason != RST_EVENT_DEEP_SLEEP )
	{
		runid = rtc_ram_[56] = 0;
	}
	else
	{
		runid = rtc_ram_[56];
	}
	init_data[107] = 0xff;

	if( (runid & 0x3f) == 0 ) //Every 64 times, do a full RF recalibration.
	{
		//0: RF initialization when powerup depends on byte 114 of 
		//		esp_init_data_default.bin (0 ~ 127 bytes). For more details please see 
		//		ESP8266 SDK Getting Started Guide
		//1: RF initialization only calibrate VDD33 and Tx power which will take about 
		//		18 ms; this reduces the current consumption. 
		//2: RF initialization only calibrate VDD33 which will take about 2 ms; this has 
		//		the least current consumption. 
		//3: RF initialization will do the whole RF calibration which will take about 200 
		//		ms; this increases the current consumption
		system_phy_set_powerup_option(3); 

		system_phy_set_rfoption( 1 ); //1 forces cal; 2 disables cal.
	}
	else
	{
		system_phy_set_powerup_option(0); //fastest possible.
		system_phy_set_rfoption( 2 ); //1 forces cal; 2 disables cal.
	}
}
