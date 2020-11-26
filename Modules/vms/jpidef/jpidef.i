%module jpidef

%constant int JPI_K_DEFAULT_POLICY = 0;
%constant int JPI_K_PSX_FIFO_POLICY = 1;
%constant int JPI_K_PSX_RR_POLICY = 2;
%constant int JPI_K_ALL_THREADS = -2147483648;
%constant int JPI_C_ADRTYPE = 1;
%constant int JPI_C_CTLTYPE = 2;
%constant int JPI_C_PCBTYPE = 3;
%constant int JPI_C_PHDTYPE = 4;
%constant int JPI_C_PCBFLDTYPE = 5;
%constant int JPI_C_PHDFLDTYPE = 6;
%constant int JPI_C_JIBTYPE = 7;
%constant int JPI_C_PSBTYPE = 8;
%constant int JPI_C_KTBTYPE = 9;
%constant int JPI_C_MAXSTRUC = 6;
%constant int JPI_C_LISTEND = 0;
%constant int JPI__CHAIN = -1;
%constant int JPI__GETJPI_CONTROL_FLAGS = -2;
%constant int JPI_M_NO_TARGET_INSWAP = 0x1L;
%constant int JPI_M_NO_TARGET_AST = 0x2L;
%constant int JPI_M_IGNORE_TARGET_STATUS = 0x4L;
%constant int JPI_M_THREAD = 0x8L;
%constant int JPI_M_NATURAL_PERSONA = 0x10L;
%constant int JPI_M_FILL1 = 0x7FE0L;
%constant int JPI_M_NEED_THREAD_CAP = 0x8000L;
%constant int JPI_S_JPICTLDEF = 4;
%constant int JPI_S_FILL1 = 10;
%constant int JPI_K_OTHER = 0;
%constant int JPI_K_NETWORK = 1;
%constant int JPI_K_BATCH = 2;
%constant int JPI_K_INTERACTIVE = 3;
%constant int JPI_K_DETACHED = 0;
%constant int JPI_K_LOCAL = 3;
%constant int JPI_K_DIALUP = 4;
%constant int JPI_K_REMOTE = 5;
%constant int JPI_M_NEW_MAIL_AT_LOGIN = 0x1L;
%constant int JPI_M_PASSWORD_CHANGED = 0x2L;
%constant int JPI_M_PASSWORD_EXPIRED = 0x4L;
%constant int JPI_M_PASSWORD_WARNING = 0x8L;
%constant int JPI_M_PASSWORD2_CHANGED = 0x10L;
%constant int JPI_M_PASSWORD2_EXPIRED = 0x20L;
%constant int JPI_M_PASSWORD2_WARNING = 0x40L;
%constant int JPI_S_JPILGIDEF = 4;
%constant int JPI_K_SEARCH_SYMLINK_NONE = 1;
%constant int JPI_K_SEARCH_SYMLINK_ALL = 2;
%constant int JPI_K_SEARCH_SYMLINK_NOELLIPS = 3;
%constant int JPI__ASTACT = 768;
%constant int JPI__ASTEN = 769;
%constant int JPI__PRI = 770;
%constant int JPI__OWNER = 771;
%constant int JPI__UIC = 772;
%constant int JPI__STS = 773;
%constant int JPI__STATE = 774;
%constant int JPI__MEM = 775;
%constant int JPI__GRP = 776;
%constant int JPI__PRIB = 777;
%constant int JPI__APTCNT = 778;
%constant int JPI__TMBU = 779;
%constant int JPI__GPGCNT = 780;
%constant int JPI__PPGCNT = 781;
%constant int JPI__ASTCNT = 782;
%constant int JPI__BIOCNT = 783;
%constant int JPI__BIOLM = 784;
%constant int JPI__BYTCNT = 785;
%constant int JPI__DIOCNT = 786;
%constant int JPI__DIOLM = 787;
%constant int JPI__FILCNT = 788;
%constant int JPI__TQCNT = 789;
%constant int JPI__EFWM = 790;
%constant int JPI__EFCS = 791;
%constant int JPI__EFCU = 792;
%constant int JPI__PID = 793;
%constant int JPI__BYTLM = 794;
%constant int JPI__PRCCNT = 795;
%constant int JPI__PRCNAM = 796;
%constant int JPI__TERMINAL = 797;
%constant int JPI__JOBPRCCNT = 798;
%constant int JPI__ENQCNT = 799;
%constant int JPI__ENQLM = 800;
%constant int JPI__SWPFILLOC = 801;
%constant int JPI__MODE = 802;
%constant int JPI__JOBTYPE = 803;
%constant int JPI__PROC_INDEX = 804;
%constant int JPI__MASTER_PID = 805;
%constant int JPI__RIGHTSLIST = 806;
%constant int JPI__CPU_ID = 807;
%constant int JPI__STS2 = 808;
%constant int JPI__NODENAME = 809;
%constant int JPI__NODE_CSID = 810;
%constant int JPI__NODE_VERSION = 811;
%constant int JPI__TT_PHYDEVNAM = 812;
%constant int JPI__TT_ACCPORNAM = 813;
%constant int JPI__PROCESS_RIGHTS = 814;
%constant int JPI__SYSTEM_RIGHTS = 815;
%constant int JPI__IMAGE_RIGHTS = 816;
%constant int JPI__RIGHTS_SIZE = 817;
%constant int JPI__CLASSIFICATION = 818;
%constant int JPI__SCHED_POLICY = 819;
%constant int JPI__RMS_FILEPROT = 820;
%constant int JPI__MULTITHREAD = 821;
%constant int JPI__KT_COUNT = 822;
%constant int JPI__INITIAL_THREAD_PID = 823;
%constant int JPI__THREAD_INDEX = 824;
%constant int JPI__CURRENT_USERCAP_MASK = 825;
%constant int JPI__PERMANENT_USERCAP_MASK = 826;
%constant int JPI__CURRENT_AFFINITY_MASK = 827;
%constant int JPI__PERMANENT_AFFINITY_MASK = 828;
%constant int JPI__PERSONA_ID = 829;
%constant int JPI__PERSONA_AUTHPRIV = 830;
%constant int JPI__PERSONA_PERMPRIV = 831;
%constant int JPI__PERSONA_WORKPRIV = 832;
%constant int JPI__IMAGE_WORKPRIV = 833;
%constant int JPI__PERSONA_RIGHTS_SIZE = 834;
%constant int JPI__PERSONA_RIGHTS = 835;
%constant int JPI__SUBSYSTEM_RIGHTS_SIZE = 836;
%constant int JPI__SUBSYSTEM_RIGHTS = 837;
%constant int JPI__INSTALL_RIGHTS_SIZE = 838;
%constant int JPI__INSTALL_RIGHTS = 839;
%constant int JPI__SYSTEM_RIGHTS_SIZE = 840;
%constant int JPI__CURRENT_CAP_MASK = 841;
%constant int JPI__PERMANENT_CAP_MASK = 842;
%constant int JPI__SCHED_CLASS_NAME = 843;
%constant int JPI__HOME_RAD = 844;
%constant int JPI__POSIX_SID = 845;
%constant int JPI__POSIX_PGID = 846;
%constant int JPI__CREATOR = 847;
%constant int JPI__KRNLTIM = 848;
%constant int JPI__EXECTIM = 849;
%constant int JPI__SUPRTIM = 850;
%constant int JPI__USERTIM = 851;
%constant int JPI__ORG_BYTLM = 852;
%constant int JPI__DEADLOCK_WAIT = 853;
%constant int JPI__KT_LIMIT = 854;
%constant int JPI__LASTPCB = 855;
%constant int JPI__CURPRIV = 1024;
%constant int JPI__WSAUTH = 1025;
%constant int JPI__WSQUOTA = 1026;
%constant int JPI__DFWSCNT = 1027;
%constant int JPI__FREP0VA = 1028;
%constant int JPI__FREP1VA = 1029;
%constant int JPI__DFPFC = 1030;
%constant int JPI__CPUTIM = 1031;
%constant int JPI__PRCLM = 1032;
%constant int JPI__ASTLM = 1033;
%constant int JPI__PAGEFLTS = 1034;
%constant int JPI__DIRIO = 1035;
%constant int JPI__BUFIO = 1036;
%constant int JPI__CPULIM = 1037;
%constant int JPI__PGFLQUOTA = 1038;
%constant int JPI__FILLM = 1039;
%constant int JPI__TQLM = 1040;
%constant int JPI__WSSIZE = 1041;
%constant int JPI__AUTHPRIV = 1042;
%constant int JPI__IMAGPRIV = 1043;
%constant int JPI__PAGFILCNT = 1044;
%constant int JPI__FREPTECNT = 1045;
%constant int JPI__WSEXTENT = 1046;
%constant int JPI__WSAUTHEXT = 1047;
%constant int JPI__AUTHPRI = 1048;
%constant int JPI__PAGFILLOC = 1049;
%constant int JPI__IMAGECOUNT = 1050;
%constant int JPI__PHDFLAGS = 1051;
%constant int JPI__VP_CPUTIM = 1052;
%constant int JPI__VP_CONSUMER = 1053;
%constant int JPI__P0_FIRST_FREE_VA_64 = 1054;
%constant int JPI__P1_FIRST_FREE_VA_64 = 1055;
%constant int JPI__P2_FIRST_FREE_VA_64 = 1056;
%constant int JPI__IMAGE_AUTHPRIV = 1057;
%constant int JPI__IMAGE_PERMPRIV = 1058;
%constant int JPI__LASTPHD = 1059;
%constant int JPI__VIRTPEAK = 512;
%constant int JPI__WSPEAK = 513;
%constant int JPI__USERNAME = 514;
%constant int JPI__ACCOUNT = 515;
%constant int JPI__PROCPRIV = 516;
%constant int JPI__VOLUMES = 517;
%constant int JPI__LOGINTIM = 518;
%constant int JPI__IMAGNAME = 519;
%constant int JPI__SITESPEC = 520;
%constant int JPI__MSGMASK = 521;
%constant int JPI__CLINAME = 522;
%constant int JPI__TABLENAME = 523;
%constant int JPI__CREPRC_FLAGS = 524;
%constant int JPI__UAF_FLAGS = 525;
%constant int JPI__MAXDETACH = 526;
%constant int JPI__MAXJOBS = 527;
%constant int JPI__SHRFILLM = 528;
%constant int JPI__FAST_VP_SWITCH = 529;
%constant int JPI__SLOW_VP_SWITCH = 530;
%constant int JPI__LAST_LOGIN_I = 531;
%constant int JPI__LAST_LOGIN_N = 532;
%constant int JPI__LOGIN_FAILURES = 533;
%constant int JPI__LOGIN_FLAGS = 534;
%constant int JPI__RMS_DFMBC = 535;
%constant int JPI__RMS_DFMBFSDK = 536;
%constant int JPI__RMS_DFMBFSMT = 537;
%constant int JPI__RMS_DFMBFSUR = 538;
%constant int JPI__RMS_DFMBFREL = 539;
%constant int JPI__RMS_DFMBFIDX = 540;
%constant int JPI__RMS_PROLOGUE = 541;
%constant int JPI__RMS_EXTEND_SIZE = 542;
%constant int JPI__RMS_DFNBC = 543;
%constant int JPI__RMS_DFLRL = 544;
%constant int JPI__RMS_HEURISTIC = 545;
%constant int JPI__PARSE_STYLE_PERM = 546;
%constant int JPI__PARSE_STYLE_IMAGE = 547;
%constant int JPI__RMSD6 = 548;
%constant int JPI__RMS_QUERY_LOCK = 549;
%constant int JPI__CASE_LOOKUP_PERM = 550;
%constant int JPI__CASE_LOOKUP_IMAGE = 551;
%constant int JPI__UNITS = 552;
%constant int JPI__TOKEN = 553;
%constant int JPI__SEARCH_SYMLINK_PERM = 554;
%constant int JPI__SEARCH_SYMLINK_TEMP = 555;
%constant int JPI__LASTCTL = 556;
%constant int JPI__EXCVEC = 256;
%constant int JPI__FINALEXC = 257;
%constant int JPI__LASTADR = 258;
%constant int JPI__LASTPCBFLD = 1280;
%constant int JPI__LASTPHDFLD = 1536;
