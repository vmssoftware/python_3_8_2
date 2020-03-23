%module dmtdef

%constant int DMT_M_NOUNLOAD = 0x1L;
%constant int DMT_M_UNIT = 0x2L;
%constant int DMT_M_ABORT = 0x4L;
%constant int DMT_M_CLUSTER = 0x8L;
%constant int DMT_M_UNLOAD = 0x10L;
%constant int DMT_M_OVR_CHECKS = 0x20L;
%constant int DMT_M_CHECK_ONLY = 0x40L;
%constant int DMT_M_REMOTE = 0x80L;
%constant int DMT_M_MINICOPY_REQUIRED = 0x100L;
%constant int DMT_M_MINICOPY_OPTIONAL = 0x200L;
%constant int DMT_M_FORCE = 0x400L;
%constant int DMT_S_DMTDEF = 2;
