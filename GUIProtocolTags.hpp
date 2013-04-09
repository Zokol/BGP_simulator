
/*!
 * TCP port of the GUI socket
 */
#define PORT 50000

///************************ Configureation parsing *******************************

/*!
 * Start tag of the configuration string
 */
#define START_TAG "<SIM_CONFIG>"

/*!
 * Length of the start tag
 */
#define START_TAG_LENGTH 12

/*!
 * End tag of the configuration string
 */
#define END_TAG "</SIM_CONFIG>"

/*!
 * Length of the end tag
 */
#define END_TAG_LENGTH START_TAG_LENGTH+1

/*!
 * Router field separator
 */
#define ROUTER_SEPARATOR ";"

/*!
 * Connection field separator
 */
#define CONNECTION_SEPARATOR "_"

/*!
 * Field separator
 */
#define FIELD_SEPARATOR ","

///************************ Simulation commands *******************************


/*!
 * Positive acknowledgement
 */
#define ACK "<akc />"

/*!
 * Negative acknowledgement
 */
#define NACK "<ack />"

#define RESET_ROUTER "RESET_ROUTER"//RESET_ROUTER_N
#define KILL_ROUTER "KILL_ROUTER" //KILL_ROUTER_N
#define REVIVE_ROUTER "REVIVE_ROUTER" //REVIVE_ROUTER_N
#define READ_ROUTER "READ_ROUTER" //READ_ROUTER_LIST
#define READ_ROUTER "READ_ROUTER" //READ_ROUTER_N
#define READ_PACKET "READ_PACKET" //READ_PACKET_N
#define SEND_PACKET "SEND_PACKET" //SEND_PACKET PORT_N [HEADER] [PAYLOAD]
#define CONNECT "CONNECT" //CONNECT PORT_N PORT_N
#define DISCONNECT "DISCONNECT" //DISCONNECT PORT_N
#define STOP "STOP"
#define READ_TABLE "READ_TABLE" //READ_TABLE_ID


///************************ Program modes *******************************


//#define _GUI

#define _GUI_TEST
