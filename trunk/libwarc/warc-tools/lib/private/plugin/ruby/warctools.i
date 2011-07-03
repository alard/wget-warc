%module warctools



%include "wclass.i"
%include "record.i"
%include "file.i"
%include "wbloc.i"
%include "wrbbless.i"
%include "typemaps.i"

%apply float *INOUT { float *modify_me };


