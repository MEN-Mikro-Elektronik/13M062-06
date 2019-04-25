/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: m62_main.c
 *      Project: MDIS5
 *
 *       Author: uf
 *        $Date: 2013/06/03 15:15:26 $
 *    $Revision: 1.6 $
 *
 *  Description: test of the m62_drv.c m62_drv.h
 *               signal handling and buffer modes
 *
 *     Required: -
 *     Switches: VWORKS
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m62_main.c,v $
 * Revision 1.6  2013/06/03 15:15:26  Apb
 * R: Porting to MDIS5
 * M: Changed according to MDIS Porting Guide 0.9
 *
 * Revision 1.5  2004/08/25 13:50:50  dpfeuffer
 * minor modifications for MDIS4/2004 conformity
 *
 * Revision 1.4  1999/07/30 17:34:32  Franke
 * cosmetics
 *
 * Revision 1.3  1998/07/17 17:57:08  Schmidt
 * include <MEN/dbg.h> added, DBG_LEV2 instead of OSS_DBG_LEVEL_2
 *
 * Revision 1.2  1998/07/15 17:42:52  Franke
 * changed to MDIS 4.1
 *
 * Revision 1.1  1998/02/19 17:17:23  franke
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <stdio.h>

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/m62_drv.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
static const int32 OFF = 0;
static const int32 ON  = 1;

static const int32 T_OK = 0;
static const int32 T_ERROR = 1;

/******************************* errShow *************************************
 *
 *  Description:  Show MDIS or OS error message.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *
 *  Output.....:  -
 *
 *  Globals....:  errno
 *
 ****************************************************************************/
static void errShow( void )
{
   u_int32 error;

   error = UOS_ErrnoGet();

   printf("*** %s ***\n",M_errstring( error ) );
}

static u_int16 VoltToHex( int32 Volt )
{
u_int16 ret;

    ret = (u_int16)Volt * (0x199);
    ret = ret << 4;

    return( ret );
}/*VoltToHex*/

/******************************** main **************************************
 *
 *  Description:  main() - function
 *
 *                - Open and close the device.
 *                - Check M_write and M_setblock.
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
    int16       wrVal;
    int32       oldDebugLevel;
    u_int16     buf[M62_MAX_CH]; /*usr buffer*/
    int         error;
     	
    if( argc < 2){
        printf("usage: m62_main <device name>\n");
        return 1;
    }
    
    devName = argv[1];

    printf("=========================\n");
    printf("m62 test\n");
    if( (fd = M_open(devName)) < 0 ) goto M62_TESTERR;

    /* debug level switch on for interrupt */
    if( M_getstat(fd, M_LL_DEBUG_LEVEL, &oldDebugLevel ) ) goto M62_TESTERR;
    oldDebugLevel = oldDebugLevel | 0x02 /*DBG_LEV2*/;
    if( M_setstat(fd, M_LL_DEBUG_LEVEL, oldDebugLevel ) ) goto M62_TESTERR;

    /*------------------------------------+
    |  check write                        |
    +-------------------------------------*/
    if( M_setstat( fd, M_MK_IO_MODE, M_IO_EXEC ) ) goto M62_TESTERR;
    printf("   write\n");
    error = 0;
    for( ch=0; ch<10; ch++)
    {
        M_setstat(fd, M_MK_CH_CURRENT, ch);    /* set current channel */
        wrVal = VoltToHex( ch+1 );
        if( M_write(fd, wrVal ) ) goto M62_TESTERR;
    }/*for*/

    for( ch=10; ch<15; ch++)
    {
        M_setstat(fd, M_MK_CH_CURRENT, ch);    /* set current channel */
        wrVal = VoltToHex( 0 );
        if( M_write(fd, (int32) wrVal ) ) goto M62_TESTERR;
    }/*for*/

    printf(" channel  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15\n");
    printf(" volt     1  2  3  4  5  6  7  8  9 10   0   0   0   0   0   0\n");

    UOS_Delay( 3000 );

    if( error )
        printf("    => Error\n");
    else
        printf("    => OK\n");

    /*------------------------------------+
    |  check block write                  |
    +-------------------------------------*/
    printf("   block write\n");
    error = 0;
    M_setstat(fd, M_MK_CH_CURRENT, 0);    /* set current channel */
    for( ch=0; ch<16; ch++)
    {
        if( ch < 7 )
            buf[ch] = VoltToHex( 0 );
        else
            buf[ch] = VoltToHex( ch-6 );
    }/*for*/

    if( (M_setblock( fd, (u_int8*) buf, M62_MAX_CH ) != M62_MAX_CH) )
    {
        error = T_ERROR;
        printf(" M_setblock() failed - nbr of written bytes\n");
    }/*if*/

    printf(" channel  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15\n");
    printf(" volt     0  0  0  0  0  0  0  1  2  3  4   5   6   7   8   9\n");

    UOS_Delay( 3000 );

    if( error )
        printf("    => Error\n");
    else
        printf("    => OK\n");

    if( M_close( fd ) ) goto M62_TESTERR;

    printf("    => OK\n");
    return( T_OK );

M62_TESTERR:
    errShow();
    printf("    => Error\n");
    M_close( fd );
    return( T_ERROR );

}/*m62_test*/

