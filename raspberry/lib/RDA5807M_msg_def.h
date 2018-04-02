/* RDA5807M_msg_def
 * Brief: msg definition.
 *
 * Evolution 02-Apr-2018   Thibaut .Creation
 *
 */

#ifndef RDA5807M_MSG_DEF_H
#define RDA5807M_MSG_DEF_H

/*
 * Message format
 *
 * From the bridge
 *  ----------------------------------------------------------------
 * | 's' | <Payload size> | <Argument 0> | ... | <Argument N> | 'E' |
 *  ----------------------------------------------------------------
 * Example:
 *  Send
 *   --------------------------------------------
 *  | 'S' | 2 | <High value> | <Low value> | 'E' |
 *   --------------------------------------------
 *
 * From the proxy
 *  ----------------------------------------------------------------------------
 * | 'S' | <Payload size> | <Command> | <Argument 0> | ... | <Argument N> | 'E' |
 *  ----------------------------------------------------------------------------
 * Example:
 *  Read:
 *   ------------------------------------------
 *  | 'S' | 2 | 'r' | <Register address> | 'E' |
 *   ------------------------------------------ 
 *  Write
 *   -----------------------------------------------------------------------
 *  | 'S' | 4 | 'w' | <Register address> | <High value> | <Low value> | 'E' |
 *   -----------------------------------------------------------------------
 */

/*
 * Brief: SPI max size
 */
#define RDA5807M_BRIDGE_MAX_MSG_SIZE 10

/*
 * Brief: Size of the msg (from the bridge) payload.
 */
#define RDA5807M_BRIDGE_MSG_PAYLOAD_SIZE 2

/*
 * Brief: Byte used to indentify the beginning
 * of the msg (from the bridge)
 */
#define RDA5807M_BRIDGE_MSG_START 's'

/*
 * Brief: Byte used to indentify the acknowledge
 * of the msg (from the bridge)
 */
#define RDA5807M_BRIDGE_MSG_ACK   'a'

/*
 * Brief: Byte used to indentify the endding
 * of the msg (from the bridge)
 */
#define RDA5807M_BRIDGE_MSG_STOP  'e'

/*
 * Brief: Index of the first acknowledge
 */
#define RDA5807M_BRIDGE_MSG_ACK_INDEX_1  1

/*
 * Brief: Byte used to indentify the beginning
 * of the msg (from the proxy)
 */
#define RDA5807M_PROXY_MSG_START 'S'

/*
 * Brief: Byte used to indentify the command
 * 'read'
 */
#define RDA5807M_PROXY_MSG_READ  'r'

/*
 * Brief: Byte used to indentify the command
 * 'write'
 */
#define RDA5807M_PROXY_MSG_WRITE 'w'

/*
 * Brief: Byte used to indentify the endding
 * of the msg (from the proxy)
 */
#define RDA5807M_PROXY_MSG_STOP  'E'

#endif //RDA5807M_MSG_DEF_H