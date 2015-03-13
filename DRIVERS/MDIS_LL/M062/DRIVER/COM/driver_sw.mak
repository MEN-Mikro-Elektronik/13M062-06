#***************************  M a k e f i l e  *******************************
#
#         Author: uf
#          $Date: 2004/08/25 13:50:32 $
#      $Revision: 1.2 $
#        $Header: /dd2/CVSR/COM/DRIVERS/MDIS_LL/M062/DRIVER/COM/driver_sw.mak,v 1.2 2004/08/25 13:50:32 dpfeuffer Exp $
#
#    Description: makefile descriptor file for common
#                 modules MDIS 4.0   e.g. low level driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_sw.mak,v $
#   Revision 1.2  2004/08/25 13:50:32  dpfeuffer
#   minor modifications for MDIS4/2004 conformity
#
#   Revision 1.1  2002/04/29 09:48:22  Franke
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2002 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m62_sw

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
		   $(SW_PREFIX)MAC_BYTESWAP   \
           $(SW_PREFIX)ID_SW
           
MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id_sw$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)      \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)


MAK_INCL=$(MEN_INC_DIR)/m62_drv.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/maccess.h     \
         $(MEN_INC_DIR)/desc.h        \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_com.h    \
         $(MEN_INC_DIR)/microwire.h   \
         $(MEN_INC_DIR)/ll_defs.h     \
         $(MEN_INC_DIR)/ll_entry.h    \
         $(MEN_INC_DIR)/dbg.h         \

MAK_INP1=m62_drv$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

