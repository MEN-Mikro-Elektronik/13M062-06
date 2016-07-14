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
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m62_simp.c,v $
 * Revision 1.7  2013/06/03 15:15:22  Apb
 * R 1: Porting to MDIS5
 *   2: Syntax error on console output
 * M 1: Changed according to MDIS Porting Guide 0.9
 *   2: Syntax error corrected
 *
 * Revision 1.6  2004/08/25 13:50:48  dpfeuffer
 * minor modifications for MDIS4/2004 conformity
 *
 * Revision 1.5  2002/03/21 16:26:41  Schmidt
 * cosmetics
 *
 * Revision 1.4  1999/07/30 17:34:35  Franke
 * cosmetics
 *
 * Revision 1.3  1998/07/27 16:00:22  see
 * ramp info added
 *
 * Revision 1.2  1998/07/15 17:42:57  Franke
 * changed to MDIS 4.1
 *
 * Revision 1.1  1998/04/14 13:29:46  Franke
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

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
