/* -*- mode: c++; c-file-style: raknet; tab-always-indent: nil; -*- */
/**
 * @file
 * @brief Unique Player Identifier Class implementation 
 * 
 * This file is part of RakNet Copyright 2003, 2004 Rakkarsoft LLC and
 * Kevin Jenkins.
 * 
 * Usage of Raknet is subject to the appropriate licence agreement.
 * "Shareware" Licensees with Rakkarsoft LLC are subject to the
 * shareware license found at
 * http://www.rakkarsoft.com/shareWareLicense.html which you agreed to
 * upon purchase of a "Shareware license" "Commercial" Licensees with
 * Rakkarsoft LLC are subject to the commercial license found at
 * http://www.rakkarsoft.com/sourceCodeLicense.html which you agreed
 * to upon purchase of a "Commercial license" All other users are
 * subject to the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * Refer to the appropriate license agreement for distribution,
 * modification, and warranty rights.
 */
#include "NetworkTypes.h"
#pragma pack(1)

int operator==( const PlayerID& left, const PlayerID& right )
{
	return left.binaryAddress == right.binaryAddress && left.port == right.port;
}

int operator!=( const PlayerID& left, const PlayerID& right )
{
	return left.binaryAddress != right.binaryAddress || left.port != right.port;
}

int operator>( const PlayerID& left, const PlayerID& right )
{
	return ( ( left.binaryAddress > right.binaryAddress ) || ( ( left.binaryAddress == right.binaryAddress ) && ( left.port > right.port ) ) );
}

int operator<( const PlayerID& left, const PlayerID& right )
{
	return ( ( left.binaryAddress < right.binaryAddress ) || ( ( left.binaryAddress == right.binaryAddress ) && ( left.port < right.port ) ) );
}
