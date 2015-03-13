#***************************  M a k e f i l e  *******************************
#
#         Author: uf
#          $Date: 2004/08/25 13:50:30 $
#      $Revision: 1.5 $
#
#    Description: Makefile definitions for the M62 driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.5  2004/08/25 13:50:30  dpfeuffer
#   minor modifications for MDIS4/2004 conformity
#
#   Revision 1.4  2002/03/21 16:26:33  Schmidt
#   modcom.h replaced with microwire.h, MAK_SWITCH added
#
#   Revision 1.3  1998/07/15 17:42:35  Franke
#   added dbg lib for MDIS 4.1
#
#   Revision 1.2  1998/03/09 13:56:39  Schmidt
#   use id.lib - not modcom.lib
#
#   Revision 1.1  1998/02/19 17:17:01  franke
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m62

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id$(LIB_SUFFIX)       \
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

