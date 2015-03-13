/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: m62_driver.h
 *
 *      $Author: Apb $
 *        $Date: 2013/06/03 15:15:33 $
 *    $Revision: 1.3 $
 *
 *  Description: Header file for M62 & M62N driver modules
 *               - M62 specific status codes
 *               - M62 function prototypes
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m62_drv.h,v $
 * Revision 1.3  2013/06/03 15:15:33  Apb
 * R: Porting to MDIS5
 * M: Changed according to MDIS Porting Guide 0.9
 *
 * Revision 1.2  2004/08/25 13:51:02  dpfeuffer
 * - support swapped variant
 * - unused external declarations removed
 *
 * Revision 1.1  1998/02/18 13:44:53  franke
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
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

