%module stsdef

%constant int STS_M_SEVERITY = 0x7L;
%constant int STS_M_COND_ID = 0xFFFFFF8L;
%constant int STS_M_CONTROL = 0xF0000000L;
%constant int STS_M_SUCCESS = 0x1L;
%constant int STS_M_MSG_NO = 0xFFF8L;
%constant int STS_M_CODE = 0x7FF8L;
%constant int STS_M_FAC_SP = 0x8000L;
%constant int STS_M_CUST_DEF = 0x8000000L;
%constant int STS_M_INHIB_MSG = 0x10000000L;
%constant int STS_M_FAC_NO = 0xFFF0000L;
%constant int STS_K_WARNING = 0;
%constant int STS_K_SUCCESS = 1;
%constant int STS_K_ERROR = 2;
%constant int STS_K_INFO = 3;
%constant int STS_K_SEVERE = 4;
%constant int STS_S_STSDEF = 4;
%constant int STS_S_SEVERITY = 3;
%constant int STS_S_COND_ID = 25;
%constant int STS_S_CONTROL = 4;
%constant int STS_S_MSG_NO = 13;
%constant int STS_S_CODE = 12;
%constant int STS_S_FAC_NO = 12;
