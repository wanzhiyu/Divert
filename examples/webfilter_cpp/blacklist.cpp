#include "blacklist.h"

/*
 * Attempt to parse a URL and match it with the blacklist.
 *
 * BUG:
 * - This function makes several assumptions about HTTP requests, such as:
 *      1) The URL will be contained within one packet;
 *      2) The HTTP request begins at a packet boundary;
 *      3) The Host header immediately follows the GET/POST line.
 *   Some browsers, such as Internet Explorer, violate these assumptions
 *   and therefore matching will not work.
 */
bool Blacklist::PayloadMatch(const Url& url) {
	return false;
}
