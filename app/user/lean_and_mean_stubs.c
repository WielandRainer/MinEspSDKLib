#ifdef LEAN_AND_MEAN

#include <c_types.h>

struct netif
{
	int i;
};
const uint8_t ethbroadcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};

struct pbuf
{
	int i;
};

struct netif netif_default[4];

void sys_check_timeouts() { }
void lwip_init(void) { }
void netif_set_default(struct netif *netif) { }
void *pbuf_alloc(int l, uint16_t length, uint8_t type) { return &ethbroadcast; }
void pbuf_free( void *v  ) { }
void * eagle_lwip_if_alloc(void *conn, uint8 *macaddr, void *ipinfo) { return &ethbroadcast; } 
void netif_set_up(struct netif *netif){ }
void netif_set_down(struct netif *netif){ }
void eagle_lwip_if_free( void * v ) { }
void pbuf_ref(struct pbuf *p) { }
struct netif *eagle_lwip_getif(int n) { return &netif_default[n]; } 

void dhcps_start(void * v) { }
void dhcps_stop() { }
void dhcp_release(struct netif *netif) { }
void dhcp_stop() { }
void dhcp_start( void * v ) { }
void dhcp_cleanup(struct netif *netif) { }

char hostname[64];
char default_hostname[64];

void netif_set_addr(struct netif *netif, void *ipaddr, void *netmask, void *gw) { }


#endif

