
/*!
 * TCP port of the GUI socket
 */
#define PORT 50000

#define AS_INTERFACE 3

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
#define ACK "ACK"

/*!
 * Negative acknowledgement
 */
#define NACK "NACK"


/*!
 * Empties the routing tables
 */
#define RESET_ROUTER "RESET_ROUTER"//RESET_ROUTER [ROUTER_ID] 

/*!
 * Diconnects all interfaces, which are up and empties the routing tables
 */
#define KILL_ROUTER "KILL_ROUTER" //KILL_ROUTER [ROUTER_ID] 

/*!
 * Recovers from the KILL_ROUTER. Sets the interfaces up.
 */
#define REVIVE_ROUTER "REVIVE_ROUTER" //REVIVE_ROUTER [ROUTER_ID]

/*!
 *Reads message string of received messages from the router.
 */
#define READ_PACKET "READ_PACKET" //READ_PACKET [ROUTER_ID]

/*!
 *Clears the message string of received messages
 */
#define CLEAR_PACKET "CLEAR_PACKET" //CLEAR_PACKET [ROUTER_ID]

/*!
 * Sends a message to the given AS prefix
 */
#define SEND_PACKET "SEND_PACKET" //SEND_PACKET [DEST_PREFIX] [ROUTER_ID] [PAYLOAD]

/*!
 * Set the interfaceup that is been DISCONNECTED
 */
#define CONNECT "CONNECT" //CONNECT ROUTER_ID  [PORT_ID]

/*!
 * Puts the interface down in the given router
 */
#define DISCONNECT "DISCONNECT" //DISCONNECT [ROUTER_ID] [PORT_ID]

/*!
 * Terminates the simulations
 */
#define STOP "STOP" //STOP

/*!
 * Reads the routing table from the given router
 */
#define READ_TABLE "READ_TABLE" //READ_TABLE [ROUTER_ID]

/*!
 * Reads the raw table with all learned routes from the given router
 */
#define READ_RAW_TABLE "READ_RAW_TABLE" //READ_RAW_TABLE [ROUTER_ID]

/*!
 * Set the local preference value for the give AS in the given router
 */
#define SET_LOCAL_PREF "SET_LOCAL_PREF" //SET_LOCAL_PREF [ROUTER_ID] [AS_ID] [VALUE]

/*!
 * Set the keepalive time for the given router 
 */
#define SET_KEEPALIVE "SET_KEEPALIVE" // SET_KEEPALIVE ROUTER_ID [ROUTE_ID] [VALUE]

/*!
 * Set the hold-down time multiplier for the given router
 */
#define SET_HOLDDOWN_MULT "SET_HOLDDOWN_MULT" // SET_HOLDDOWN_MULT [ROUTER_ID] [VALUE]

/*!
 * return the interface status information as a string:
 * receivingbuf_available_slot_count, forwardingbuf_available_slot_count, interface_state
 */
#define SHOW_IF "SHOW_IF" // SHOW_IF [ROUTER_ID] [IF_ID]


///************************ Program modes *******************************

#define SIMULATION_DURATION 10

#define IF_COUNT 4

//#define _GUI

//#define _GUI_TEST
