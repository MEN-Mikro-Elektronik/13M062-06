/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: m62_ll.c
 *      Project: MDIS5
 *
 *       Author: uf
 *        $Date: 2013/06/03 15:15:24 $
 *    $Revision: 1.3 $
 *
 *  Description: simple example for using M62 Low Level Driver direct
 *
 *     Required: -
 *     Switches: VXWORKS
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m62_ll.c,v $
 * Revision 1.3  2013/06/03 15:15:24  Apb
 * R: Porting to MDIS5
 * M: Changed according to MDIS Porting Guide 0.9
 *
 * Revision 1.2  1999/07/30 17:34:37  Franke
 * changed -  MDIS 4.1 debug handling
 *
 * Revision 1.1  1998/04/14 13:29:57  Franke
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static char *RCSid="$Id: m62_ll.c,v 1.3 2013/06/03 15:15:24 Apb Exp $\n";

#include <stdio.h>

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/mbuf.h>       /* buffer lib functions and types */
#include <MEN/maccess.h>    /* hw access macros and types     */

#include <MEN/oss.h>    /* operating system services */
#include <MEN/desc.h>   /* descriptor manager        */
#include <MEN/ll_defs.h>    /* low level driver definitions   */
#include <MEN/ll_entry.h>   /* low level driver entry struct  */

#define _LL_DRV_
#define M_ERRSTRING
#include <MEN/dbg.h>
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

static const int32 CH_DUMMY  = 0;
static const int32 CH_0      = 0;
static const int32 CH_31     = 31;

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

void m62_ll( void );

#ifdef VXWORKS
    #define  OS_HDL		       	NULL   /* OS Handle not used */
    #define  DEV_SEM_NULL_HDL   NULL   /* Device semaphore handle only used by MDIS Kernel */
    #define  IRQ_NULL_HDL       NULL   /* no irq's used in this example */
    extern   DESC_SPEC          M62_1; /* low level descriptor specifier  */
#else
    /*
    *  OS like OS9, HPRT, LYNX
    */
    void main( void )
    {
        m62_ll();
    }

#endif/*VXWORKS*/

static void delay()
{
volatile int i,j;
    for( i=0;i<2000;i++)
        for( j=0;j<1000;j++);
}/*delay*/

/******************************** m62_ll ***********************************
 *
 *  Description:  This function shows the use of:
 *                    M62_Init()
 *                    M62_Write()
 *                    M62_BlockWrite()
 *                    M62_Exit()
 *
 *                It initialize the module, sets channel 0 to max (M62_Write),
 *                writes a pattern to all channels (M62_BlockWrite)
 *                and deinitialize the module.
 *
 *---------------------------------------------------------------------------
 *  Input......:  void
 *
 *  Output.....:  return  ---
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
void m62_ll( void )
{
int32      ret;
MACCESS    ma[1];                /* access handle */
LL_HANDLE  *llDrvHdl;            /* ll drv handle */
int32      value;                /* value to write/read */
u_int8     buffer[M62_MAX_CH];   /* block write/read buffer */
int32      nbrOfBytes;           /* number of written/read bytes */
int        loopy;

	printf("%s\n", RCSid );
    printf("m62_ll - started ...\n");

    /* set the base address of the device */
	ma[0] = (MACCESS) 0x88aaac00; /* VME A08/D16 range */

	/* gets the ll-drv handle, decodes descriptor, init HW */
    ret = M62_Init( &M62_1,                   /* LL-Descriptor handle */
                    OS_HDL,                   /* operating system handle */
                    ma,				          /* access handle */
                    DEV_SEM_NULL_HDL,         /* device semaphore handle -  not used in this example */
                    IRQ_NULL_HDL,             /* irq handle - not used in this example */
                    &llDrvHdl );		      /* pointer to ll drv handle */
    if( ret != 0 )
    {
        printf("*** ERROR - M62_Init()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    /* sets channel 0 to max */
    value = 0xfff;
    ret = M62_Write( llDrvHdl,
                     CH_0,
                     value << 4 );
    if( ret != 0 )
    {
        printf("*** ERROR - M62_Write()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    delay();

    /* prepare output pattern - ch0 min, ch15 max */
    for( loopy = 0; loopy < M62_MAX_CH; loopy++ )
        buffer[value] = (value * 0xC3) << 4;
    /* set output pattern */
    ret = M62_BlockWrite( llDrvHdl,
                          CH_DUMMY,
                          buffer,
                          M62_MAX_CH,
                          &nbrOfBytes );
    if( ret != 0 )
    {
        printf("*** ERROR - M62_BlockWrite()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    delay();

    /* setstat enable debug - task level 1 */
    ret = M62_SetStat( llDrvHdl,
                       M_LL_DEBUG_LEVEL,
                       CH_DUMMY,
                       DBG_NORM |DBG_LEV1 );
    if( ret != 0 )
    {
        printf("*** ERROR - M62_SetStat()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    /* getstat ID_CHECK at M62_Init() */
    ret = M62_GetStat( llDrvHdl,
                       M_LL_ID_CHECK,
                       CH_DUMMY,
                       &value );
    if( ret != 0 )
    {
        printf("*** ERROR - M62_GetStat()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    /* deinitialize the M62 and free the low level handle */
    ret = M62_Exit( &llDrvHdl );
    if( ret != 0 )
    {
        printf("*** ERROR - M62_Exit()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/


    printf("m62_ll - finished - OK\n");
    return;

    /* jump to if error occurred */
LL_EXAMPLE_ERROR:
    printf(" with error 0x%04x ***\n", (int) ret );

#ifdef M_ERRSTRING
    printf("*** %s ***\n",M_errstring( ret ) );
#endif

}/*m62_ll*/

