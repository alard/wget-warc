
/* ------------------------------------------------------------------- */
/* Copyright (c) 2007-2008 Hanzo Archives Limited.                     */
/*                                                                     */
/* Licensed under the Apache License, Version 2.0 (the "License");     */
/* you may not use this file except in compliance with the License.    */
/* You may obtain a copy of the License at                             */
/*                                                                     */
/*     http://www.apache.org/licenses/LICENSE-2.0                      */
/*                                                                     */
/* Unless required by applicable law or agreed to in writing, software */
/* distributed under the License is distributed on an "AS IS" BASIS,   */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or     */
/* implied.                                                            */
/* See the License for the specific language governing permissions and */
/* limitations under the License.                                      */
/*                                                                     */
/* You may find more information about Hanzo Archives at               */
/*                                                                     */
/*     http://www.hanzoarchives.com/                                   */
/*                                                                     */
/* You may find more information about the WARC Tools project at       */
/*                                                                     */
/*     http://code.google.com/p/warc-tools/                            */
/* ------------------------------------------------------------------- */

#ifndef	__WARC_WPORT_H__
#define	__WARC_WPORT_H__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif

    /*
     * Portability insurance.
     */

    /*
    ** ----------------------------------------------------------------------------
    ** Include <limits.h> optionally
    ** ----------------------------------------------------------------------------
    */
#ifdef WARC_USE_LIMITS_H
#  include <limits.h>
#endif

    /*
    ** ----------------------------------------------------------------------------
    ** Determine compilation environment
    ** ----------------------------------------------------------------------------
    */
#if defined __ECC || defined __ICC || defined __INTEL_COMPILER
#  define WARC_COMPILER_STRING "Intel C/C++"
#  define WARC_COMPILER_INTEL 1
#endif

#if ( defined __host_mips || defined __sgi ) && !defined __GNUC__
#  define WARC_COMPILER_STRING    "MIPSpro C/C++"
#  define WARC_COMPILER_MIPSPRO 1
#endif

#if defined __hpux && !defined __GNUC__
#  define WARC_COMPILER_STRING "HP-UX CC"
#  define WARC_COMPILER_HPCC 1
#endif

#if defined __GNUC__
#  define WARC_COMPILER_STRING "Gnu GCC"
#  define WARC_COMPILER_GCC 1
#endif

#if defined __APPLE_CC__
    /* we don't define the compiler string here, let it be GNU */
#  define WARC_COMPILER_APPLECC 1
#endif

#if defined __IBMC__ || defined __IBMCPP__
#  define WARC_COMPILER_STRING "IBM C/C++"
#  define WARC_COMPILER_IBM 1
#endif

#if defined _MSC_VER
#  define WARC_COMPILER_STRING "Microsoft Visual C++"
#  define WARC_COMPILER_MSVC 1
#endif

#if defined __SUNPRO_C
#  define WARC_COMPILER_STRING "Sun Pro"
#  define WARC_COMPILER_SUN 1
#endif

#if defined __BORLANDC__
#  define WARC_COMPILER_STRING "Borland C/C++"
#  define WARC_COMPILER_BORLAND 1
#endif

#if defined __MWERKS__
#  define WARC_COMPILER_STRING     "MetroWerks CodeWarrior"
#  define WARC_COMPILER_METROWERKS 1
#endif

#if defined __DECC || defined __DECCXX
#  define WARC_COMPILER_STRING "Compaq/DEC C/C++"
#  define WARC_COMPILER_DEC 1
#endif

#if defined __WATCOMC__
#  define WARC_COMPILER_STRING "Watcom C/C++"
#  define WARC_COMPILER_WATCOM 1
#endif

#if !defined WARC_COMPILER_STRING
#  define WARC_COMPILER_STRING "Unknown compiler"
#endif

    /*
    ** ----------------------------------------------------------------------------
    ** Determine target operating system
    ** ----------------------------------------------------------------------------
    */
#if defined linux || defined __linux__
#  define WARC_OS_LINUX 1
#  define WARC_OS_STRING "Linux"
#endif

#if defined __CYGWIN32__
#  define WARC_OS_CYGWIN32 1
#  define WARC_OS_STRING "Cygwin"
#endif

#if defined GEKKO
#  define WARC_OS_GAMECUBE
#  define __powerpc__
#  define WARC_OS_STRING "GameCube"
#endif

#if defined __MINGW32__
#  define WARC_OS_MINGW 1
#  define WARC_OS_STRING "MinGW"
#endif

#if defined GO32 && defined DJGPP && defined __MSDOS__
#  define WARC_OS_GO32 1
#  define WARC_OS_STRING "GO32/MS-DOS"
#endif

    /* NOTE: make sure you use /bt=DOS if compiling for 32-bit DOS,
       otherwise Watcom assumes host=target */
#if defined __WATCOMC__  && defined __386__ && defined __DOS__
#  define WARC_OS_DOS32 1
#  define WARC_OS_STRING "DOS/32-bit"
#endif

#if defined _UNICOS
#  define WARC_OS_UNICOS 1
#  define WARC_OS_STRING "UNICOS"
#endif

#if ( defined __MWERKS__ && defined __powerc && !defined macintosh ) || defined __APPLE_CC__ || defined macosx
#  define WARC_OS_OSX 1
#  define WARC_OS_STRING "MacOS X"
#endif

#if defined __sun__ || defined sun || defined __sun || defined __solaris__
#  if defined __SVR4 || defined __svr4__ || defined __solaris__
#     define WARC_OS_STRING "Solaris"
#     define WARC_OS_SOLARIS 1
#  endif
#  if !defined WARC_OS_STRING
#     define WARC_OS_STRING "SunOS"
#     define WARC_OS_SUNOS 1
#  endif
#endif

#if defined __sgi__ || defined sgi || defined __sgi
#  define WARC_OS_IRIX 1
#  define WARC_OS_STRING "Irix"
#endif

#if defined __hpux__ || defined __hpux
#  define WARC_OS_HPUX 1
#  define WARC_OS_STRING "HP-UX"
#endif

#if defined _AIX
#  define WARC_OS_AIX 1
#  define WARC_OS_STRING "AIX"
#endif

#if ( defined __alpha && defined __osf__ )
#  define WARC_OS_TRU64 1
#  define WARC_OS_STRING "Tru64"
#endif

#if defined __BEOS__ || defined __beos__
#  define WARC_OS_BEOS 1
#  define WARC_OS_STRING "BeOS"
#endif

#if defined amiga || defined amigados || defined AMIGA || defined _AMIGA
#  define WARC_OS_AMIGA 1
#  define WARC_OS_STRING "Amiga"
#endif

#if defined __unix__
#  define WARC_OS_UNIX 1
#  if !defined WARC_OS_STRING
#     define WARC_OS_STRING "Unix-like(generic)"
#  endif
#endif

#if defined _WIN32_WCE
#  define WARC_OS_WINCE 1
#  define WARC_OS_STRING "Windows CE"
#endif

#if defined _XBOX
#  define WARC_OS_XBOX 1
#  define WARC_OS_STRING "XBOX"
#endif

#if defined _WIN32 || defined WIN32 || defined __NT__ || defined __WIN32__
#  define WARC_OS_WIN32 1
#  if !defined WARC_OS_XBOX
#     if defined _WIN64
#        define WARC_OS_WIN64 1
#        define WARC_OS_STRING "Win64"
#     else
#        if !defined WARC_OS_STRING
#           define WARC_OS_STRING "Win32"
#        endif
#     endif
#  endif
#endif

#if defined __palmos__
#  define WARC_OS_PALM 1
#  define WARC_OS_STRING "PalmOS"
#endif

#if defined THINK_C || defined macintosh
#  define WARC_OS_MACOS 1
#  define WARC_OS_STRING "MacOS"
#endif

    /*
    ** -----------------------------------------------------------------------------
    ** Determine target CPU
    ** -----------------------------------------------------------------------------
    */

#if defined GEKKO
#  define WARC_CPU_PPC750 1
#  define WARC_CPU_STRING "IBM PowerPC 750 (NGC)"
#endif

#if defined mc68000 || defined m68k || defined __MC68K__ || defined m68000
#  define WARC_CPU_68K 1
#  define WARC_CPU_STRING "MC68000"
#endif

#if defined __PPC__ || defined __POWERPC__  || defined powerpc || defined _POWER || defined __ppc__ || defined __powerpc__
#  define WARC_CPU_PPC 1
#  if !defined WARC_CPU_STRING
#    if defined __powerpc64__
#       define WARC_CPU_STRING "PowerPC64"
#    else
#       define WARC_CPU_STRING "PowerPC"
#    endif
#  endif
#endif

#if defined _CRAYT3E || defined _CRAYMPP
#  define WARC_CPU_CRAYT3E 1 /* target processor is a DEC Alpha 21164 used in a Cray T3E*/
#  define WARC_CPU_STRING "Cray T3E (Alpha 21164)"
#endif

#if defined CRAY || defined _CRAY && !defined _CRAYT3E
#  error Non-AXP Cray systems not supported
#endif

#if defined _SH3
#  define WARC_CPU_SH3 1
#  define WARC_CPU_STRING "Hitachi SH-3"
#endif

#if defined __sh4__ || defined __SH4__
#  define WARC_CPU_SH3 1
#  define WARC_CPU_SH4 1
#  define WARC_CPU_STRING "Hitachi SH-4"
#endif

#if defined __sparc__ || defined __sparc
#  if defined __arch64__ || defined __sparcv9 || defined __sparc_v9__
#     define WARC_CPU_SPARC64 1
#     define WARC_CPU_STRING "Sparc/64"
#  else
#     define WARC_CPU_STRING "Sparc/32"
#  endif
#  define WARC_CPU_SPARC 1
#endif

#if defined ARM || defined __arm__ || defined _ARM
#  define WARC_CPU_STRONGARM 1
#  define WARC_CPU_STRING "ARM"
#endif

#if defined mips || defined __mips__ || defined __MIPS__ || defined _MIPS
#  define WARC_CPU_MIPS 1
#  if defined _R5900
#    define WARC_CPU_STRING "MIPS R5900 (PS2)"
#  else
#    define WARC_CPU_STRING "MIPS"
#  endif
#endif

#if defined __ia64 || defined _M_IA64 || defined __ia64__
#  define WARC_CPU_IA64 1
#  define WARC_CPU_STRING "IA64"
#endif

#if defined __X86__ || defined __i386__ || defined i386 || defined _M_IX86 || defined __386__ || defined __x86_64__
#  define WARC_CPU_X86 1
#  if defined __x86_64__
#     define WARC_CPU_X86_64 1
#  endif
#  if defined WARC_CPU_X86_64
#     define WARC_CPU_STRING "AMD x86-64"
#  else
#     define WARC_CPU_STRING "Intel 386+"
#  endif
#endif

#if defined __alpha || defined alpha || defined _M_ALPHA || defined __alpha__
#  define WARC_CPU_AXP 1
#  define WARC_CPU_STRING "AXP"
#endif

#if defined __hppa || defined hppa
#  define WARC_CPU_HPPA 1
#  define WARC_CPU_STRING "PA-RISC"
#endif

#if !defined WARC_CPU_STRING
#  error WARC cannot determine target CPU
#  define WARC_CPU_STRING "Unknown" /* this is here for Doxygen's benefit */
#endif

    /*
    ** -----------------------------------------------------------------------------
    ** Attempt to autodetect building for embedded on Sony PS2
    ** -----------------------------------------------------------------------------
    */
#if !defined WARC_OS_STRING
#  if !defined FORCE_DOXYGEN
#    define WARC_OS_EMBEDDED 1
#  endif
#  if defined _R5900
#     define WARC_OS_STRING "Sony PS2(embedded)"
#  else
#     define WARC_OS_STRING "Embedded/Unknown"
#  endif
#endif

    /*
    ** ---------------------------------------------------------------------------
    ** Handle cdecl, stdcall, fastcall, etc.
    ** ---------------------------------------------------------------------------
    */
#if defined WARC_CPU_X86 && !defined WARC_CPU_X86_64
#  if defined __GNUC__
#     define WARC_CDECL __attribute__((cdecl))
#     define WARC_STDCALL __attribute__((stdcall))
#     define WARC_FASTCALL __attribute__((fastcall))
#  elif ( defined _MSC_VER || defined __WATCOMC__ || defined __BORLANDC__ || defined __MWERKS__ )
#     define WARC_CDECL    __cdecl
#     define WARC_STDCALL  __stdcall
#     define WARC_FASTCALL __fastcall
#  endif
#else
#  define WARC_CDECL
#  define WARC_STDCALL
#  define WARC_FASTCALL
#endif

    /*
    ** ---------------------------------------------------------------------------
    ** Define WARC_IMPORTEXPORT signature based on WARC_DLL and WARC_BUILDING_LIB
    ** ---------------------------------------------------------------------------
    */

    /*
    ** We undefine this so that multiple inclusions will work
    */
#if defined WARC_IMPORTEXPORT
#  undef WARC_IMPORTEXPORT
#endif

#if defined WARC_DLL
#   if defined WARC_OS_WIN32
#      if defined _MSC_VER
#         if ( _MSC_VER >= 800 )
#            if defined WARC_BUILDING_LIB
#               define WARC_IMPORTEXPORT __declspec( dllexport )
#            else
#               define WARC_IMPORTEXPORT __declspec( dllimport )
#            endif
#         else
#            if defined WARC_BUILDING_LIB
#               define WARC_IMPORTEXPORT __export
#            else
#               define WARC_IMPORTEXPORT
#            endif
#         endif
#      endif  /* defined _MSC_VER */
#      if defined __BORLANDC__
#         if ( __BORLANDC__ >= 0x500 )
#            if defined WARC_BUILDING_LIB
#               define WARC_IMPORTEXPORT __declspec( dllexport )
#            else
#               define WARC_IMPORTEXPORT __declspec( dllimport )
#            endif
#         else
#            if defined WARC_BUILDING_LIB
#               define WARC_IMPORTEXPORT __export
#            else
#               define WARC_IMPORTEXPORT
#            endif
#         endif
#      endif /* defined __BORLANDC__ */
    /* for all other compilers, we're just making a blanket assumption */
#      if defined __GNUC__ || defined __WATCOMC__ || defined __MWERKS__
#         if defined WARC_BUILDING_LIB
#            define WARC_IMPORTEXPORT __declspec( dllexport )
#         else
#            define WARC_IMPORTEXPORT __declspec( dllimport )
#         endif
#      endif /* all other compilers */
#      if !defined WARC_IMPORTEXPORT
#         error Building DLLs not supported on this compiler (warc@hanzo.org if you know how)
#      endif
#   endif /* defined WARC_OS_WIN32 */
#endif

    /* On pretty much everything else, we can thankfully just ignore this */
#if !defined WARC_IMPORTEXPORT
#  define WARC_IMPORTEXPORT
#endif

#if defined FORCE_DOXYGEN
#  define WARC_DLL
#  define WARC_BUILDING_LIB
#  undef WARC_DLL
#  undef WARC_BUILDING_LIB
#endif

    /*
    ** ----------------------------------------------------------------------------
    ** (Re)define WARC_PUBLIC_API export signature
    ** ----------------------------------------------------------------------------
    */
#ifdef WARC_PUBLIC_API
#  undef WARC_PUBLIC_API
#endif

#if ( ( defined _MSC_VER ) && ( _MSC_VER < 800 ) ) || ( defined __BORLANDC__ && ( __BORLANDC__ < 0x500 ) )
#  define WARC_PUBLIC_API(rtype) extern rtype WARC_IMPORTEXPORT
#else
#  define WARC_PUBLIC_API(rtype) extern WARC_IMPORTEXPORT rtype
#endif

    /*
    ** ----------------------------------------------------------------------------
    ** Try to infer endianess.  Basically we just go through the CPUs we know are
    ** little endian, and assume anything that isn't one of those is big endian.
    ** As a sanity check, we also do this with operating systems we know are
    ** little endian, such as Windows.  Some processors are bi-endian, such as
    ** the MIPS series, so we have to be careful about those.
    ** ----------------------------------------------------------------------------
    */
#if defined WARC_CPU_X86 || defined WARC_CPU_AXP || defined WARC_CPU_STRONGARM || defined WARC_OS_WIN32 || defined WARC_OS_WINCE || defined __MIPSEL__
#  define WARC_ENDIAN_STRING "little"
#  define WARC_LITTLE_ENDIAN 1
#else
#  define WARC_ENDIAN_STRING "big"
#  define WARC_BIG_ENDIAN 1
#endif

#if defined FORCE_DOXYGEN
#  define WARC_LITTLE_ENDIAN
#endif

    /*
    ** ----------------------------------------------------------------------------
    ** Cross-platform compile time assertion macro
    ** ----------------------------------------------------------------------------
    */
#define WARC_COMPILE_TIME_ASSERT(name, x) typedef int _WARC_dummy_ ## name[(x) ? 1 : -1 ]

    /*
    ** ----------------------------------------------------------------------------
    ** 64-bit Integer
    **
    ** We don't require 64-bit support, nor do we emulate its functionality, we
    ** simply export it if it's available.  Since we can't count on <limits.h>
    ** for 64-bit support, we ignore the WARC_USE_LIMITS_H directive.
    ** ----------------------------------------------------------------------------
    */
#if defined ( __LP64__ ) || defined ( __powerpc64__ ) || defined WARC_CPU_SPARC64
#  define WARC_64BIT_INTEGER 1
    typedef long warc_i64_t;
    typedef unsigned long warc_u64_t;
#  define WARC_I64( x ) ((warc_i64_t)x)
#  define WARC_U64( x ) ((warc_u64_t)x)
#  define WARC_I64_PRINTF_PREFIX "l"
#elif defined _MSC_VER || defined __BORLANDC__ || defined __WATCOMC__ || ( defined __alpha && defined __DECC )
#  define WARC_64BIT_INTEGER 1
    typedef __int64 warc_i64_t;
    typedef unsigned __int64 warc_u64_t;
#  define WARC_I64( x ) ((warc_i64_t)x)
#  define WARC_U64( x ) ((warc_u64_t)x)
#  define WARC_I64_PRINTF_PREFIX "I64"
#elif defined __GNUC__ || defined __MWERKS__ || defined __SUNPRO_C || defined __SUNPRO_CC || defined __APPLE_CC__ || defined WARC_OS_IRIX || defined _LONG_LONG || defined _CRAYC
#  define WARC_64BIT_INTEGER 1
    typedef long long warc_i64_t;
    typedef unsigned long long warc_u64_t;
#  define WARC_U64( x ) ((warc_u64_t)(x##LL))
#  define WARC_I64( x ) ((warc_i64_t)(x##LL))
#  define WARC_I64_PRINTF_PREFIX "ll"
#endif

    /* hack */
#ifdef __MINGW32__
#undef WARC_I64
#undef WARC_U64
#undef WARC_I64_PRINTF_PREFIX
#define WARC_I64( x ) ((warc_i64_t)x)
#define WARC_U64( x ) ((warc_u64_t)x)
#define WARC_I64_PRINTF_PREFIX "I64"
#endif

#ifdef FORCE_DOXYGEN
    typedef long long warc_i64_t;
    typedef unsigned long warc_u64_t;
#  define WARC_64BIT_INTEGER
#  define WARC_I64_PRINTF_PREFIX
#  define WARC_I64(x)
#  define WARC_U64(x)
#endif

    /** Minimum value for a 64-bit signed integer */
#define WARC_I64_MIN  WARC_I64(0x8000000000000000)
    /** Maximum value for a 64-bit signed integer */
#define WARC_I64_MAX  WARC_I64(0x7FFFFFFFFFFFFFFF)
    /** Minimum value for a 64-bit unsigned integer */
#define WARC_U64_MIN  WARC_U64(0)
    /** Maximum value for a 64-bit unsigned integer */
#define WARC_U64_MAX  WARC_U64(0xFFFFFFFFFFFFFFFF)

    /* ----------------------------------------------------------------------------
    ** Basic Sized Types
    **
    ** These types are expected to be EXACTLY sized so you can use them for
    ** serialization.
    ** ----------------------------------------------------------------------------
    */
#define WARC_FALSE 0
#define WARC_TRUE  1

    typedef int            warc_bool_t;
    typedef unsigned char  warc_byte_t;

    /* NOTE: These assume that CHAR_BIT is 8!! */
    typedef unsigned char  warc_u8_t;
    typedef signed char    warc_i8_t;

#if defined WARC_USE_LIMITS_H
#  if CHAR_BITS > 8
#    error This machine uses 9-bit characters.  This is a warning, you can comment this out now.
#  endif /* CHAR_BITS > 8 */

    /* 16-bit */
#  if ( USHRT_MAX == 65535 )
    typedef unsigned short warc_u16_t;
    typedef short          warc_i16_t;
#  else
    /* Yes, in theory there could still be a 16-bit character type and shorts are
    32-bits in size...if you find such an architecture, let me know =P */
#    error No 16-bit type found
#  endif

    /* 32-bit */
#  if ( INT_MAX == 2147483647 )
    typedef unsigned       warc_u32_t;
    typedef int            warc_i32_t;
#  elif ( LONG_MAX == 2147483647 )
    typedef unsigned long  warc_u32_t;
    typedef long           warc_i32_t;
#  else
    error No 32 - bit type found
#  endif

#else /* WARC_USE_LIMITS_H */

    typedef unsigned short warc_u16_t;
    typedef short          warc_i16_t;

#  if !defined WARC_OS_PALM
    typedef unsigned       warc_u32_t;
    typedef int            warc_i32_t;
#  else
    typedef unsigned long  warc_u32_t;
    typedef long           warc_i32_t;
#  endif
#endif

    /** Minimum value for a byte */
#define WARC_BYTE_MIN    0
    /** Maximum value for an 8-bit unsigned value */
#define WARC_BYTE_MAX    255
    /** Minimum value for a byte */
#define WARC_I16_MIN     ( ( warc_i16_t ) 0x8000 )
    /** Maximum value for a 16-bit signed value */
#define WARC_I16_MAX     ( ( warc_i16_t ) 0x7FFF )
    /** Minimum value for a 16-bit unsigned value */
#define WARC_U16_MIN     0
    /** Maximum value for a 16-bit unsigned value */
#define WARC_U16_MAX     ( ( warc_u16_t ) 0xFFFF )
    /** Minimum value for a 32-bit signed value */
#define WARC_I32_MIN     ( ( warc_i32_t ) 0x80000000 )
    /** Maximum value for a 32-bit signed value */
#define WARC_I32_MAX     ( ( warc_i32_t ) 0x7FFFFFFF )
    /** Minimum value for a 32-bit unsigned value */
#define WARC_U32_MIN     0
    /** Maximum value for a 32-bit unsigned value */
#define WARC_U32_MAX     ( ( warc_u32_t ) 0xFFFFFFFF )

    /*
    ** ----------------------------------------------------------------------------
    ** Sanity checks on expected sizes
    ** ----------------------------------------------------------------------------
    */
#if !defined FORCE_DOXYGEN

    WARC_COMPILE_TIME_ASSERT (warc_byte_t, sizeof (warc_byte_t) == 1);
    WARC_COMPILE_TIME_ASSERT (warc_u8_t, sizeof (warc_u8_t) == 1);
    WARC_COMPILE_TIME_ASSERT (warc_i8_t, sizeof (warc_i8_t) == 1);
    WARC_COMPILE_TIME_ASSERT (warc_u16_t, sizeof (warc_u16_t) == 2);
    WARC_COMPILE_TIME_ASSERT (warc_i16_t, sizeof (warc_i16_t) == 2);
    WARC_COMPILE_TIME_ASSERT (warc_u32_t, sizeof (warc_u32_t) == 4);
    WARC_COMPILE_TIME_ASSERT (warc_i32_t, sizeof (warc_i32_t) == 4);

#if !defined WARC_NO_FLOAT
    WARC_COMPILE_TIME_ASSERT (warc_testfloat_t, sizeof (float) == 4 );
    WARC_COMPILE_TIME_ASSERT (warc_testdouble_t, sizeof (double) == 8);
#endif

#if defined WARC_64BIT_INTEGER
    WARC_COMPILE_TIME_ASSERT (warc_u64_t, sizeof (warc_u64_t) == 8);
    WARC_COMPILE_TIME_ASSERT (warc_i64_t, sizeof (warc_i64_t) == 8);
#endif

#endif

    /*
    ** ----------------------------------------------------------------------------
    ** 64-bit pointer support
    ** ----------------------------------------------------------------------------
    */
#if defined WARC_CPU_AXP && ( defined WARC_OS_TRU64 || defined WARC_OS_LINUX )
#  define WARC_64BIT_POINTER 1
#endif

#if defined WARC_CPU_X86_64 && defined WARC_OS_LINUX
#  define WARC_64BIT_POINTER 1
#endif

#if defined WARC_CPU_SPARC64 || defined WARC_OS_WIN64 || defined __64BIT__ || defined __LP64 || defined _LP64 || defined __LP64__ || defined _ADDR64 || defined _CRAYC
#   define WARC_64BIT_POINTER 1
#endif

#if defined WARC_64BIT_POINTER
    WARC_COMPILE_TIME_ASSERT ( warc_64bit_pointer, sizeof ( void * ) == 8 );
#elif !defined FORCE_DOXYGEN
    WARC_COMPILE_TIME_ASSERT ( warc_32bit_pointer, sizeof ( void * ) == 4 );
#endif

#if defined FORCE_DOXYGEN
#  define WARC_64BIT_POINTER
#endif


#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif


#ifdef __cplusplus
  }

#endif

#endif /* __WARC_WPORT_H__ */
