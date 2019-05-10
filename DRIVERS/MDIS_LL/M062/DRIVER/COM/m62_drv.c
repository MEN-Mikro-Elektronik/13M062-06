/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: m62_drv.c
 *      Project: M62 module driver for MDIS5
 *
 *       Author: uf
 *        $Date: 2013/06/03 15:15:20 $
 *    $Revision: 1.12 $
 *
 *  Description: Low level driver for M62 modules
 *
 *     Required: ---
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1995-2019, MEN Mikro Elektronik GmbH
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
static const char IdentString[]="M62 - m62 low level driver: $Id: m62_drv.c,v 1.12 2013/06/03 15:15:20 Apb Exp $";

#include <MEN/men_typs.h>   /* system dependend definitions   */
#include <MEN/dbg.h>
#include <MEN/oss.h>        /* os services                    */
#include <MEN/mdis_err.h>   /* mdis error numbers             */
#include <MEN/maccess.h>    /* hw access macros and types     */
#include <MEN/desc.h>       /* descriptor functions           */
#include <MEN/mdis_api.h>   /* global set/getstat codes       */
#include <MEN/mdis_com.h>   /* info function      codes       */
#include <MEN/microwire.h>  /* id prom access                 */

#include <MEN/ll_defs.h>    /* low level driver definitions   */
#include <MEN/ll_entry.h>   /* low level driver entry struct  */
#include <MEN/m62_drv.h>    /* M62 driver header file         */



/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define DBG_MYLEVEL     m62Hdl->dbgLevel
#define DBH             m62Hdl->dbgHdl
#define M62_CH_WIDTH    2    /* byte */

#define MOD_ID_MAGIC		0x5346      /* eeprom identification (magic) */
#define MOD_ID_SIZE			128			/* eeprom size */
#define MOD_ID_M62		    0x3e		/* module id = 62 dez. */
#define MOD_ID_M62N		    0x7d3e		/* module id M62N */

#define M62_HW_ACCESS_NO              0
#define M62_HW_ACCESS_PERMITED        1


/*--------------------- M62 register defs/offsets ------------------------*/
#define OUT_REG(ch) ( (ch)<<1 ) /* data output register */
#define M62_ID_RESET_REG  0xfe  /* software reset register and M-Module ID */
#define M62_RANGE_REG     0x40  /* M62N: Rangeselect Reg. (uni/bipolar) */

#define M62_DEFAULT_POLARITY 1  /* M62N: Startup is unipolar, 0..10V */

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
typedef struct
{
    int32           ownMemSize;
    u_int32         dbgLevel;
    DBG_HANDLE      *dbgHdl;        /* debug handle */
    OSS_HANDLE      *osHdl;         /* for complicated os */
    MCRW_HANDLE     *mcrwHdl;
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	/* id function table */
    MACCESS         ma62;
    int32           nbrOfChannels;
    u_int32         useModulId;
    u_int32         unipol;			/* 1: output 0..10V  0: -10..10 V */
    u_int32         modId;			/* the MOD_ID read from EEprom */
    u_int16         modIdBuf[MOD_ID_SIZE/2]; /* must be read once at init
												because the reset bit is 
												located at EEDAT */
} M62_HANDLE;

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int32 memCleanup( M62_HANDLE *m62Hdl );
static int32 getStatBlock(
    M62_HANDLE         *m62Hdl,
    int32              code,
    M_SETGETSTAT_BLOCK *blockStruct );
static char* ident( void );

static int32 M62_Init(DESC_SPEC *descSpec, OSS_HANDLE *osHdl,
					   MACCESS *ma, OSS_SEM_HANDLE *devSemHdl,
					   OSS_IRQ_HANDLE *irqHdl, LL_HANDLE **llHdlP);
static int32 M62_Exit(LL_HANDLE **llHdlP );
static int32 M62_Read(LL_HANDLE *llHdl, int32 ch, int32 *value);
static int32 M62_Write(LL_HANDLE *llHdl, int32 ch, int32 value);
static int32 M62_SetStat(LL_HANDLE *llHdl,int32 ch, int32 code, INT32_OR_64 value32_or_64);
static int32 M62_GetStat(LL_HANDLE *llHdl, int32 ch, int32 code, INT32_OR_64 *value32_or_64P);
static int32 M62_BlockRead(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size,
							int32 *nbrRdBytesP);
static int32 M62_BlockWrite(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size,
							 int32 *nbrWrBytesP);
static int32 M62_Irq(LL_HANDLE *llHdl );
static int32 M62_Info(int32 infoType, ... );

/*****************************  ident  **********************************
 *
 *  Description:  Gets the pointer to ident string.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
static char* ident( void )	/* nodoc */
{
    return( (char*) IdentString );
}/*ident*/


/**************************** memCleanup *********************************
 *
 *  Description:  deallocates ll-drv structure installed signale and buffer
 *
 *---------------------------------------------------------------------------
 *  Input......:  m62Hdl   m62 low level driver handle
 *  Output.....:  return   0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 memCleanup( M62_HANDLE *m62Hdl )	/* nodoc */
{
int32       retCode;

	if( m62Hdl->mcrwHdl )
		m62Hdl->mcrwHdl->Exit( (void**)&m62Hdl->mcrwHdl );

    /* cleanup debug */
    DBGEXIT((&DBH));

    /*-------------------------------------+
    | free ll handle                       |
    +-------------------------------------*/
    /* dealloc lldrv memory */
    retCode = OSS_MemFree( m62Hdl->osHdl, (int8*) m62Hdl, m62Hdl->ownMemSize );


    return( retCode );
}/*memCleanup*/

/**************************** M62_GetEntry *********************************
 *
 *  Description:  Initialize drivers jump table
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  drvP  pointer to the initialized jump table structure
 *  Globals....:  ---
 ****************************************************************************/
#ifdef _ONE_NAMESPACE_PER_DRIVER_
    void LL_GetEntry( LL_ENTRY* drvP )
#else
# ifdef	MAC_BYTESWAP
	void M62_SW_GetEntry( LL_ENTRY*	drvP )
# else
	void M62_GetEntry( LL_ENTRY* drvP )
# endif
#endif
{
    drvP->init        = M62_Init;
    drvP->exit        = M62_Exit;
    drvP->read        = M62_Read;
    drvP->write       = M62_Write;
    drvP->blockRead   = M62_BlockRead;
    drvP->blockWrite  = M62_BlockWrite;
    drvP->setStat     = M62_SetStat;
    drvP->getStat     = M62_GetStat;
    drvP->irq         = M62_Irq;
    drvP->info        = M62_Info;

}/*M62_GetEntry*/

/******************************** M62_Init ***********************************
 *
 *  Description:  Decodes descriptor , allocates and initializes the ll-driver
 *                structure. Initializes the hardware.
 *                Reads and checks the ID if it is enabled in the descriptor.
 *                Sets all outputs to 0V, 0mA or 4mA. (depends on HW model)
 *                The driver supports 16 analog output channels.
 *
 *                Deskriptor key        Default         Range/Unit
 *                ------------------------------------------------------------
 *                DEBUG_LEVEL_DESC      OSS_DBG_DEFAULT see oss_os.h
 *                DEBUG_LEVEL           OSS_DBG_DEFAULT see oss_os.h
 *
 *                ID_CHECK              1               0..1    0 - disabled
 *                                                              1 - enabled
 *
 *---------------------------------------------------------------------------
 *  Input......:  descSpec   pointer to descriptor data
 *                osHdl      oss handle
 *                ma         hw access handle
 *                devSem     device semaphore handle
 *                irqHdl     irq handle
 *  Output.....:  llHdlP     ptr to low level driver handle
 *                return     success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 M62_Init
(
    DESC_SPEC       *descSpec,
    OSS_HANDLE      *osHdl,
    MACCESS         *ma,
    OSS_SEM_HANDLE  *devSem,
    OSS_IRQ_HANDLE  *irqHdl,
    LL_HANDLE       **llHdlP
)
{
    DBGCMD( static const char functionName[] = "LL - M62_Init()"; )
    M62_HANDLE  *m62Hdl;
    int32       retCode;
    u_int32     gotsize;
    DESC_HANDLE *descHdl;
    int         hwAccess;
    u_int16     modIdMagic;
    u_int16     modId;
    u_int32     dbgLevelDesc;

    hwAccess = M62_HW_ACCESS_NO;

    retCode = DESC_Init( descSpec, osHdl, &descHdl );
    if( retCode )
    {
        return( retCode );
    }/*if*/

    /*-------------------------------------+
    |  LL-HANDLE allocate and initialize   |
    +-------------------------------------*/
    m62Hdl = (M62_HANDLE*) OSS_MemGet( osHdl, sizeof(M62_HANDLE), &gotsize );
    if( m62Hdl == NULL )
    {
       *llHdlP = 0;
       return( ERR_OSS_MEM_ALLOC );
    }/*if*/

    *llHdlP = (LL_HANDLE*) m62Hdl;

    /* fill turkey with 0 */
    OSS_MemFill( osHdl, gotsize, (char*) m62Hdl, 0 );

    m62Hdl->ownMemSize = gotsize;
    m62Hdl->osHdl      = osHdl;
    m62Hdl->ma62       = *ma;
    m62Hdl->nbrOfChannels = M62_MAX_CH;


    DBG_MYLEVEL = OSS_DBG_DEFAULT;
    DBGINIT((NULL,&DBH));

    /*-------------------------------------+
    |   get DEBUG LEVEL                    |
    +-------------------------------------*/
    /* DEBUG_LEVEL_DESC */
    retCode = DESC_GetUInt32( descHdl,
                              OSS_DBG_DEFAULT,
                              &dbgLevelDesc,
                              "DEBUG_LEVEL_DESC",
                              NULL );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
    DESC_DbgLevelSet(descHdl, dbgLevelDesc);
    retCode = 0;

	retCode = DESC_GetUInt32( descHdl,
							  OSS_DBG_DEFAULT,
							  &m62Hdl->dbgLevel,
							  "DEBUG_LEVEL",
							  NULL );
	if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
	retCode = 0;


    retCode = DESC_GetUInt32( descHdl, 
							  M62_DEFAULT_POLARITY, 
							  &m62Hdl->unipol, 
							  "UNIPOL", 
							  NULL );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) 
		goto CLEANUP;
    retCode = 0;

    DBGWRT_1((DBH, "%s\n", functionName )  );

    /*-------------------------------------+
    |  descriptor - use module id ?        |
    +-------------------------------------*/
    retCode = DESC_GetUInt32( descHdl,
                              1,
                              &m62Hdl->useModulId,
                              "ID_CHECK",
                              NULL );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) 
		goto CLEANUP;
    retCode = 0;

    /*---------------------------+
    |  MICROWIRE Library Handle  |
    +---------------------------*/
	{
		MCRW_DESC_PORT   descMcrw;

		/* clear the structure */
		OSS_MemFill( m62Hdl->osHdl, sizeof(descMcrw), (char*)&descMcrw, 0 );

		/* bus speed */
		descMcrw.busClock   = 10; /* OSS_MikroDelay */

		/* address length */
		descMcrw.addrLength   = 6; /* for 93C46 */

		/* set FLAGS */
		descMcrw.flagsDataIn   = 
		  ( MCRW_DESC_PORT_FLAG_SIZE_16 | MCRW_DESC_PORT_FLAG_READABLE_REG | 
			MCRW_DESC_PORT_FLAG_POLARITY_HIGH );

		descMcrw.flagsDataOut  =
		  ( MCRW_DESC_PORT_FLAG_SIZE_16 | MCRW_DESC_PORT_FLAG_POLARITY_HIGH );

		descMcrw.flagsClockOut =
		  ( MCRW_DESC_PORT_FLAG_SIZE_16 | MCRW_DESC_PORT_FLAG_POLARITY_HIGH );

		descMcrw.flagsCsOut =
		  ( MCRW_DESC_PORT_FLAG_SIZE_16 | MCRW_DESC_PORT_FLAG_POLARITY_HIGH );

		descMcrw.flagsOut   = MCRW_DESC_PORT_FLAG_OUT_IN_ONE_REG;

		/* set addr and mask */
		descMcrw.addrDataIn   = (char*)m62Hdl->ma62 + M62_ID_RESET_REG;
		descMcrw.maskDataIn   = 0x0001; /*EEDAT*/

		descMcrw.addrDataOut  = (char*)m62Hdl->ma62 + M62_ID_RESET_REG;
		descMcrw.maskDataOut  = 0x0001; /*EEDAT*/
		descMcrw.notReadBackDefaultsDataOut = 0xFFFE;
		descMcrw.notReadBackMaskDataOut     = 0xFFFE;

		descMcrw.addrClockOut = (char*)m62Hdl->ma62 + M62_ID_RESET_REG;
		descMcrw.maskClockOut = 0x0002; /*EECLK*/
		descMcrw.notReadBackDefaultsClockOut = 0xFFFD;
		descMcrw.notReadBackMaskClockOut     = 0xFFFD;

		descMcrw.addrCsOut	 = (char*)m62Hdl->ma62 + M62_ID_RESET_REG;
		descMcrw.maskCsOut	 = 0x0004; /*EECS*/
		descMcrw.notReadBackDefaultsCsOut = 0xFFFB;
		descMcrw.notReadBackMaskCsOut     = 0xFFFB;

	    retCode = MCRW_PORT_Init( &descMcrw,
			                   	  m62Hdl->osHdl,
   	        		              (void**)&m62Hdl->mcrwHdl );
   	    if( retCode || m62Hdl->mcrwHdl == NULL )
   	    {
             DBGWRT_ERR( ( DBH, "*** %s: MCRW_PORT_Init() error=%d\n",
                          functionName, retCode ));
             retCode = ERR_ID;
             goto CLEANUP;
   	    }/*if*/
	}/*MCRW INIT*/

    if( m62Hdl->useModulId)
    {
    	/* read MAGIC and module type */
        retCode = m62Hdl->mcrwHdl->ReadEeprom( (void*)m62Hdl->mcrwHdl, 
											   0, 
											   &modIdMagic,
                                               sizeof(modIdMagic) );
        if( retCode )
        {
             DBGWRT_ERR( ( DBH, "*** %s: ReadEeprom() error=%d\n",
                          functionName, retCode ));
             retCode = ERR_ID_NOTFOUND;
             goto CLEANUP;
        }/*if*/
        retCode = m62Hdl->mcrwHdl->ReadEeprom( (void*)m62Hdl->mcrwHdl, 
											   2, 
											   &modId,
        									   sizeof(modId) );
        if( retCode )
        {
             DBGWRT_ERR( ( DBH, "*** %s: ReadEeprom() error=%d\n",
                          functionName, retCode ));
             retCode = ERR_ID_NOTFOUND;
             goto CLEANUP;
        }/*if*/
        if( modIdMagic != MOD_ID_MAGIC )
        {
             DBGWRT_ERR((DBH, "*** %s: module id - illegal magic word 0x%04x\n",
                          functionName, modIdMagic ));
             retCode = ERR_LL_ILL_ID;
             goto CLEANUP;
        }/*if*/

        if( (modId != MOD_ID_M62) && (modId != MOD_ID_M62N))
        {
             DBGWRT_ERR((DBH, "*** %s: module id - illegal module id 0x%04x\n",
                          functionName, modId ));
             retCode = ERR_LL_ILL_ID;
             goto CLEANUP;
        }/*if*/
        
        DBGWRT_2(( DBH, "%s: module id OK (0x%04x = %d dez)\n", 
				   functionName, modId, modId ));

		m62Hdl->modId = modId;

        /* read the whole ID PROM once at init */
		retCode = m62Hdl->mcrwHdl->ReadEeprom( m62Hdl->mcrwHdl, 
		 									   0,
			  								   m62Hdl->modIdBuf,
                                               sizeof(m62Hdl->modIdBuf) );
		if( retCode )
		{
        	DBGWRT_ERR( ( DBH, "*** %s: whole ID PROM ReadEeprom() error=%d\n",
            			 functionName, retCode ));
        	retCode = ERR_ID;
	    	goto CLEANUP;
		}/*if*/
    }/*if*/

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* drivers ident function */
	m62Hdl->idFuncTbl.idCall[0].identCall = ident;
	/* libraries ident functions */
	m62Hdl->idFuncTbl.idCall[1].identCall = DESC_Ident;
	m62Hdl->idFuncTbl.idCall[2].identCall = OSS_Ident;
	m62Hdl->idFuncTbl.idCall[3].identCall = m62Hdl->mcrwHdl->Ident;

	/* terminator */
	m62Hdl->idFuncTbl.idCall[4].identCall = NULL;


    /*------------------------------+
    |  Init HW                      |
    +------------------------------*/
    hwAccess = M62_HW_ACCESS_PERMITED;

	/* reset channels to 0 V */
    MWRITE_D16( m62Hdl->ma62, M62_ID_RESET_REG, 0x00 );
    MWRITE_D16( m62Hdl->ma62, M62_ID_RESET_REG, 0x01 );

	/* Set polarity mode from Descriptor */
	if (m62Hdl->modId == MOD_ID_M62N)
		MWRITE_D16( m62Hdl->ma62, M62_RANGE_REG, m62Hdl->unipol ? 1 : 0 );

    DESC_Exit( &descHdl );
    return( retCode );

 CLEANUP:
    DESC_Exit( &descHdl );
    if( hwAccess == M62_HW_ACCESS_PERMITED )
        M62_Exit( llHdlP );
    else
    {
        memCleanup( m62Hdl );
        *llHdlP = 0;
    }/*if*/

    return( retCode );

}/*M62_Init*/

/****************************** M62_Exit *************************************
 *
 *  Description:  Deinits HW, disables interrupts, frees allocated memory.
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdlP  	ptr to low level driver handle
 *  Output.....:  return    success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 M62_Exit
(
    LL_HANDLE **llHdlP
)
{
    DBGCMD( static const char functionName[] = "LL - M62_Exit()"; )
    M62_HANDLE  *m62Hdl = (M62_HANDLE*) *llHdlP;
    int32       retCode;

    DBGWRT_1((DBH, "%s\n", functionName) );

    /*--------------------------+
    | output off                |
    +--------------------------*/

    MWRITE_D16( m62Hdl->ma62, M62_ID_RESET_REG, 0x00 );
    MWRITE_D16( m62Hdl->ma62, M62_ID_RESET_REG, 0x01 );

    retCode = memCleanup( m62Hdl );
    *llHdlP = 0;

    return( retCode );
}/*M62_Exit*/

/****************************** M62_Read *************************************
 *
 *  Description:  Do nothing.
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl   pointer to low-level driver data structure
 *                ch      current channel
 *                valueP  pointer to variable where read value stored
 *  Output.....:  *valueP  read value
 *                return   always ERR_LL_ILL_FUNC
 *  Globals....:  ---
 ****************************************************************************/
int32 M62_Read
(
    LL_HANDLE   *llHdl,
    int32       ch,
    int32       *valueP
)
{
    return( ERR_LL_ILL_FUNC );
}/*M62_Read*/

/****************************** M62_Write ************************************
 *
 *  Description:  Writes 2 bytes to current channel.
 *
 *                byte structure of value
 *
 *                   bit        15   14..5    4    3..1
 *                           +-----+- - - -+-----+- - -+
 *                meaning    | msb |       | lsb |  no |
 *                           +-----+- - - -+-----+- - -+
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl  pointer to low-level driver data structure
 *                ch     current channel 0..15
 *                value  output value 12 bit in the left justified word
 *  Output.....:  return 0 | error code
 *  Globals....:  ---
 ****************************************************************************/
int32 M62_Write
(
    LL_HANDLE *llHdl,
    int32  ch,
    int32  value
)
{
    DBGCMD( static const char functionName[] = "LL - M62_Write"; )
    M62_HANDLE*       m62Hdl = (M62_HANDLE*) llHdl;

    DBGWRT_1((DBH, "%s to ch=%d\n", functionName, ch) );

    MWRITE_D16( m62Hdl->ma62, OUT_REG(ch), (u_int16)value );

    return(0);
}/*M62_Write*/

/****************************** M62_SetStat **********************************
 *
 *  Description:  Changes the device state.
 *
 *      common codes            values          meaning
 *      ---------------------------------------------------------------------
 *      M_LL_CH_DIR             M_CH_OUT        channel direction (always out)
 *
 *      M_LL_DEBUG_LEVEL        see oss.h       enable/disable debug output
 *
 *
 *      specific codes          values          meaning
 *      ---------------------------------------------------------------------
 *      ---                     ---             ---
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl         pointer to low-level driver data structure
 *                code          setstat code
 *                ch            current channel (ignored for some codes)
 *                value32_or_64 setstat value or pointer to blocksetstat data
 *  Output.....:  return 0 | error code
 *  Globals....:  -
 ****************************************************************************/
int32 M62_SetStat
(
    LL_HANDLE *llHdl,
    int32  code,
    int32  ch,
    INT32_OR_64 value32_or_64
)
{
    int32       value   = (int32)value32_or_64; /* 32bit value     */

    DBGCMD( static const char functionName[] = "LL - M62_SetStat()"; )
    M62_HANDLE *m62Hdl = (M62_HANDLE*) llHdl;

    DBGWRT_1((DBH, "%s code=$%04lx data=%ld\n", functionName, code, value) );

    switch(code)
    {
        /* ------ common setstat codes --------- */
        /*------------------+
        |  ch direction     |
        +------------------*/
        case M_LL_CH_DIR:
            if( value != M_CH_OUT )
                return( ERR_LL_ILL_PARAM );
            break;

        /*------------------+
        |  debug level      |
        +------------------*/
        case M_LL_DEBUG_LEVEL:
            m62Hdl->dbgLevel = value;
            break;



        /* ------ special setstat codes --------- */
        /*--------------------+
        |  (unknown)          |
        +--------------------*/
        default:
            DBGWRT_ERR( ( DBH, "*** %s:  unkown setstat code\n",
                          functionName ));
            return(ERR_LL_UNK_CODE);
    }/*switch*/

    return(0);
}/*M62_SetStat*/

/****************************** M62_GetStat **********************************
 *
 *  Description:  Gets the device state.
 *
 *      common codes            values          meaning
 *      ---------------------------------------------------------------------
 *      M_LL_CH_NUMBER          16              number of channels
 *
 *      M_LL_CH_DIR             M_CH_OUT        direction of curr ch
 *                                               always out
 *
 *      M_LL_CH_LEN             12              channel length in bit
 *
 *      M_LL_CH_TYP             M_CH_ANALOG     channel type
 *
 *      M_LL_ID_CHECK           0 or 1          check EEPROM-Id from module
 *
 *      M_LL_DEBUG_LEVEL        see oss.h       current debug level
 *
 *      M_LL_ID_SIZE            128             eeprom size [bytes] 
 *
 *		M_LL_BLK_ID_DATA		-               eeprom raw data
 *
 *      M_MK_BLK_REV_ID         pointer to the ident function table
 *
 *      M_BUF_WR_WIDTH          2               channel width [byte]
 *
 *      M_BUF_WR_MODE           M_BUF_USRCTRL   mode of write buffer
 *                                              (buffer always controlled by user)
 *
 *      specific codes          values          meaning
 *      ---------------------------------------------------------------------
 *      ---                     ---             ---
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl             pointer to low-level driver data structure
 *                code              getstat code
 *                ch                current channel
 *                value32_or_64P    pointer to variable where value stored or
 *                                  pointer to blockgetstat data
 *  Output.....:  *value32_or_64P   getstat value or pointer to blockgetstat data
 *                return   0 | error code
 *  Globals....:  ---
 ****************************************************************************/
int32 M62_GetStat
(
    LL_HANDLE *llHdl,
    int32  code,
    int32  ch,
    INT32_OR_64 *value32_or_64P
)
{
    int32       *valueP     = (int32*)value32_or_64P;   /* pointer to 32bit value  */
    INT32_OR_64 *value64P   = value32_or_64P;           /* stores 32/64bit pointer  */

    DBGCMD( static const char functionName[] = "LL - M62_GetStat()"; )
    M62_HANDLE  *m62Hdl = (M62_HANDLE*) llHdl;

    DBGWRT_1((DBH, "%s code=$%04lx\n", functionName, code) );
	
	
    switch(code)
    {
        /* ------ common getstat codes --------- */
        /*------------------+
        |  get ch count     |
        +------------------*/
        case M_LL_CH_NUMBER:
            *valueP = m62Hdl->nbrOfChannels;
            break;

        /*------------------+
        |  ch direction     |
        +------------------*/
        case M_LL_CH_DIR:
             *valueP = M_CH_OUT;
            break;

        /*--------------------+
        |  ch length (bit)    |
        +--------------------*/
        case M_LL_CH_LEN:
             *valueP = 12;
             break;

        /*------------------+
        |  ch typ           |
        +------------------*/
        case M_LL_CH_TYP:
             *valueP = M_CH_ANALOG;
             break;

        /*------------------+
        |  id check enabled |
        +------------------*/
        case M_LL_ID_CHECK:
            *valueP = m62Hdl->useModulId;
            break;

        /*------------------+
        |  debug level       |
        +------------------*/
        case M_LL_DEBUG_LEVEL:
            *valueP = m62Hdl->dbgLevel;
            break;

        /*--------------------+
        |  id prom size       |
        +--------------------*/
        case M_LL_ID_SIZE:
            *valueP = MOD_ID_SIZE;
            break;

        /*--------------------+
        |  ident table        |
        +--------------------*/
        case M_MK_BLK_REV_ID:
           *value64P = (INT32_OR_64)&m62Hdl->idFuncTbl;
           break;

        /*--------------------+
        |  ch width (byte)    |
        +--------------------*/
        case M_BUF_WR_WIDTH:
             *valueP = 2;
             break;

        /*--------------------+
        |  buffer mode        |
        +--------------------*/
        case M_BUF_WR_MODE:
             *valueP = M_BUF_USRCTRL;
             break;

        /* ------ special getstat codes --------- */

        /*--------------------+
        |  (unknown)          |
        +--------------------*/
        default:
            if(    ( M_LL_BLK_OF <= code && code <= (M_LL_BLK_OF+0xff) )
                || ( M_DEV_BLK_OF <= code && code <= (M_DEV_BLK_OF+0xff) )
              )
                return( getStatBlock( m62Hdl, code, (M_SETGETSTAT_BLOCK*) value64P ) );

            DBGWRT_ERR( ( DBH, "*** %s:  unkown getstat code\n",
                          functionName ));
            return(ERR_LL_UNK_CODE);

    }/*switch*/
    return(0);
}/*M62_GetStat*/

/******************************* M62_BlockRead *******************************
 *
 *  Description:  Do nothing.
 *                Supported modes:  not supported
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl        pointer to low-level driver data structure
 *                ch           current channel (always ignored)
 *                buf          buffer to store read values
 *                size         must be multiple of buffer width (2)
 *                nbrRdBytesP  pointer to variable where number of read bytes
 *                             is stored
 *  Output.....:  *nbrRdBytesP number of read bytes
 *                return       always ERR_LL_ILL_FUNC
 *  Globals....:  -
 ****************************************************************************/
int32 M62_BlockRead
(
    LL_HANDLE  *llHdl,
    int32     ch,
    void      *buf,
    int32     size,
    int32     *nbrRdBytesP
)
{
    DBGCMD( static const char functionName[] = "LL - M62_BlockRead()"; )
    DBGCMD( M62_HANDLE *m62Hdl = (M62_HANDLE*) llHdl; )

    DBGWRT_1((DBH, "%s\n", functionName) );

    *nbrRdBytesP = 0;

    return( ERR_LL_ILL_FUNC );
}

/****************************** M62_BlockWrite *******************************
 *
 *  Description:  Writes all channels maximal 16. Always starts with channel 0.
 *                Supported modes:  M_BUF_USRCTRL         writes to hardware
 *
 *                buffer structure
 *
 *                   word     0    1..14    15
 *                (2 byte)  +---+- - - - -+----+
 *                channel   | 0 |  1..14  | 15 |
 *                          +---+- - - - -+----+
 *
 *                byte structure of a word
 *
 *                   bit        15   14..5    4    3..1
 *                           +-----+- - - -+-----+- - -+
 *                meaning    | msb |       | lsb |  no |
 *                           +-----+- - - -+-----+- - -+
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl  pointer to low-level driver data structure
 *                ch     current channel (always ignored)
 *                buf    buffer where output values are stored
 *                size   number of bytes to write (min. 2, max. 32)
 *                       must be multiple of buffer width (2)
 *  Output.....:  nbrWrBytesP  number of written bytes
 *                return       0 | error code
 *  Globals....:  ---
 ****************************************************************************/
int32 M62_BlockWrite
(
    LL_HANDLE *llHdl,
    int32     ch,
    void      *buf,
    int32     size,
    int32     *nbrWrBytesP
)
{
    DBGCMD( static const char functionName[] = "LL - M62_BlockWrite()"; )
    M62_HANDLE*       m62Hdl = (M62_HANDLE*) llHdl;
    int16 *bufPtr = (int16*) buf;           /* ptr to buffer */

    DBGWRT_1((DBH, "%s size=%ld\n", functionName, size) );

    *nbrWrBytesP = 0;

    if( size > M62_MAX_CH )
        size = M62_MAX_CH;

    for( ch=0; ch<size; ch++ ) {
        MWRITE_D16( m62Hdl->ma62, OUT_REG(ch), *bufPtr );
        bufPtr++;
    }

    *nbrWrBytesP = size;

    return( 0 );
}

/****************************** M62_Irq *************************************
 *
 *  Description:  Do nothing.
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl  pointer to ll-drv data structure
 *  Output.....:  return MDIS_IRQ_DEV_NOT
 *  Globals....:  ---
 ****************************************************************************/
int32 M62_Irq
(
    LL_HANDLE *llHdl
)
{
    return( LL_IRQ_DEV_NOT );
}/*M62_Irq*/

/****************************** M62_Info ************************************
 *
 *  Description:  Get information about hardware and driver requirements.
 *
 *                Following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  -----------------------------
 *                LL_INFO_HW_CHARACTER      hardware characteristics
 *                LL_INFO_ADDRSPACE_COUNT   nr of required address spaces
 *                LL_INFO_ADDRSPACE         address space information
 *                LL_INFO_IRQ               interrupt required
 *                LL_INFO_LOCKMODE          process lock mode required
 *
 *                The LL_INFO_HW_CHARACTER code returns all address and 
 *                data modes (OR'ed), which are supported from the
 *                hardware (MDIS_MAxx, MDIS_MDxx).
 *
 *                The LL_INFO_ADDRSPACE_COUNT code returns the number
 *                of address spaces used from the driver.
 *
 *                The LL_INFO_ADDRSPACE code returns information about one
 *                specific address space (MDIS_MAxx, MDIS_MDxx). The returned 
 *                data mode represents the widest hardware access used from 
 *                the driver.
 *
 *                The LL_INFO_IRQ code returns, if the driver supports an
 *                interrupt routine (TRUE or FALSE).
 *
 *                The LL_INFO_LOCKMODE code returns, which process locking
 *                mode is required from the driver (LL_LOCK_xxx).
 *
 *--------------------------------------------------------------------------
 *  Input......:  infoType	   info code
 *                ...          argument(s)
 *  Output.....:  return       success (0) or error code
 *  Globals....:  ---
 ****************************************************************************/
int32 M62_Info
(
   int32  infoType,
   ...
)
{
    int32   error;
    va_list argptr;
    u_int32 *nbrOfAddrSpaceP;
    u_int32 *addrModeP;
    u_int32 *dataModeP;
    u_int32 *addrSizeP;
    u_int32 *useIrqP;
    u_int32 addrSpaceIndex;

    error = 0;
    va_start( argptr, infoType );

    switch( infoType )
    {
        case LL_INFO_HW_CHARACTER:
          addrModeP  = va_arg( argptr, u_int32* );
          dataModeP  = va_arg( argptr, u_int32* );
          *addrModeP = MDIS_MA08;
          *dataModeP = MDIS_MD16;
          break;

        case LL_INFO_ADDRSPACE_COUNT:
          nbrOfAddrSpaceP  = va_arg( argptr, u_int32* );
          *nbrOfAddrSpaceP = 1;
          break;

        case LL_INFO_ADDRSPACE:
          addrSpaceIndex = va_arg( argptr, u_int32 );
          addrModeP  = va_arg( argptr, u_int32* );
          dataModeP  = va_arg( argptr, u_int32* );
          addrSizeP  = va_arg( argptr, u_int32* );

          switch( addrSpaceIndex )
          {
              case 0:
                *addrModeP = MDIS_MA08;
                *dataModeP = MDIS_MD16;
                *addrSizeP = 0x100;
                break;

              default:
                 error = ERR_LL_ILL_PARAM;
          }/*switch*/
          break;

        case LL_INFO_IRQ:
          useIrqP  = va_arg( argptr, u_int32* );
          *useIrqP = 0;
          break;
       
        case LL_INFO_LOCKMODE:
		{
			u_int32 *lockModeP = va_arg(argptr, u_int32*);

			*lockModeP = LL_LOCK_CALL;
			break;
	    }
	    
        default:
          error = ERR_LL_ILL_PARAM;
    }/*switch*/

    va_end( argptr );
    return( error );
}/*M62_Info*/


/************************** getStatBlock *************************************
 *
 *  Description:  Decodes the M_SETGETSTAT_BLOCK code and executes them.
 *                (see M62_GetStat)
 *
 *---------------------------------------------------------------------------
 *  Input......:  m62Hdl         m62 handle
 *                code           getstat code
 *                blockStruct    the struct with code size and data buffer
 *  Output.....:  blockStruct->data  filled data buffer
 *                return - 0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 getStatBlock	/* nodoc */
(
    M62_HANDLE         *m62Hdl,
    int32              code,
    M_SETGETSTAT_BLOCK *blockStruct
)
{
   DBGCMD( static const char functionName[] = "LL - getStatBlock"; )
   int32   error;

   DBGWRT_1((DBH, "%s\n", functionName) );

   error = 0;
   switch( code )
   {
       case M_LL_BLK_ID_DATA:
          if( !m62Hdl->useModulId )
          {
              DBGWRT_ERR( ( DBH, "*** %s:  module id disabled in descriptor\n",
                          functionName ));
			  return(ERR_ID);
		  }/*if*/
		  if (blockStruct->size < MOD_ID_SIZE)		/* check buf size */
		  {
			  return(ERR_LL_USERBUF);
		  }/*if*/

          OSS_MemCopy( m62Hdl->osHdl, MOD_ID_SIZE, (char*)m62Hdl->modIdBuf, (char*)blockStruct->data );
          break;

       default:
          DBGWRT_ERR( ( DBH, "*** %s:  unkown blockgetstat code\n",
                        functionName ));
          error = ERR_LL_UNK_CODE;
   }/*switch*/

   return( error );
}/*getStatBlock*/



