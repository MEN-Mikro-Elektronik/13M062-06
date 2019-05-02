#***************************  M a k e f i l e  *******************************
#
#         Author: uf
#          $Date: 2004/08/25 13:50:52 $
#      $Revision: 1.2 $
#        $Header: /mnt/swserver-disc/CVSR/COM/DRIVERS/MDIS_LL/M062/TEST/M62_MAIN/COM/program.mak,v 1.2 2004/08/25 13:50:52 dpfeuffer Exp $
#
#    Description: makefile descriptor file for common
#                 modules MDIS 4.x   e.g. low level driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.2  2004/08/25 13:50:52  dpfeuffer
#   minor modifications for MDIS4/2004 conformity
#
#   Revision 1.1  1998/02/19 17:17:19  franke
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m62_main

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)     \


MAK_INCL=$(MEN_INC_DIR)/m62_drv.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/usr_oss.h     \

MAK_INP1=m62_main$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
