%module rabdef

%constant int RAB_C_BID = 1;
%constant int RAB_M_PPF_RAT = 0x3FC0L;
%constant int RAB_M_PPF_IND = 0x4000L;
%constant int RAB_M_PPISI = 0x8000L;
%constant int RAB_M_ASY = 0x1L;
%constant int RAB_M_TPT = 0x2L;
%constant int RAB_M_REA = 0x4L;
%constant int RAB_M_RRL = 0x8L;
%constant int RAB_M_UIF = 0x10L;
%constant int RAB_M_MAS = 0x20L;
%constant int RAB_M_FDL = 0x40L;
%constant int RAB_M_REV = 0x80L;
%constant int RAB_M_EOF = 0x100L;
%constant int RAB_M_RAH = 0x200L;
%constant int RAB_M_WBH = 0x400L;
%constant int RAB_M_BIO = 0x800L;
%constant int RAB_M_CDK = 0x1000L;
%constant int RAB_M_LOA = 0x2000L;
%constant int RAB_M_LIM = 0x4000L;
%constant int RAB_M_SYNCSTS = 0x8000L;
%constant int RAB_M_LOC = 0x10000L;
%constant int RAB_M_WAT = 0x20000L;
%constant int RAB_M_ULK = 0x40000L;
%constant int RAB_M_RLK = 0x80000L;
%constant int RAB_M_NLK = 0x100000L;
%constant int RAB_M_KGE = 0x200000L;
%constant int RAB_M_KGT = 0x400000L;
%constant int RAB_M_NXR = 0x800000L;
%constant int RAB_M_RNE = 0x1000000L;
%constant int RAB_M_TMO = 0x2000000L;
%constant int RAB_M_CVT = 0x4000000L;
%constant int RAB_M_RNF = 0x8000000L;
%constant int RAB_M_ETO = 0x10000000L;
%constant int RAB_M_PTA = 0x20000000L;
%constant int RAB_M_PMT = 0x40000000L;
%constant int RAB_M_CCO = 0x80000000L;
%constant int RAB_M_EQNXT = 0x200000L;
%constant int RAB_M_NXT = 0x400000L;
%constant int RAB_M_NQL = 0x1L;
%constant int RAB_M_NODLCKWT = 0x2L;
%constant int RAB_M_NODLCKBLK = 0x4L;
%constant int RAB_C_SEQ = 0;
%constant int RAB_C_KEY = 1;
%constant int RAB_C_RFA = 2;
%constant int RAB_C_STM = 3;
%constant int RAB_C_MAXRAC = 2;
%constant int RAB_S_RABDEF = 68;
%constant int RAB_S_PPF_RAT = 8;
%constant int RAB_S_RFA = 6;
%constant int RAB_K_BLN = 68;
%constant int RAB_C_BLN = 68;
%constant int RAB64_C_BID = 1;
%constant int RAB64_M_PPF_RAT = 0x3FC0L;
%constant int RAB64_M_PPF_IND = 0x4000L;
%constant int RAB64_M_PPISI = 0x8000L;
%constant int RAB64_M_ASY = 0x1L;
%constant int RAB64_M_TPT = 0x2L;
%constant int RAB64_M_REA = 0x4L;
%constant int RAB64_M_RRL = 0x8L;
%constant int RAB64_M_UIF = 0x10L;
%constant int RAB64_M_MAS = 0x20L;
%constant int RAB64_M_FDL = 0x40L;
%constant int RAB64_M_REV = 0x80L;
%constant int RAB64_M_EOF = 0x100L;
%constant int RAB64_M_RAH = 0x200L;
%constant int RAB64_M_WBH = 0x400L;
%constant int RAB64_M_BIO = 0x800L;
%constant int RAB64_M_CDK = 0x1000L;
%constant int RAB64_M_LOA = 0x2000L;
%constant int RAB64_M_LIM = 0x4000L;
%constant int RAB64_M_SYNCSTS = 0x8000L;
%constant int RAB64_M_LOC = 0x10000L;
%constant int RAB64_M_WAT = 0x20000L;
%constant int RAB64_M_ULK = 0x40000L;
%constant int RAB64_M_RLK = 0x80000L;
%constant int RAB64_M_NLK = 0x100000L;
%constant int RAB64_M_KGE = 0x200000L;
%constant int RAB64_M_KGT = 0x400000L;
%constant int RAB64_M_NXR = 0x800000L;
%constant int RAB64_M_RNE = 0x1000000L;
%constant int RAB64_M_TMO = 0x2000000L;
%constant int RAB64_M_CVT = 0x4000000L;
%constant int RAB64_M_RNF = 0x8000000L;
%constant int RAB64_M_ETO = 0x10000000L;
%constant int RAB64_M_PTA = 0x20000000L;
%constant int RAB64_M_PMT = 0x40000000L;
%constant int RAB64_M_CCO = 0x80000000L;
%constant int RAB64_M_EQNXT = 0x200000L;
%constant int RAB64_M_NXT = 0x400000L;
%constant int RAB64_M_NQL = 0x1L;
%constant int RAB64_M_NODLCKWT = 0x2L;
%constant int RAB64_M_NODLCKBLK = 0x4L;
%constant int RAB64_C_SEQ = 0;
%constant int RAB64_C_KEY = 1;
%constant int RAB64_C_RFA = 2;
%constant int RAB64_C_STM = 3;
%constant int RAB64_C_MAXRAC = 2;
%constant int RAB64_M_RESERVED29 = 0x20000000L;
%constant int RAB64_M_RESERVED30 = 0x40000000L;
%constant int RAB64_M_RESERVED31 = 0x80000000L;
%constant int RAB_M_RESERVED29 = 536870912;
%constant int RAB_M_RESERVED30 = 1073741824;
%constant int RAB_M_RESERVED31 = -2147483648;
%constant int RAB64_S_RAB64DEF = 144;
%constant int RAB64_S_PPF_RAT = 8;
%constant int RAB64_S_RFA = 6;
%constant int RAB64_S_UBF = 8;
%constant int RAB_S_UBF = 8;
%constant int RAB64_S_USZ = 8;
%constant int RAB_S_USZ = 8;
%constant int RAB64_S_RBF = 8;
%constant int RAB_S_RBF = 8;
%constant int RAB64_S_RSZ = 8;
%constant int RAB_S_RSZ = 8;
%constant int RAB64_S_KBF = 8;
%constant int RAB_S_KBF = 8;
%constant int RAB64_S_RHB = 8;
%constant int RAB_S_RHB = 8;
%constant int RAB64_S_CTX = 8;
%constant int RAB_S_CTX = 8;
%constant int RAB64_K_BLN64 = 144;
%constant int RAB64_C_BLN64 = 144;
%constant int RAB_K_BLN64 = 144;
%constant int RAB_C_BLN64 = 144;
%constant int RAB_K_MAXBLN = 144;
%constant int RAB_C_MAXBLN = 144;
