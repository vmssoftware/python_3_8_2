%module armdef

%constant int ARM_M_READ = 0x1L;
%constant int ARM_M_WRITE = 0x2L;
%constant int ARM_M_EXECUTE = 0x4L;
%constant int ARM_M_DELETE = 0x8L;
%constant int ARM_M_CONTROL = 0x10L;
%constant int ARM_M_CREATE = 0x4L;
%constant int ARM_M_LOCK = 0x4L;
%constant int ARM_M_PHYSICAL = 0x4L;
%constant int ARM_M_LOGICAL = 0x8L;
%constant int ARM_M_ASSOCIATE = 0x1L;
%constant int ARM_M_SUBMIT = 0x2L;
%constant int ARM_M_MANAGE = 0x4L;
%constant int ARM_S_ARMDEF = 4;
%constant int ARM_S_FILL = 27;
