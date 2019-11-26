/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: m62_driver.h
 *
 *      Author: Apb 
 *
 *  Description: Header file for M62 & M62N driver modules
 *               - M62 specific status codes
 *               - M62 function prototypes
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
 *---------------------------------------------------------------------------
 * Copyright 1997-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _M62_LLDRV_H
#  define M62_LLDRV_H

#  ifdef __cplusplus
      extern "C" {
#  endif

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define M62_MAX_CH     16

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
#ifdef _LL_DRV_

#   ifndef _ONE_NAMESPACE_PER_DRIVER_
        extern void M62_GetEntry( LL_ENTRY* drvP );
		extern void M62_SW_GetEntry(LL_ENTRY* drvP);
#   endif /* !_ONE_NAMESPACE_PER_DRIVER_ */

#endif /* _LL_DRV_ */

/*-----------------------------------------+
|  BACKWARD COMPATIBILITY TO MDIS4         |
+-----------------------------------------*/
#ifndef U_INT32_OR_64
 /* we have an MDIS4 men_types.h and mdis_api.h included */
 /* only 32bit compatibility needed!                     */
 #define INT32_OR_64  int32
 #define U_INT32_OR_64 u_int32
 typedef INT32_OR_64  MDIS_PATH;
#endif /* U_INT32_OR_64 */

#  ifdef __cplusplus
      }
#  endif

#endif/*_M62_LLDRV_H*/

