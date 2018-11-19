#ifndef __DEFINES__
#define __DEFINES__


// The purpose of this file is to defines the macros
// Used in all system's parts


// ----------------- General macros -----------------
#define __ERROR__   -1
#define __OK__      0
#define DEFAULT_TTL 60
#define LOOPBACK_IP "127.0.0.1"
#define XARPD_PORT  5050
#define LISTEN_ENQ 5 // listen queue size
// -------------- End general macros -----------------


// ----------- Types of arp table entries ------------
#define DYNAMIC_ENTRY 0
#define STATIC_ENTRY  1
// --------- End types of arp table entries ----------


// ------------------ ARP protocol -------------------
#define ARP_ETHERTYPE     0x0806

#define ARP_HW_TYPE       0x0001
#define ARP_PROTOTYPE     0x0800 // IP
#define HW_ADDR_LEN       0x06 // 6 bytes
#define PROTOCOL_ADDR_LEN 0x04 // 4 bytes
#define ARP_REQUEST       0x01
#define ARP_RESPONSE      0x02
// ---------------- End ARP protocol ------------------

// ---------- Inter program communication ------------

// The protocol looks like that. The protocol has a
// fixed size of 13bytes this is used for most requests
// OPCODE IPADDRESS ETHERADDRESS TTL
// 1 byte   4 bytes    6 bytes   2 bytes

// There is a other possible header used when xifconfig
// changes the parameters of a interface and needs to notify the
// xarpd
// OPCODE IFACE_NAME
// 1 byte n bytes

// OPCODES
#define SHOW_TABLE    0
#define RES_IP        1
#define ADD_LINE      2
#define DEL_LINE      3
#define SET_TTL       4
#define LIST_IFCES    5
#define CONFIG_IFACE  6
#define SET_IFACE_MTU 7

// -------- End inter program communication ----------

#endif
