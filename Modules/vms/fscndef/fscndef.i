%module fscndef

%constant int FSCN_M_NODE = 0x1L;
%constant int FSCN_M_DEVICE = 0x2L;
%constant int FSCN_M_ROOT = 0x4L;
%constant int FSCN_M_DIRECTORY = 0x8L;
%constant int FSCN_M_NAME = 0x10L;
%constant int FSCN_M_TYPE = 0x20L;
%constant int FSCN_M_VERSION = 0x40L;
%constant int FSCN_M_NODE_PRIMARY = 0x80L;
%constant int FSCN_M_NODE_ACS = 0x100L;
%constant int FSCN_M_NODE_SECONDARY = 0x200L;
%constant int FSCN_S_FLDFLAGS = 4;
%constant int FSCN__FILESPEC = 1;
%constant int FSCN__NODE = 2;
%constant int FSCN__DEVICE = 3;
%constant int FSCN__ROOT = 4;
%constant int FSCN__DIRECTORY = 5;
%constant int FSCN__NAME = 6;
%constant int FSCN__TYPE = 7;
%constant int FSCN__VERSION = 8;
%constant int FSCN__NODE_PRIMARY = 9;
%constant int FSCN__NODE_ACS = 10;
%constant int FSCN__NODE_SECONDARY = 11;
%constant int FSCN_S_ITEM_LEN = 8;
%constant int FSCN_S_FSCNDEF = 8;
