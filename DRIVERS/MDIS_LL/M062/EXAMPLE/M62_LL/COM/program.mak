#***************************  M a k e f i l e  *******************************
#
#         Author: uf
#          $Date: 1998/04/14 13:29:53 $
#      $Revision: 1.1 $
#        $Header: /dd2/CVSR/COM/DRIVERS/MDIS_LL/M062/EXAMPLE/M62_LL/COM/program.mak,v 1.1 1998/04/14 13:29:53 Franke Exp $
#
#    Description: makefile descriptor file for common
#                 modules MDIS 4.x   e.g. low level driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  1998/04/14 13:29:53  Franke
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m62_ll

MAK_LIBS= \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)     \


MAK_INCL=$(MEN_INC_DIR)/m62_drv.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/usr_oss.h     \


MAK_OPTIM=$(OPT_1)

MAK_INP1=m62_ll$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
