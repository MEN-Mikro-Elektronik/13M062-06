#***************************  M a k e f i l e  *******************************
#
#         Author: uf
#          $Date: 2004/08/25 13:50:46 $
#      $Revision: 1.2 $
#        $Header: /dd2/CVSR/COM/DRIVERS/MDIS_LL/M062/EXAMPLE/M62_SIMP/COM/program.mak,v 1.2 2004/08/25 13:50:46 dpfeuffer Exp $
#
#    Description: makefile descriptor file for common
#                 modules MDIS 4.x   e.g. low level driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.2  2004/08/25 13:50:46  dpfeuffer
#   minor modifications for MDIS4/2004 conformity
#
#   Revision 1.1  1998/04/14 13:29:41  Franke
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m62_simp

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)     \


MAK_INCL=$(MEN_INC_DIR)/m62_drv.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/usr_oss.h     \

MAK_INP1=m62_simp$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
