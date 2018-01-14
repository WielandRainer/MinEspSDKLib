#ifdef LEAN_AND_MEAN

#include <c_types.h>
#include <osapi.h>
#include <bios/mem.h>
#include <sdk/libmain.h>
struct netif
{
	int i;
};
uint8_t ethbroadcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t test[] = {0xaa,0xff,0xff,0xff,0xff,0xff};

struct pbuf
{
	int i;
};

struct netif netif_default[4];

void ICACHE_FLASH_ATTR sys_check_timeouts() { }
void ICACHE_FLASH_ATTR lwip_init(void) { }
void ICACHE_FLASH_ATTR netif_set_default(struct netif *netif) { }
void ICACHE_FLASH_ATTR *pbuf_alloc(int l, uint16_t length, uint8_t type) { return &test; }
void ICACHE_FLASH_ATTR pbuf_free( void *v  ) { }
void ICACHE_FLASH_ATTR * eagle_lwip_if_alloc(void *conn, uint8 *macaddr, void *ipinfo) { return &test; } 
void ICACHE_FLASH_ATTR netif_set_up(struct netif *netif){ }
void ICACHE_FLASH_ATTR netif_set_down(struct netif *netif){ }
void ICACHE_FLASH_ATTR eagle_lwip_if_free( void * v ) { }
void ICACHE_FLASH_ATTR pbuf_ref(struct pbuf *p) { }
struct netif * ICACHE_FLASH_ATTR eagle_lwip_getif(int n) { return &netif_default[n]; } 

void ICACHE_FLASH_ATTR ICACHE_FLASH_ATTR dhcps_start(void * v) { }
void ICACHE_FLASH_ATTR dhcps_stop() { }
void ICACHE_FLASH_ATTR dhcp_release(struct netif *netif) { }
void ICACHE_FLASH_ATTR dhcp_stop() { }
void ICACHE_FLASH_ATTR dhcp_start( void * v ) { }
void ICACHE_FLASH_ATTR dhcp_cleanup(struct netif *netif) { }

char hostname[64];
char default_hostname[64];

void ICACHE_FLASH_ATTR netif_set_addr(struct netif *netif, void *ipaddr, void *netmask, void *gw) { }


//Not really stubs but deferring to the mask rom...
void ICACHE_FLASH_ATTR * pvPortMalloc( int x ) { return mem_malloc(x); }
void ICACHE_FLASH_ATTR * pvPortZalloc( int x ) { return mem_zalloc(x); }
void ICACHE_FLASH_ATTR vPortFree( void * x ) { mem_free(x); }
void ICACHE_FLASH_ATTR prvHeapInit() {   mem_init(&_bss_end);  } //Already called.

int ICACHE_FLASH_ATTR xPortGetFreeHeapSize() { return 10000; } //bogus; pvvx mentions  *((uint32*)0x3FFE9E50);  *((uint32*)0x3FFE9CB0);... should look into it?
void ICACHE_FLASH_ATTR xPortWantedSizeAlign( int x ) {  } //Nope!


//Need this cause we aren't including lib80211
void ICACHE_FLASH_ATTR wpa_auth_sta_deinit() {}
void ICACHE_FLASH_ATTR wpa_config_bss() {}
void ICACHE_FLASH_ATTR wpa_init() {}
void ICACHE_FLASH_ATTR wpa_receive() {}
void ICACHE_FLASH_ATTR wpa_config_parse_string() {}
void ICACHE_FLASH_ATTR wpa_parse_wpa_ie() {}
void ICACHE_FLASH_ATTR wpa_sm_rx_eapol() {}
void ICACHE_FLASH_ATTR wpa_attach() {}

void ICACHE_FLASH_ATTR hostapd_setup_wpa_psk() {}
void ICACHE_FLASH_ATTR eagle_auth_done() {}
void ICACHE_FLASH_ATTR wpa_config_profile() {}

void ICACHE_FLASH_ATTR hexstr2bin() { }
void ICACHE_FLASH_ATTR ppInstallKey() { }
void ICACHE_FLASH_ATTR wpa_auth_sta_init() { }
void ICACHE_FLASH_ATTR wpa_validate_wpa_ie() { }
void ICACHE_FLASH_ATTR wpa_auth_sta_associated() { }


#endif

