#ifndef UZENET_H
#define UZENET_H
#include <avr/pgmspace.h>
#ifndef SPIRAM_H
	#include "spiram.h"
#endif

extern u8 UartUnreadCount();
extern s16 UartReadChar();
extern s8 UartSendChar(u8 data);		
extern bool IsUartTxBufferEmpty();
extern bool IsUartTxBufferFull();
extern void InitUartTxBuffer();
extern void InitUartRxBuffer();
extern unsigned char ReadEeprom(unsigned int addr);

extern void Uzenet_SyncLFSR();//all games using an LFSR must define this
//extern const char UN_ROM_IDENTIFIER[];

//optional user supplied custom implementations
#ifdef UN_CUSTOM_RSVP_HANDLER
extern void Uzenet_CustomRsvpHandler();
#endif
#ifdef UN_CUSTOM_MATCH_HANDLER
extern void Uzenet_CustomMatchHandler();
#endif
#ifdef UN_FONT_SPECIFICATION
extern const char UN_FONT_SPECIFICATION_TABLE[];
#endif

//#define UN_HARDCODED_SERVER	1
#ifdef UN_HARDCODED_SERVER
const char UN_HARDCODED_SERVER_NAME[] PROGMEM = "10.0.99.158";
#endif

const char UN_ROM_IDENTIFIER[] PROGMEM = "CONCT400";//must be 8 characters, the first 6 are registers to the original game implementor(or overriden by admins), the last 2 are for versioning
#define UN_FONT_SPECIFICATION	1
const char UN_FONT_SPECIFICATION_TABLE[] PROGMEM = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]???ABCDEFGHIJKLMNOPQRSTUVWXYZ?????";

//(186=9600=16/frame),(124=14400=24/frame),(92=19200=32/frame),(61=28800=48/frame),(46=38400=64/frame),(30=57600=96/frame),(22=76800=128/frame),(15=115200=192/frame),
#define UN_BAUD_RATE	9600//the default state is to boot in 9600, but you can use a higher rate after the intial setup(higher speeds required larger UART_RX_BUFFER_SIZE values)
//http://wormfood.net/avrbaudcalc.php?bitrate=&clock=28.63636&databits=8
//https://lucidar.me/en/serialib/most-used-baud-rates-table/
//the below divisors all assume double speed mode bit is set
#if (UN_BAUD_RATE == 600)//1 byte/frame, 2982 divisor, 0,0% error, **16.67ms** serialization delay
	#define UN_RX_MIN_BUF	1
	#define UN_UBRR0H	1
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 1200)//2 bytes/frame, 1490 divisor, 0.0% error, **8.33ms** serialization delay 
	#define UN_RX_MIN_BUF	2
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 2400)//4 bytes/frame, 1490 0.0% error, **4.17ms** serialization delay
	#define UN_RX_MIN_BUF	4
	#define UN_UBRR0H	1
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 4800)//8 bytes/frame, 745 0.0% error, **2.08ms** serialization delay
	#define UN_RX_MIN_BUF	8
	#define UN_UBRR0H	1
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 9600)//16 bytes/frame, 185 0.0% error, 1.04ms serialization delay
	#define UN_RX_MIN_BUF	16

#elif (UN_BAUD_RATE == 14400)//24 bytes/frame, 248 0.2% error,
	#define UN_RX_MIN_BUF	24+8//UART power of 2 implementation requires 32
	#define UN_UBRR0H 	1
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 19200)//32 bytes/frame, 185 0.2% error, 0.52ms serialization delay
	#define UN_RX_MIN_BUF	32
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 28800)//48 bytes/frame, 123 0.2% error, 0.35ms serialization delay
	#define UN_RX_MIN_BUF	48+16//UART power of 2 implementation requires 64
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 38400)//64 bytes/frame, 92 0.2% error, 0.26ms serialization delay
	#define UN_RX_MIN_BUF	64
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 57600)//96 bytes/frame, 61 0.2% error, 0.17ms serialization delay
	#define UN_RX_MIN_BUF	96+32//UART power of 2 implementation requires 128
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 76800)//128 bytes/frame, 46 0.8% error, 0.13ms serialization delay
	#define UN_RX_MIN_BUF	128
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 115200)//192 bytes/frame, 30 0.2% error, 0.09ms serialization delay
	#define UN_RX_MIN_BUF	192+64//UART power of 2 implementation requires 256
	#define UN_UBRR0H	0
	#define UN_UBRR0L	15
#elif (UN_BAUD_RATE == 230400)//**384 bytes/frame, 7 **2.9%** error, 0.04ms serialization delay
	#define UN_RX_MIN_BUF	328-128//**limited by UART implementation to 256 ***WARNING, possible to overflow mid-frame(use with consideration)
	#define UN_NO_DOUBLE	1//less error issues?
	#define UN_UBRR0H	0
	#define UN_UBRR0L	7
#elif (UN_BAUD_RATE == 250000)//**417/+1 bytes/frame, 6 **2.3%** error, <0.04ms serialization delay
	#define UN_RX_MIN_BUF	417-161//**limited by UART implementation to 256 ***WARNING, possible to overflow mid-frame(use with consideration)
	#define UN_NO_DOUBLE	1//less error issues?
	#define UN_UBRR0H	0
	#define UN_UBRR0L	6
#endif

#define UN_MATCH_IMMEDIATE_READY 1

const char UN_STR_PING[] PROGMEM = "PING";
const char UN_STR_PONG[] PROGMEM = "PONG";
const char UN_STR_ERROR[] PROGMEM = "ERROR";


#define ESP_RESET PD3
#define ESP_ENABLE PA6

#define UN_ESP8266_DETECTED	1//recieved at least 1 byte(could have been garbage/bad baud)
#define UN_ESP8266_VERIFIED	2//received something intelligble, we know the ESP8266 can talk to us
#define UN_SPI_RAM_DETECTED	4
#define UN_PASS_DETECTED	8//something is in EEPROM at least, not verified to be a real password
#define UN_HOST_DETECTED	16//something is in EEPROM at least, not verified to be a real password
#define UN_IP_DETECTED		32
#ifdef UN_SKIP_EEPROM_CHECK
	#define UN_PREREQUISITES	(UN_ESP8266_DETECTED|UN_ESP8266_VERIFIED|UN_SPI_RAM_DETECTED|UN_HOST_DETECTED|UN_PASS_DETECTED|UN_IP_DETECTED)	
#else
	#define UN_PREREQUISITES	(UN_ESP8266_DETECTED|UN_ESP8266_VERIFIED|UN_SPI_RAM_DETECTED|UN_IP_DETECTED)
#endif
#define UN_SERVER_DETECTED	64
#define UN_FIND_OPPONENT	128
#define UN_DETECTED_RSVP	256
#define UN_STATE_GOT_NAMES	512

#define UN_STATE_DO_SYNC		1024
#define UN_STATE_PLAYING	2048
#define UN_STATE_QUIT_GAME	4096
#define UN_STATE_TRANSPARENT	8192
#define UN_STATE_TX_BREAK	16384
#define UN_STATE_	32768


#define UN_ERROR_TRANSIENT		1//a(possibly) temporary error, which might have been caused by temporary network conditions
#define UN_ERROR_CONNECT_FAILED	2//possibly a transient error
#define UN_ERROR_SERVER_TIMEOUT	4//possibly a transient error
#define UN_ERROR_GAME_FAILED		64
#define UN_ERROR_CRITICAL		128//an error that wont be fixed by resetting and trying again

#define UN_MATCH_RSVP_DETECTED 2//a pre-arranged match was detected, bypass the requirement for the user to manuall select UZENET option
#define UN_MATCH_HAVE_NAMES	4//we have all the names of the players in the room?
#define UN_MATCH_WAS_KICKED	8//we got kicked from the room somehow?


#define UN_PASS_EEPROM_OFF	23//8 characters long
#define UN_HOST_EEPROM_OFF	11//up to 8 characters long, used as a suffix to "uze", so a value of "net.us" implies the hostname "uzenet.us"(was the old/unused MAC attribute).
#define UN_SPIR_BASE		512
#define UN_SPIR_BASE_TX	UN_SPIR_BASE+0
#define UN_SPIR_BASE_RX	UN_SPIR_BASE_TX+1024
#define UN_PASS_BASE		UN_SPIR_BASE_RX+1024
#define UN_SPIR_HOST_BASE	UN_PASS_BASE+16
#define UN_PLAYER_INFO_BASE	UN_SPIR_HOST_BASE+64
#define UN_PAD_HISTORY_BASE	UN_PLAYER_INFO_BASE+(13*4)//512 per player(256 pad states each)


#define UN_STEP_RESET		0
#define UN_STEP_START		1
#define UN_STEP_HOST_DETECTED	2
#define UN_STEP_CHECK_BOOT	3
#define UN_STEP_SET_BAUD	4
#define UN_STEP_SET_MUX	5
#define UN_STEP_WAIT_IP	6
#define UN_STEP_CONNECT	7
#define UN_STEP_CHECK_CONNECT	8
#define UN_STEP_SET_SEND	9
#define UN_STEP_SEND_LOGIN	10
#define UN_STEP_PRECONNECT	11
#define UN_STEP_GAME_SPECIFY	12
#define UN_STEP_REQ_RSVP	13
#define UN_STEP_CHECK_RSVP	14
#define UN_STEP_CHECK_JOIN	15
#define UN_STEP_NO_MATCH	16
#define UN_STEP_IN_MATCH	17
#define UN_STEP_CHECK_READY	18
#define UN_STEP_PLAYER_INFO	19
#define UN_STEP_QUIT_GAME	40
//custom override states, to allow user implementations of RSVP and matchmaking handling
#define UN_STEP_CUSTOM_RSVP	101
#define UN_STEP_CUSTOM_MATCH	151


#define UN_STEP_PLAYING	240

//COMMAND:				ASCII//HEX:(DESCRIPTION):
#define UN_CMD_NULL			0//0x00:(NULL, as a command this will cause the server to disconnect assumed as error)
#define UN_CMD_HEADER_START		1//0x01:(start of header)
#define UN_CMD_TEXT_START		2//0x02:(start of text)
#define UN_CMD_TEXT_END		3//0x03:(end of text)
#define UN_CMD_TX_END			4//0x04:(end of transmission)disconnects from the server
#define UN_CMD_ENQUIRY			5//0x05:(enquiry)
#define UN_CMD_ACKNOWLEDGE		6//0x06:(acknowledge)
#define UN_CMD_BELL			7//0x07:(bell)
#define UN_CMD_BACKSPACE		8//0x08:(backspace)
#define UN_CMD_HORIZONTAL_TAB		9//0x09:(horizontal tab)
#define UN_CMD_LINE_FEED		10//0x0A:(line feed)
#define UN_CMD_VERTICAL_TAB		11//0x0B:(vertical tab)
#define UN_CMD_FORM_FEED		12//0x0C:(form feed)
#define UN_CMD_CARRIAGE_RETURN	13//0x0D:(carriage return)
#define UN_CMD_SHIFT_OUT		14//0x0E:(shift out)
#define UN_CMD_SHIFT_IN		15//0x0F:(shift in)
#define UN_CMD_DATA_LINK_ESCAPE	16//0x10:(data link escape)
#define UN_CMD_DEVICE_CONTROL1	17//0x11:(device control 1)
#define UN_CMD_DEVICE_CONTROL2	18//0x12:(device control 2)
#define UN_CMD_DEVICE_CONTROL3	19//0x13:(device control 3)
#define UN_CMD_DEVICE_CONTROL4	20//0x14:(device control 4)
#define UN_CMD_NEGATIVE_ACKNOWLEDGE	21//0x15:(negative acknowledge)
#define UN_CMD_SYNCHRONIZE		22//0x16:(synchronize)
#define UN_CMD_END_OF_TX_BLOCK	23//0x17:(end of transmission block)
#define UN_CMD_CANCEL			24//0x18:(cancel)
#define UN_CMD_END_OF_MEDIUM		25//0x19:(end of medium)
#define UN_CMD_SUBSTITUTE		26//0x1A:(substitute)
#define UN_CMD_ESCAPE			27//0x1B:(escape)
#define UN_CMD_FILE_SEPARATOR		28//0x1C:(file separator)
#define UN_CMD_GROUP_SEPARATOR	29//0x1D:(group separator)
#define UN_CMD_RECORD_SEPARATOR	30//0x1E:(record separator)
#define UN_CMD_UNIT_SEPARATOR		31//0x1F:(unit separator)

#define UN_CMD_ROM_IDENTIFY		32//' '
#define UN_FONT_SPECIFICATION_COMMON	33//'!'
#define UN_CMD_FONT_SPECIFY		34//'"'
#define UN_CMD_CHECK_RSVP		35//'#'
#define UN_CMD_JOIN_MATCH		36//'$'
#define UN_CMD_REQ_MATCH_SIMPLE	37//'%'
#define UN_CMD_CHECK_MATCH_READY	38
#define UN_CMD_SEND_MATCH_READY	39
#define UN_CMD_PLAYER_INFO_SIMPLE	40

#define UN_CMD_DO_SYNC			45

#define UN_CMD_BAD_SYNC		49
#define UN_CMD_QUIT_MATCH		50

#define UN_CMD_CHAT_BYTE		55

#define UN_CMD_PAD_DATA_SIMPLE	60
//TODO NEED TO SUPPORT NEW BAUD FORMAT IN EMULATOR BEFORE DOING THIS
//#define UN_CUSTOM_BAUD_DIVISOR	30
//const char UN_CUSTOM_BAUD_NUM[] PROGMEM = "57600";//the equivalent baud rate to the above divisor, in string form(for the AT command)


//#define UN_MATCH_MAKING_STYLE	1//override the default simple "join/host any game without password" behavior

#define UN_KBD_ROLE_GET_ASCII		0
#define UN_KBD_ROLE_EMULATE_PAD	1


#define UN_MAX_RUN_AHEAD	120
#define UN_MAX_SYNC_WAIT	180

#define MAX_UZENET_PLAYERS	2

// This system lets you use a small Tx buffer, which makes this RAM nearly free...
volatile u16 uzenet_state;
volatile u8 uzenet_sync;
volatile u8 uzenet_error;
volatile u8 uzenet_step,uzenet_wait,uzenet_local_tick;
volatile u8 uzenet_remote_player,uzenet_remote_tick,uzenet_remote_cmd,uzenet_remote_last_rx_tick;//[MAX_UZENET_PLAYERS];
volatile u8 uzenet_local_player;
volatile u16 uzenet_spir_rx_pos, uzenet_spir_tx_pos;
volatile u16 uzenet_spir_rx_len;
volatile u8 uzenet_spir_tx_len;

u8 uzenet_last_rx;
u8 uzenet_crc;//use during critical sections


#endif//UZENET_H
