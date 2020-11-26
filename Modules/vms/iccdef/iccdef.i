%module iccdef

%constant int IOS_ICC_S_IOS_ICC = 20;
%constant int IOS_ICC_s_parameters = 16;
%constant int IOS_ICC_s_connect = 16;
%constant int IOS_ICC_s_receive = 16;
%constant int IOS_ICC_s_reply = 16;
%constant int IOS_ICC_s_transceive = 16;
%constant int IOS_ICC_s_reply_buffer = 8;
%constant int ICC_C_receive_len = 16;
%constant int ICC_C_reply_len = 8;
%constant int ICC_C_transceive_len = 20;
%constant int ICC_C_MAX_ASSOC_LEN = 31;
%constant int ICC_C_MAX_CONN_DATA_LEN = 1000;
%constant int ICC_M_Synch_Mode = 0x1L;
%constant int ICC_S_ICC_CONNECTION_FLAGS = 1;
%constant int ICC_C_EV_CONNECT = 0;
%constant int ICC_C_EV_DISCONNECT = 1;
%constant int ICC_C_min_event = 0;
%constant int ICC_C_max_event = 1;
%constant int ICC_C_DFLT_ASSOC_HANDLE = 1;
%constant int ICC_C_DEFAULT_MAXFLOWBUFCNT = 5;
