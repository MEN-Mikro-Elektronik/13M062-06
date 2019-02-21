/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: m62_simp.c
 *      Project: MDIS5
 *
 *       Author: uf
 *        $Date: 2013/06/03 15:15:22 $
 *    $Revision: 1.7 $
 *
 *  Description: simple test of the m62 mdis driver
 *
 *     Required: -
 *     Switches: -
 *
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2019, MEN Mikro Elektronik GmbH
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

static char *RCSid="$Id: m62_simp.c,v 1.7 2013/06/03 15:15:22 Apb Exp $\n";

#include <stdio.h>
#include <string.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>

#include <MEN/m62_drv.h>

/******************************** main **************************************
 *
 *  Description:  main() - function
 *
 *                - Opens the device.
 *				  - Sets channel from min to max with M_write().
 *				  - Sets channels 0..15 with M_setblock().
 *				  - Close the device.
 * 
 *---------------------------------------------------------------------------
 *  Input......:  argc      number of arguments
 *                *argv     pointer to arguments
 *                argv[1]   device name
 *
 *  Output.....:  return    0   if no error
 *                          1   if error
 *
 *  Globals....:  -
 ****************************************************************************/
 int main( int argc, char *argv[ ] )
 {
	char        *devName;
    MDIS_PATH   fd;
    int32       ch;
    int16       value;
    u_int16     buf[M62_MAX_CH]; /*usr buffer*/
    u_int32     error;
     	
    if( argc < 2){
        printf("usage: m62_main <device name>\n");
        return 1;
    }
    
    devName = argv[1];

    printf("%s\n", RCSid );
    
	/******************/
	/*     M_open     */
    /******************/
	/* open device - error information */
	if( (fd = M_open( devName )) < 0 )
    {
        error = UOS_ErrnoGet();
        printf( "*** %s ***\n", M_errstring( error ) );
		return 1;
    }/*if*/

	/******************/
	/*   M_setstat    */
	/******************/
	/* select channel 0 */
      ch = 0;
	M_setstat( fd, M_MK_CH_CURRENT, ch );													

	/******************/
	/*    M_write     */
	/******************/
	/* turn channel 0 from low  to  high */
	/* select channel n */
	M_setstat( fd, M_MK_CH_CURRENT, 0 );
	printf("channel 0: produce lowest to highest ramp...\n");
	fflush(stdout);
	for( value=0x000; value <= 0xFFF; value+=0x4 )
    {
		M_write( fd, value<<4 );
        UOS_Delay( 1 ); /* delay at least 1 ms */
	}/*for*/
	printf("\n");
	fflush(stdout);

	/******************/
	/*  M_setblock    */
     /******************/
     /* ch0 = 0V ... ch15 = 10V, output range 0..10V */
     for( value = 0; value < M62_MAX_CH; value++ )
         buf[value] = (value * 0xC3) << 4;

	printf("all channels: output range 0..10V..\n");
	fflush(stdout);
	M_setblock(  fd, (u_int8*)buf, M62_MAX_CH * 2 ); /* set all channels */

    UOS_Delay( 2500 ); /* delay 2500 ms */
	printf("\n");
	fflush(stdout);

	/******************/
	/*     M_close    */
     /******************/
	M_close( fd );                              /* terminate */
	
	return 0;
}
