/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MU_PARSER_H
#define MU_PARSER_H

//--- Standard includes ------------------------------------------------------------------------
#include <vector>

//--- Parser includes --------------------------------------------------------------------------
// #include "muParserBase.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MU_PARSER_BASE_H
#define MU_PARSER_BASE_H

//--- Standard includes ------------------------------------------------------------------------
#include <cmath>
#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <locale>
#include <limits.h>

//--- Parser includes --------------------------------------------------------------------------
// #include "muParserDef.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2014 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MUP_DEF_H
#define MUP_DEF_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>

// #include "muParserFixes.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef MU_PARSER_FIXES_H
#define MU_PARSER_FIXES_H

/** \file
    \brief This file contains compatibility fixes for some platforms.
*/

//
// Compatibility fixes
//

//---------------------------------------------------------------------------
//
// Intel Compiler
//
//---------------------------------------------------------------------------

#ifdef __INTEL_COMPILER

// remark #981: operands are evaluated in unspecified order
// disabled -> completely pointless if the functions do not have side effects
//
#pragma warning(disable:981)

// remark #383: value copied to temporary, reference to temporary used
#pragma warning(disable:383)

// remark #1572: floating-point equality and inequality comparisons are unreliable
// disabled -> everyone knows it, the parser passes this problem
//             deliberately to the user
#pragma warning(disable:1572)

#endif


/* From http://gcc.gnu.org/wiki/Visibility */
/* Generic helper definitions for shared library support */
#if defined _WIN32 || defined __CYGWIN__
#define MUPARSER_HELPER_DLL_IMPORT __declspec(dllimport)
#define MUPARSER_HELPER_DLL_EXPORT __declspec(dllexport)
#define MUPARSER_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define MUPARSER_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define MUPARSER_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define MUPARSER_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define MUPARSER_HELPER_DLL_IMPORT
#define MUPARSER_HELPER_DLL_EXPORT
#define MUPARSER_HELPER_DLL_LOCAL
#endif
#endif

/* Now we use the generic helper definitions above to define API_EXPORT_CXX and MUPARSER_LOCAL.
 * API_EXPORT_CXX is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
 * MUPARSER_LOCAL is used for non-api symbols. */

#ifndef MUPARSER_STATIC /* defined if muParser is compiled as a DLL */
#ifdef MUPARSERLIB_EXPORTS /* defined if we are building the muParser DLL (instead of using it) */
#define API_EXPORT_CXX MUPARSER_HELPER_DLL_EXPORT
#else
#define API_EXPORT_CXX MUPARSER_HELPER_DLL_IMPORT
#endif /* MUPARSER_DLL_EXPORTS */
#define MUPARSER_LOCAL MUPARSER_HELPER_DLL_LOCAL
#else /* MUPARSER_STATIC is defined: this means muParser is a static lib. */
#define API_EXPORT_CXX
#define MUPARSER_LOCAL
#endif /* !MUPARSER_STATIC */


#ifdef _WIN32
#define API_EXPORT(TYPE) API_EXPORT_CXX TYPE __cdecl
#else
#define API_EXPORT(TYPE) TYPE
#endif


#endif // include guard




/** \file
    \brief This file contains standard definitions used by the parser.
*/

#define MUP_VERSION _T("2.2.6")
#define MUP_VERSION_DATE _T("20181004")

#define MUP_CHARS _T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

/** \brief If this macro is defined mathematical exceptions (div by zero) will be thrown as exceptions. */
//#define MUP_MATH_EXCEPTIONS

/** \brief Define the base datatype for values.

  This datatype must be a built in value type. You can not use custom classes.
  It should be working with all types except "int"!
*/
#define MUP_BASETYPE double

/** \brief Activate this option in order to compile with OpenMP support. 

  OpenMP is used only in the bulk mode it may increase the performance a bit. 
*/
//#define MUP_USE_OPENMP

#if defined(_UNICODE)
  /** \brief Definition of the basic parser string type. */
  #define MUP_STRING_TYPE std::wstring

  #if !defined(_T)
    #define _T(x) L##x
  #endif // not defined _T
#else
  #ifndef _T
  #define _T(x) x
  #endif
  
  /** \brief Definition of the basic parser string type. */
  #define MUP_STRING_TYPE std::string
#endif

#if defined(_DEBUG)
  /** \brief Debug macro to force an abortion of the programm with a certain message.
  */
  #define MUP_FAIL(MSG)     \
          {                 \
            bool MSG=false; \
            assert(MSG);    \
          }

    /** \brief An assertion that does not kill the program.

        This macro is neutralised in UNICODE builds. It's
        too difficult to translate.
    */
    #define MUP_ASSERT(COND)                         \
            if (!(COND))                             \
            {                                        \
              stringstream_type ss;                  \
              ss << _T("Assertion \"") _T(#COND) _T("\" failed: ") \
                 << __FILE__ << _T(" line ")         \
                 << __LINE__ << _T(".");             \
              throw ParserError( ss.str() );         \
            }
#else
  #define MUP_FAIL(MSG)
  #define MUP_ASSERT(COND)
#endif


namespace mu
{
#if defined(_UNICODE)

  //------------------------------------------------------------------------------
  /** \brief Encapsulate wcout. */
  inline std::wostream& console()
  {
    return std::wcout;
  }

  /** \brief Encapsulate cin. */
  inline std::wistream& console_in()
  {
    return std::wcin;
  }

#else

  /** \brief Encapsulate cout. 
  
    Used for supporting UNICODE more easily.
  */
  inline std::ostream& console()
  {
    return std::cout;
  }

  /** \brief Encapsulate cin. 

    Used for supporting UNICODE more easily.
  */
  inline std::istream& console_in()
  {
    return std::cin;
  }

#endif

  //------------------------------------------------------------------------------
  /** \brief Bytecode values.

      \attention The order of the operator entries must match the order in ParserBase::c_DefaultOprt!
  */
  enum ECmdCode
  {
    // The following are codes for built in binary operators
    // apart from built in operators the user has the opportunity to
    // add user defined operators.
    cmLE            = 0,   ///< Operator item:  less or equal
    cmGE            = 1,   ///< Operator item:  greater or equal
    cmNEQ           = 2,   ///< Operator item:  not equal
    cmEQ            = 3,   ///< Operator item:  equals
    cmLT            = 4,   ///< Operator item:  less than
    cmGT            = 5,   ///< Operator item:  greater than
    cmADD           = 6,   ///< Operator item:  add
    cmSUB           = 7,   ///< Operator item:  subtract
    cmMUL           = 8,   ///< Operator item:  multiply
    cmDIV           = 9,   ///< Operator item:  division
    cmPOW           = 10,  ///< Operator item:  y to the power of ...
    cmLAND          = 11,
    cmLOR           = 12,
    cmASSIGN        = 13,  ///< Operator item:  Assignment operator
    cmBO            = 14,  ///< Operator item:  opening bracket
    cmBC            = 15,  ///< Operator item:  closing bracket
    cmIF            = 16,  ///< For use in the ternary if-then-else operator
    cmELSE          = 17,  ///< For use in the ternary if-then-else operator
    cmENDIF         = 18,  ///< For use in the ternary if-then-else operator
    cmARG_SEP       = 19,  ///< function argument separator
    cmVAR           = 20,  ///< variable item
    cmVAL           = 21,  ///< value item

    // For optimization purposes
    cmVARPOW2,
    cmVARPOW3,
    cmVARPOW4,
    cmVARMUL,
    cmPOW2,

    // operators and functions
    cmFUNC,                ///< Code for a generic function item
    cmFUNC_STR,            ///< Code for a function with a string parameter
    cmFUNC_BULK,           ///< Special callbacks for Bulk mode with an additional parameter for the bulk index 
    cmSTRING,              ///< Code for a string token
    cmOPRT_BIN,            ///< user defined binary operator
    cmOPRT_POSTFIX,        ///< code for postfix operators
    cmOPRT_INFIX,          ///< code for infix operators
    cmEND,                 ///< end of formula
    cmUNKNOWN              ///< uninitialized item
  };

  //------------------------------------------------------------------------------
  /** \brief Types internally used by the parser.
  */
  enum ETypeCode
  {
    tpSTR  = 0,     ///< String type (Function arguments and constants only, no string variables)
    tpDBL  = 1,     ///< Floating point variables
    tpVOID = 2      ///< Undefined type.
  };

  //------------------------------------------------------------------------------
  enum EParserVersionInfo
  {
    pviBRIEF,
    pviFULL
  };

  //------------------------------------------------------------------------------
  /** \brief Parser operator precedence values. */
  enum EOprtAssociativity
  {
    oaLEFT  = 0,
    oaRIGHT = 1,
    oaNONE  = 2
  };

  //------------------------------------------------------------------------------
  /** \brief Parser operator precedence values. */
  enum EOprtPrecedence
  {
    // binary operators
    prLOR     = 1,
    prLAND    = 2,
    prLOGIC   = 3,  ///< logic operators
    prCMP     = 4,  ///< comparsion operators
    prADD_SUB = 5,  ///< addition
    prMUL_DIV = 6,  ///< multiplication/division
    prPOW     = 7,  ///< power operator priority (highest)

    // infix operators
    prINFIX   = 6, ///< Signs have a higher priority than ADD_SUB, but lower than power operator
    prPOSTFIX = 6  ///< Postfix operator priority (currently unused)
  };

  //------------------------------------------------------------------------------
  // basic types

  /** \brief The numeric datatype used by the parser. 
  
    Normally this is a floating point type either single or double precision.
  */
  typedef MUP_BASETYPE value_type;

  /** \brief The stringtype used by the parser. 

    Depends on whether UNICODE is used or not.
  */
  typedef MUP_STRING_TYPE string_type;

  /** \brief The character type used by the parser. 
  
    Depends on whether UNICODE is used or not.
  */
  typedef string_type::value_type char_type;

  /** \brief Typedef for easily using stringstream that respect the parser stringtype. */
  typedef std::basic_stringstream<char_type,
                                  std::char_traits<char_type>,
                                  std::allocator<char_type> > stringstream_type;

  // Data container types

  /** \brief Type used for storing variables. */
  typedef std::map<string_type, value_type*> varmap_type;
  
  /** \brief Type used for storing constants. */
  typedef std::map<string_type, value_type> valmap_type;
  
  /** \brief Type for assigning a string name to an index in the internal string table. */
  typedef std::map<string_type, std::size_t> strmap_type;

  // Parser callbacks
  
  /** \brief Callback type used for functions without arguments. */
  typedef value_type (*generic_fun_type)();

  /** \brief Callback type used for functions without arguments. */
  typedef value_type (*fun_type0)();

  /** \brief Callback type used for functions with a single arguments. */
  typedef value_type (*fun_type1)(value_type);

  /** \brief Callback type used for functions with two arguments. */
  typedef value_type (*fun_type2)(value_type, value_type);

  /** \brief Callback type used for functions with three arguments. */
  typedef value_type (*fun_type3)(value_type, value_type, value_type);

  /** \brief Callback type used for functions with four arguments. */
  typedef value_type (*fun_type4)(value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with five arguments. */
  typedef value_type (*fun_type5)(value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with six arguments. */
  typedef value_type (*fun_type6)(value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with seven arguments. */
  typedef value_type (*fun_type7)(value_type, value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with eight arguments. */
  typedef value_type (*fun_type8)(value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with nine arguments. */
  typedef value_type (*fun_type9)(value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with ten arguments. */
  typedef value_type (*fun_type10)(value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions without arguments. */
  typedef value_type (*bulkfun_type0)(int, int);

  /** \brief Callback type used for functions with a single arguments. */
  typedef value_type (*bulkfun_type1)(int, int, value_type);

  /** \brief Callback type used for functions with two arguments. */
  typedef value_type (*bulkfun_type2)(int, int, value_type, value_type);

  /** \brief Callback type used for functions with three arguments. */
  typedef value_type (*bulkfun_type3)(int, int, value_type, value_type, value_type);

  /** \brief Callback type used for functions with four arguments. */
  typedef value_type (*bulkfun_type4)(int, int, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with five arguments. */
  typedef value_type (*bulkfun_type5)(int, int, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with six arguments. */
  typedef value_type (*bulkfun_type6)(int, int, value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with seven arguments. */
  typedef value_type (*bulkfun_type7)(int, int, value_type, value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with eight arguments. */
  typedef value_type (*bulkfun_type8)(int, int, value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with nine arguments. */
  typedef value_type (*bulkfun_type9)(int, int, value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with ten arguments. */
  typedef value_type (*bulkfun_type10)(int, int, value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with a variable argument list. */
  typedef value_type (*multfun_type)(const value_type*, int);

  /** \brief Callback type used for functions taking a string as an argument. */
  typedef value_type (*strfun_type1)(const char_type*);

  /** \brief Callback type used for functions taking a string and a value as arguments. */
  typedef value_type (*strfun_type2)(const char_type*, value_type);

  /** \brief Callback type used for functions taking a string and two values as arguments. */
  typedef value_type (*strfun_type3)(const char_type*, value_type, value_type);

  /** \brief Callback used for functions that identify values in a string. */
  typedef int (*identfun_type)(const char_type *sExpr, int *nPos, value_type *fVal);

  /** \brief Callback used for variable creation factory functions. */
  typedef value_type* (*facfun_type)(const char_type*, void*);
} // end of namespace

#endif


// #include "muParserStack.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2011 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef MU_PARSER_STACK_H
#define MU_PARSER_STACK_H

#include <cassert>
#include <string>
#include <stack>
#include <vector>

// #include "muParserError.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2011 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef MU_PARSER_ERROR_H
#define MU_PARSER_ERROR_H

#include <cassert>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

// #include "muParserDef.h"


/** \file 
    \brief This file defines the error class used by the parser.
*/

namespace mu
{

/** \brief Error codes. */
enum EErrorCodes
{
  // Formula syntax errors
  ecUNEXPECTED_OPERATOR    = 0,  ///< Unexpected binary operator found
  ecUNASSIGNABLE_TOKEN     = 1,  ///< Token can't be identified.
  ecUNEXPECTED_EOF         = 2,  ///< Unexpected end of formula. (Example: "2+sin(")
  ecUNEXPECTED_ARG_SEP     = 3,  ///< An unexpected comma has been found. (Example: "1,23")
  ecUNEXPECTED_ARG         = 4,  ///< An unexpected argument has been found
  ecUNEXPECTED_VAL         = 5,  ///< An unexpected value token has been found
  ecUNEXPECTED_VAR         = 6,  ///< An unexpected variable token has been found
  ecUNEXPECTED_PARENS      = 7,  ///< Unexpected Parenthesis, opening or closing
  ecUNEXPECTED_STR         = 8,  ///< A string has been found at an inapropriate position
  ecSTRING_EXPECTED        = 9,  ///< A string function has been called with a different type of argument
  ecVAL_EXPECTED           = 10, ///< A numerical function has been called with a non value type of argument
  ecMISSING_PARENS         = 11, ///< Missing parens. (Example: "3*sin(3")
  ecUNEXPECTED_FUN         = 12, ///< Unexpected function found. (Example: "sin(8)cos(9)")
  ecUNTERMINATED_STRING    = 13, ///< unterminated string constant. (Example: "3*valueof("hello)")
  ecTOO_MANY_PARAMS        = 14, ///< Too many function parameters
  ecTOO_FEW_PARAMS         = 15, ///< Too few function parameters. (Example: "ite(1<2,2)")
  ecOPRT_TYPE_CONFLICT     = 16, ///< binary operators may only be applied to value items of the same type
  ecSTR_RESULT             = 17, ///< result is a string

  // Invalid Parser input Parameters
  ecINVALID_NAME           = 18, ///< Invalid function, variable or constant name.
  ecINVALID_BINOP_IDENT    = 19, ///< Invalid binary operator identifier
  ecINVALID_INFIX_IDENT    = 20, ///< Invalid function, variable or constant name.
  ecINVALID_POSTFIX_IDENT  = 21, ///< Invalid function, variable or constant name.

  ecBUILTIN_OVERLOAD       = 22, ///< Trying to overload builtin operator
  ecINVALID_FUN_PTR        = 23, ///< Invalid callback function pointer 
  ecINVALID_VAR_PTR        = 24, ///< Invalid variable pointer 
  ecEMPTY_EXPRESSION       = 25, ///< The Expression is empty
  ecNAME_CONFLICT          = 26, ///< Name conflict
  ecOPT_PRI                = 27, ///< Invalid operator priority
  // 
  ecDOMAIN_ERROR           = 28, ///< catch division by zero, sqrt(-1), log(0) (currently unused)
  ecDIV_BY_ZERO            = 29, ///< Division by zero (currently unused)
  ecGENERIC                = 30, ///< Generic error
  ecLOCALE                 = 31, ///< Conflict with current locale

  ecUNEXPECTED_CONDITIONAL = 32,
  ecMISSING_ELSE_CLAUSE    = 33, 
  ecMISPLACED_COLON        = 34,

  ecUNREASONABLE_NUMBER_OF_COMPUTATIONS = 35,

  // internal errors
  ecINTERNAL_ERROR         = 36, ///< Internal error of any kind.
  
  // The last two are special entries 
  ecCOUNT,                      ///< This is no error code, It just stores just the total number of error codes
  ecUNDEFINED              = -1  ///< Undefined message, placeholder to detect unassigned error messages
};

//---------------------------------------------------------------------------
/** \brief A class that handles the error messages.
*/
class ParserErrorMsg
{
public:
    static const ParserErrorMsg& Instance();
    string_type operator[](unsigned a_iIdx) const;

private:
    ParserErrorMsg& operator=(const ParserErrorMsg &) = delete;
    ParserErrorMsg(const ParserErrorMsg&) = delete;
    ParserErrorMsg();

   ~ParserErrorMsg() = default;

    std::vector<string_type>  m_vErrMsg;  ///< A vector with the predefined error messages
};

//---------------------------------------------------------------------------
/** \brief Error class of the parser. 
    \author Ingo Berg

  Part of the math parser package.
*/
class API_EXPORT_CXX ParserError
{
private:

    /** \brief Replace all ocuurences of a substring with another string. */
    void ReplaceSubString( string_type &strSource, 
                           const string_type &strFind,
                           const string_type &strReplaceWith);
    void Reset();

public:

    ParserError();
    explicit ParserError(EErrorCodes a_iErrc);
    explicit ParserError(const string_type &sMsg);
    ParserError( EErrorCodes a_iErrc,
                 const string_type &sTok,
                 const string_type &sFormula = string_type(),
                 int a_iPos = -1);
    ParserError( EErrorCodes a_iErrc, 
                 int a_iPos, 
                 const string_type &sTok);
    ParserError( const char_type *a_szMsg, 
                 int a_iPos = -1, 
                 const string_type &sTok = string_type());
    ParserError(const ParserError &a_Obj);
    ParserError& operator=(const ParserError &a_Obj);
   ~ParserError();

    void SetFormula(const string_type &a_strFormula);
    const string_type& GetExpr() const;
    const string_type& GetMsg() const;
    int GetPos() const;
    const string_type& GetToken() const;
    EErrorCodes GetCode() const;

private:
    string_type m_strMsg;     ///< The message string
    string_type m_strFormula; ///< Formula string
    string_type m_strTok;     ///< Token related with the error
    int m_iPos;               ///< Formula position related to the error 
    EErrorCodes m_iErrc;      ///< Error code
    const ParserErrorMsg &m_ErrMsg;
};		

} // namespace mu

#endif


// #include "muParserToken.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef MU_PARSER_TOKEN_H
#define MU_PARSER_TOKEN_H

#include <cassert>
#include <string>
#include <stack>
#include <vector>
#include <memory>

// #include "muParserError.h"

// #include "muParserCallback.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2011 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef MU_PARSER_CALLBACK_H
#define MU_PARSER_CALLBACK_H

// #include "muParserDef.h"


/** \file
    \brief Definition of the parser callback class.
*/

namespace mu
{

/** \brief Encapsulation of prototypes for a numerical parser function.

    Encapsulates the prototyp for numerical parser functions. The class
    stores the number of arguments for parser functions as well
    as additional flags indication the function is non optimizeable.
    The pointer to the callback function pointer is stored as void* 
    and needs to be casted according to the argument count.
    Negative argument counts indicate a parser function with a variable number
    of arguments. 

    \author (C) 2004-2011 Ingo Berg
*/
class API_EXPORT_CXX ParserCallback
{
public:
    ParserCallback(fun_type0  a_pFun, bool a_bAllowOpti);
    ParserCallback(fun_type1  a_pFun, bool a_bAllowOpti, int a_iPrec = -1, ECmdCode a_iCode=cmFUNC);
    ParserCallback(fun_type2  a_pFun, bool a_bAllowOpti, int a_iPrec, EOprtAssociativity a_eAssociativity);
    ParserCallback(fun_type2  a_pFun, bool a_bAllowOpti);
    ParserCallback(fun_type3  a_pFun, bool a_bAllowOpti);
    ParserCallback(fun_type4  a_pFun, bool a_bAllowOpti);
    ParserCallback(fun_type5  a_pFun, bool a_bAllowOpti);
    ParserCallback(fun_type6  a_pFun, bool a_bAllowOpti);
    ParserCallback(fun_type7  a_pFun, bool a_bAllowOpti);
    ParserCallback(fun_type8  a_pFun, bool a_bAllowOpti);
    ParserCallback(fun_type9  a_pFun, bool a_bAllowOpti);
    ParserCallback(fun_type10 a_pFun, bool a_bAllowOpti);

    ParserCallback(bulkfun_type0  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type1  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type2  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type3  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type4  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type5  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type6  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type7  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type8  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type9  a_pFun, bool a_bAllowOpti);
    ParserCallback(bulkfun_type10 a_pFun, bool a_bAllowOpti);

    ParserCallback(multfun_type a_pFun, bool a_bAllowOpti);
    ParserCallback(strfun_type1 a_pFun, bool a_bAllowOpti);
    ParserCallback(strfun_type2 a_pFun, bool a_bAllowOpti);
    ParserCallback(strfun_type3 a_pFun, bool a_bAllowOpti);
    ParserCallback();
    ParserCallback(const ParserCallback &a_Fun);
    
    ParserCallback* Clone() const;

    bool  IsOptimizable() const;
    void* GetAddr() const;
    ECmdCode  GetCode() const;
    ETypeCode GetType() const;
    int GetPri()  const;
    EOprtAssociativity GetAssociativity() const;
    int GetArgc() const;

private:
    void *m_pFun;                   ///< Pointer to the callback function, casted to void
    
    /** \brief Number of numeric function arguments
    
        This number is negative for functions with variable number of arguments. in this cases
        they represent the actual number of arguments found.
    */
    int   m_iArgc;      
    int   m_iPri;                   ///< Valid only for binary and infix operators; Operator precedence.
    EOprtAssociativity m_eOprtAsct; ///< Operator associativity; Valid only for binary operators 
    ECmdCode  m_iCode;
    ETypeCode m_iType;
    bool  m_bAllowOpti;             ///< Flag indication optimizeability 
};

//------------------------------------------------------------------------------
/** \brief Container for Callback objects. */
typedef std::map<string_type, ParserCallback> funmap_type; 

} // namespace mu

#endif



/** \file
    \brief This file contains the parser token definition.
*/

namespace mu
{
  /** \brief Encapsulation of the data for a single formula token. 

    Formula token implementation. Part of the Math Parser Package.
    Formula tokens can be either one of the following:
    <ul>
      <li>value</li>
      <li>variable</li>
      <li>function with numerical arguments</li>
      <li>functions with a string as argument</li>
      <li>prefix operators</li>
      <li>infix operators</li>
	    <li>binary operator</li>
    </ul>

   \author (C) 2004-2013 Ingo Berg 
  */
  template<typename TBase, typename TString>
  class ParserToken
  {
  private:

      ECmdCode  m_iCode;  ///< Type of the token; The token type is a constant of type #ECmdCode.
      ETypeCode m_iType;
      void  *m_pTok;      ///< Stores Token pointer; not applicable for all tokens
      int  m_iIdx;        ///< An otional index to an external buffer storing the token data
      TString m_strTok;   ///< Token string
      TString m_strVal;   ///< Value for string variables
      value_type m_fVal;  ///< the value 
      std::unique_ptr<ParserCallback> m_pCallback;

  public:

      //---------------------------------------------------------------------------
      /** \brief Constructor (default).
        
          Sets token to an neutral state of type cmUNKNOWN.
          \throw nothrow
          \sa ECmdCode
      */
      ParserToken()
        :m_iCode(cmUNKNOWN)
        ,m_iType(tpVOID)
        ,m_pTok(0)
        ,m_iIdx(-1)
        ,m_strTok()
		,m_strVal()
		,m_fVal(0)
        ,m_pCallback()
      {}

      //------------------------------------------------------------------------------
      /** \brief Create token from another one.
      
          Implemented by calling Assign(...)
          \throw nothrow
          \post m_iType==cmUNKNOWN
          \sa #Assign
      */
      ParserToken(const ParserToken &a_Tok)
      {
        Assign(a_Tok);
      }
      
      //------------------------------------------------------------------------------
      /** \brief Assignment operator. 
      
          Copy token state from another token and return this.
          Implemented by calling Assign(...).
          \throw nothrow
      */
      ParserToken& operator=(const ParserToken &a_Tok)
      {
        Assign(a_Tok);
        return *this;
      }

      //------------------------------------------------------------------------------
      /** \brief Copy token information from argument.
      
          \throw nothrow
      */
      void Assign(const ParserToken &a_Tok)
      {
        m_iCode = a_Tok.m_iCode;
        m_pTok = a_Tok.m_pTok;
        m_strTok = a_Tok.m_strTok;
        m_iIdx = a_Tok.m_iIdx;
        m_strVal = a_Tok.m_strVal;
        m_iType = a_Tok.m_iType;
        m_fVal = a_Tok.m_fVal;
        // create new callback object if a_Tok has one 
        m_pCallback.reset(a_Tok.m_pCallback.get() ? a_Tok.m_pCallback->Clone() : 0);
      }

      //------------------------------------------------------------------------------
      /** \brief Assign a token type. 

        Token may not be of type value, variable or function. Those have separate set functions. 

        \pre [assert] a_iType!=cmVAR
        \pre [assert] a_iType!=cmVAL
        \pre [assert] a_iType!=cmFUNC
        \post m_fVal = 0
        \post m_pTok = 0
      */
      ParserToken& Set(ECmdCode a_iType, const TString &a_strTok=TString())
      {
        // The following types can't be set this way, they have special Set functions
        assert(a_iType!=cmVAR);
        assert(a_iType!=cmVAL);
        assert(a_iType!=cmFUNC);

        m_iCode = a_iType;
        m_iType = tpVOID;
        m_pTok = 0;
        m_strTok = a_strTok;
        m_iIdx = -1;

        return *this;
      }

      //------------------------------------------------------------------------------
      /** \brief Set Callback type. */
      ParserToken& Set(const ParserCallback &a_pCallback, const TString &a_sTok)
      {
        assert(a_pCallback.GetAddr());

        m_iCode = a_pCallback.GetCode();
        m_iType = tpVOID;
        m_strTok = a_sTok;
        m_pCallback.reset(new ParserCallback(a_pCallback));

        m_pTok = 0;
        m_iIdx = -1;
        
        return *this;
      }

      //------------------------------------------------------------------------------
      /** \brief Make this token a value token. 
      
          Member variables not necessary for value tokens will be invalidated.
          \throw nothrow
      */
      ParserToken& SetVal(TBase a_fVal, const TString &a_strTok=TString())
      {
        m_iCode = cmVAL;
        m_iType = tpDBL;
        m_fVal = a_fVal;
        m_strTok = a_strTok;
        m_iIdx = -1;
        
        m_pTok = 0;
        m_pCallback.reset(0);

        return *this;
      }

      //------------------------------------------------------------------------------
      /** \brief make this token a variable token. 
      
          Member variables not necessary for variable tokens will be invalidated.
          \throw nothrow
      */
      ParserToken& SetVar(TBase *a_pVar, const TString &a_strTok)
      {
        m_iCode = cmVAR;
        m_iType = tpDBL;
        m_strTok = a_strTok;
        m_iIdx = -1;
        m_pTok = (void*)a_pVar;
        m_pCallback.reset(0);
        return *this;
      }

      //------------------------------------------------------------------------------
      /** \brief Make this token a variable token. 
      
          Member variables not necessary for variable tokens will be invalidated.
          \throw nothrow
      */
      ParserToken& SetString(const TString &a_strTok, std::size_t a_iSize)
      {
        m_iCode = cmSTRING;
        m_iType = tpSTR;
        m_strTok = a_strTok;
        m_iIdx = static_cast<int>(a_iSize);

        m_pTok = 0;
        m_pCallback.reset(0);
        return *this;
      }

      //------------------------------------------------------------------------------
      /** \brief Set an index associated with the token related data. 
      
          In cmSTRFUNC - This is the index to a string table in the main parser.
          \param a_iIdx The index the string function result will take in the bytecode parser.
          \throw exception_type if #a_iIdx<0 or #m_iType!=cmSTRING
      */
      void SetIdx(int a_iIdx)
      {
        if (m_iCode!=cmSTRING || a_iIdx<0)
	        throw ParserError(ecINTERNAL_ERROR);
        
        m_iIdx = a_iIdx;
      }

      //------------------------------------------------------------------------------
      /** \brief Return Index associated with the token related data. 
      
          In cmSTRFUNC - This is the index to a string table in the main parser.

          \throw exception_type if #m_iIdx<0 or #m_iType!=cmSTRING
          \return The index the result will take in the Bytecode calculatin array (#m_iIdx).
      */
      int GetIdx() const
      {
        if (m_iIdx<0 || m_iCode!=cmSTRING )
          throw ParserError(ecINTERNAL_ERROR);

        return m_iIdx;
      }

      //------------------------------------------------------------------------------
      /** \brief Return the token type.
      
          \return #m_iType
          \throw nothrow
      */
      ECmdCode GetCode() const
      {
        if (m_pCallback.get())
        {
          return m_pCallback->GetCode();
        }
        else
        {
          return m_iCode;
        }
      }

      //------------------------------------------------------------------------------
      ETypeCode GetType() const
      {
        if (m_pCallback.get())
        {
          return m_pCallback->GetType();
        }
        else
        {
          return m_iType;
        }
      }
      
      //------------------------------------------------------------------------------
      int GetPri() const
      {
        if ( !m_pCallback.get())
	        throw ParserError(ecINTERNAL_ERROR);
            
        if ( m_pCallback->GetCode()!=cmOPRT_BIN && m_pCallback->GetCode()!=cmOPRT_INFIX)
	        throw ParserError(ecINTERNAL_ERROR);

        return m_pCallback->GetPri();
      }

      //------------------------------------------------------------------------------
      EOprtAssociativity GetAssociativity() const
      {
        if (m_pCallback.get()==NULL || m_pCallback->GetCode()!=cmOPRT_BIN)
	        throw ParserError(ecINTERNAL_ERROR);

        return m_pCallback->GetAssociativity();
      }

      //------------------------------------------------------------------------------
      /** \brief Return the address of the callback function assoziated with
                 function and operator tokens.

          \return The pointer stored in #m_pTok.
          \throw exception_type if token type is non of:
                 <ul>
                   <li>cmFUNC</li>
                   <li>cmSTRFUNC</li>
                   <li>cmPOSTOP</li>
                   <li>cmINFIXOP</li>
                   <li>cmOPRT_BIN</li>
                 </ul>
          \sa ECmdCode
      */
      generic_fun_type GetFuncAddr() const
      {
        return (m_pCallback.get()) ? (generic_fun_type)m_pCallback->GetAddr() : 0;
      }

      //------------------------------------------------------------------------------
      /** \biref Get value of the token.
        
          Only applicable to variable and value tokens.
          \throw exception_type if token is no value/variable token.
      */
      TBase GetVal() const
      {
        switch (m_iCode)
        {
          case cmVAL:  return m_fVal;
          case cmVAR:  return *((TBase*)m_pTok);
          default:     throw ParserError(ecVAL_EXPECTED);
        }
      }

      //------------------------------------------------------------------------------
      /** \brief Get address of a variable token.

        Valid only if m_iType==CmdVar.
        \throw exception_type if token is no variable token.
      */
      TBase* GetVar() const
      {
        if (m_iCode!=cmVAR)
	        throw ParserError(ecINTERNAL_ERROR);

        return (TBase*)m_pTok;
      }

      //------------------------------------------------------------------------------
      /** \brief Return the number of function arguments. 

        Valid only if m_iType==CmdFUNC.
      */
      int GetArgCount() const
      {
        assert(m_pCallback.get());

        if (!m_pCallback->GetAddr())
	        throw ParserError(ecINTERNAL_ERROR);

        return m_pCallback->GetArgc();
      }

      //------------------------------------------------------------------------------
      /** \brief Return the token identifier. 
          
          If #m_iType is cmSTRING the token identifier is the value of the string argument
          for a string function.
          \return #m_strTok
          \throw nothrow
          \sa m_strTok
      */
      const TString& GetAsString() const
      {
        return m_strTok;
      }
  };
} // namespace mu

#endif


/** \file 
    \brief This file defines the stack used by muparser.
*/

namespace mu
{

  /** \brief Parser stack implementation. 

      Stack implementation based on a std::stack. The behaviour of pop() had been
      slightly changed in order to get an error code if the stack is empty.
      The stack is used within the Parser both as a value stack and as an operator stack.

      \author (C) 2004-2011 Ingo Berg 
  */
  template <typename TValueType>
  class ParserStack 
  {
    private:

      /** \brief Type of the underlying stack implementation. */
      typedef std::stack<TValueType, std::vector<TValueType> > impl_type;
      
      impl_type m_Stack;  ///< This is the actual stack.

    public:	
  	 
      //---------------------------------------------------------------------------
      ParserStack()
        :m_Stack()
      {}

      //---------------------------------------------------------------------------
      virtual ~ParserStack()
      {}

      //---------------------------------------------------------------------------
      /** \brief Pop a value from the stack.
       
        Unlike the standard implementation this function will return the value that
        is going to be taken from the stack.

        \throw ParserException in case the stack is empty.
        \sa pop(int &a_iErrc)
      */
	    TValueType pop()
      {
        if (empty())
          throw ParserError( _T("stack is empty.") );

        TValueType el = top();
        m_Stack.pop();
        return el;
      }

      /** \brief Push an object into the stack. 

          \param a_Val object to push into the stack.
          \throw nothrow
      */
      void push(const TValueType& a_Val) 
      { 
        m_Stack.push(a_Val); 
      }

      /** \brief Return the number of stored elements. */
      unsigned size() const
      { 
        return (unsigned)m_Stack.size(); 
      }

      /** \brief Returns true if stack is empty false otherwise. */
      bool empty() const
      {
        return m_Stack.empty(); 
      }
       
      /** \brief Return reference to the top object in the stack. 
       
          The top object is the one pushed most recently.
      */
      TValueType& top() 
      { 
        return m_Stack.top(); 
      }
  };
} // namespace MathUtils

#endif

// #include "muParserTokenReader.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef MU_PARSER_TOKEN_READER_H
#define MU_PARSER_TOKEN_READER_H

#include <cassert>
#include <cstdio>
#include <cstring>
#include <list>
#include <map>
#include <memory>
#include <stack>
#include <string>

// #include "muParserDef.h"

// #include "muParserToken.h"


/** \file
    \brief This file contains the parser token reader definition.
*/


namespace mu
{
  // Forward declaration
  class ParserBase;

  /** \brief Token reader for the ParserBase class.

  */
  class ParserTokenReader 
  {
  private:

      typedef ParserToken<value_type, string_type> token_type;

  public:

      ParserTokenReader(ParserBase *a_pParent);
      ParserTokenReader* Clone(ParserBase *a_pParent) const;

      void AddValIdent(identfun_type a_pCallback);
      void SetVarCreator(facfun_type a_pFactory, void *pUserData);
      void SetFormula(const string_type &a_strFormula);
      void SetArgSep(char_type cArgSep);

      int GetPos() const;
      const string_type& GetExpr() const;
      varmap_type& GetUsedVar();
      char_type GetArgSep() const;

      void IgnoreUndefVar(bool bIgnore);
      void ReInit();
      token_type ReadNextToken();

  private:

      /** \brief Syntax codes. 
  	
	        The syntax codes control the syntax check done during the first time parsing of 
          the expression string. They are flags that indicate which tokens are allowed next
          if certain tokens are identified.
  	  */
      enum ESynCodes
      {
        noBO      = 1 << 0,  ///< to avoid i.e. "cos(7)(" 
        noBC      = 1 << 1,  ///< to avoid i.e. "sin)" or "()"
        noVAL     = 1 << 2,  ///< to avoid i.e. "tan 2" or "sin(8)3.14"
        noVAR     = 1 << 3,  ///< to avoid i.e. "sin a" or "sin(8)a"
        noARG_SEP = 1 << 4,  ///< to avoid i.e. ",," or "+," ...
        noFUN     = 1 << 5,  ///< to avoid i.e. "sqrt cos" or "(1)sin"	
        noOPT     = 1 << 6,  ///< to avoid i.e. "(+)"
        noPOSTOP  = 1 << 7,  ///< to avoid i.e. "(5!!)" "sin!"
	      noINFIXOP = 1 << 8,  ///< to avoid i.e. "++4" "!!4"
        noEND     = 1 << 9,  ///< to avoid unexpected end of formula
        noSTR     = 1 << 10, ///< to block numeric arguments on string functions
        noASSIGN  = 1 << 11, ///< to block assignment to constant i.e. "4=7"
        noIF      = 1 << 12,
        noELSE    = 1 << 13,
        sfSTART_OF_LINE = noOPT | noBC | noPOSTOP | noASSIGN | noIF | noELSE | noARG_SEP,
        noANY     = ~0       ///< All of he above flags set
      };	

      ParserTokenReader(const ParserTokenReader &a_Reader);
      ParserTokenReader& operator=(const ParserTokenReader &a_Reader);
      void Assign(const ParserTokenReader &a_Reader);

      void SetParent(ParserBase *a_pParent);
      int ExtractToken(const char_type *a_szCharSet, 
                       string_type &a_strTok, 
                       int a_iPos) const;
      int ExtractOperatorToken(string_type &a_sTok, int a_iPos) const;

      bool IsBuiltIn(token_type &a_Tok);
      bool IsArgSep(token_type &a_Tok);
      bool IsEOF(token_type &a_Tok);
      bool IsInfixOpTok(token_type &a_Tok);
      bool IsFunTok(token_type &a_Tok);
      bool IsPostOpTok(token_type &a_Tok);
      bool IsOprt(token_type &a_Tok);
      bool IsValTok(token_type &a_Tok);
      bool IsVarTok(token_type &a_Tok);
      bool IsStrVarTok(token_type &a_Tok);
      bool IsUndefVarTok(token_type &a_Tok);
      bool IsString(token_type &a_Tok);
      void Error(EErrorCodes a_iErrc, 
                 int a_iPos = -1, 
                 const string_type &a_sTok = string_type() ) const;

      token_type& SaveBeforeReturn(const token_type &tok);

      ParserBase *m_pParser;
      string_type m_strFormula;
      int  m_iPos;
      int  m_iSynFlags;
      bool m_bIgnoreUndefVar;

      const funmap_type *m_pFunDef;
      const funmap_type *m_pPostOprtDef;
      const funmap_type *m_pInfixOprtDef;
      const funmap_type *m_pOprtDef;
      const valmap_type *m_pConstDef;
      const strmap_type *m_pStrVarDef;
      varmap_type *m_pVarDef;  ///< The only non const pointer to parser internals
      facfun_type m_pFactory;
      void *m_pFactoryData;
      std::list<identfun_type> m_vIdentFun; ///< Value token identification function
      varmap_type m_UsedVar;
      value_type m_fZero;      ///< Dummy value of zero, referenced by undefined variables
      int m_iBrackets;
      token_type m_lastTok;
      char_type m_cArgSep;     ///< The character used for separating function arguments
  };
} // namespace mu

#endif



// #include "muParserBytecode.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MU_PARSER_BYTECODE_H
#define MU_PARSER_BYTECODE_H

#include <cassert>
#include <string>
#include <stack>
#include <vector>

// #include "muParserDef.h"

// #include "muParserError.h"

// #include "muParserToken.h"


/** \file
    \brief Definition of the parser bytecode class.
*/


namespace mu
{
  struct SToken
  {
    ECmdCode Cmd;
    int StackPos;

    union
    {
      struct //SValData
      {
        value_type *ptr;
        value_type  data;
        value_type  data2;
      } Val;

      struct //SFunData
      {
        // Note: generic_fun_type is merely a placeholder. The real type could be 
        //       anything between gun_type1 and fun_type9. I can't use a void
        //       pointer due to constraints in the ANSI standard which allows
        //       data pointers and function pointers to differ in size.
        generic_fun_type ptr;
        int   argc;
        int   idx;
      } Fun;

      struct //SOprtData
      {
        value_type *ptr;
        int offset;
      } Oprt;
    };
  };
  
  
  /** \brief Bytecode implementation of the Math Parser.

  The bytecode contains the formula converted to revers polish notation stored in a continious
  memory area. Associated with this data are operator codes, variable pointers, constant 
  values and function pointers. Those are necessary in order to calculate the result.
  All those data items will be casted to the underlying datatype of the bytecode.

  \author (C) 2004-2013 Ingo Berg 
*/
class ParserByteCode
{
private:

    /** \brief Token type for internal use only. */
    typedef ParserToken<value_type, string_type> token_type;

    /** \brief Token vector for storing the RPN. */
    typedef std::vector<SToken> rpn_type;

    /** \brief Position in the Calculation array. */
    unsigned m_iStackPos;

    /** \brief Maximum size needed for the stack. */
    std::size_t m_iMaxStackSize;
    
    /** \brief The actual rpn storage. */
    rpn_type  m_vRPN;

    bool m_bEnableOptimizer;

    void ConstantFolding(ECmdCode a_Oprt);

public:

    ParserByteCode();
    ParserByteCode(const ParserByteCode &a_ByteCode);
    ParserByteCode& operator=(const ParserByteCode &a_ByteCode);
    void Assign(const ParserByteCode &a_ByteCode);

    void AddVar(value_type *a_pVar);
    void AddVal(value_type a_fVal);
    void AddOp(ECmdCode a_Oprt);
    void AddIfElse(ECmdCode a_Oprt);
    void AddAssignOp(value_type *a_pVar);
    void AddFun(generic_fun_type a_pFun, int a_iArgc);
    void AddBulkFun(generic_fun_type a_pFun, int a_iArgc);
    void AddStrFun(generic_fun_type a_pFun, int a_iArgc, int a_iIdx);

    void EnableOptimizer(bool bStat);

    void Finalize();
    void clear();
    std::size_t GetMaxStackSize() const;
    std::size_t GetSize() const;

    const SToken* GetBase() const;
    void AsciiDump();
};

} // namespace mu

#endif



// #include "muParserError.h"



namespace mu
{
/** \file
    \brief This file contains the class definition of the muparser engine.
*/

//--------------------------------------------------------------------------------------------------
/** \brief Mathematical expressions parser (base parser engine).
    \author (C) 2013 Ingo Berg

  This is the implementation of a bytecode based mathematical expressions parser. 
  The formula will be parsed from string and converted into a bytecode. 
  Future calculations will be done with the bytecode instead the formula string
  resulting in a significant performance increase. 
  Complementary to a set of internally implemented functions the parser is able to handle 
  user defined functions and variables. 
*/
class API_EXPORT_CXX ParserBase
{
friend class ParserTokenReader;

private:

    /** \brief Typedef for the parse functions. 
    
      The parse function do the actual work. The parser exchanges
      the function pointer to the parser function depending on 
      which state it is in. (i.e. bytecode parser vs. string parser)
    */
    typedef value_type (ParserBase::*ParseFunction)() const;  

    /** \brief Type used for storing an array of values. */
    typedef std::vector<value_type> valbuf_type;

    /** \brief Type for a vector of strings. */
    typedef std::vector<string_type> stringbuf_type;

    /** \brief Typedef for the token reader. */
    typedef ParserTokenReader token_reader_type;
    
    /** \brief Type used for parser tokens. */
    typedef ParserToken<value_type, string_type> token_type;

    /** \brief Maximum number of threads spawned by OpenMP when using the bulk mode. */
    static const int s_MaxNumOpenMPThreads;

 public:

    /** \brief Type of the error class. 
    
      Included for backwards compatibility.
    */
    typedef ParserError exception_type;

    static void EnableDebugDump(bool bDumpCmd, bool bDumpStack);

    ParserBase(); 
    ParserBase(const ParserBase &a_Parser);
    ParserBase& operator=(const ParserBase &a_Parser);

    virtual ~ParserBase();
    
	  value_type  Eval() const;
    value_type* Eval(int &nStackSize) const;
    void Eval(value_type *results, int nBulkSize);

    int GetNumResults() const;

    void SetExpr(const string_type &a_sExpr);
    void SetVarFactory(facfun_type a_pFactory, void *pUserData = NULL);

    void SetDecSep(char_type cDecSep);
    void SetThousandsSep(char_type cThousandsSep = 0);
    void ResetLocale();

    void EnableOptimizer(bool a_bIsOn=true);
    void EnableBuiltInOprt(bool a_bIsOn=true);

    bool HasBuiltInOprt() const;
    void AddValIdent(identfun_type a_pCallback);

    /** \fn void mu::ParserBase::DefineFun(const string_type &a_strName, fun_type0 a_pFun, bool a_bAllowOpt = true) 
        \brief Define a parser function without arguments.
        \param a_strName Name of the function
        \param a_pFun Pointer to the callback function
        \param a_bAllowOpt A flag indicating this function may be optimized
    */
    template<typename T>
    void DefineFun(const string_type &a_strName, T a_pFun, bool a_bAllowOpt = true)
    {
      AddCallback( a_strName, ParserCallback(a_pFun, a_bAllowOpt), m_FunDef, ValidNameChars() );
    }

    void DefineOprt(const string_type &a_strName, 
                    fun_type2 a_pFun, 
                    unsigned a_iPri=0, 
                    EOprtAssociativity a_eAssociativity = oaLEFT,
                    bool a_bAllowOpt = false);
    void DefineConst(const string_type &a_sName, value_type a_fVal);
    void DefineStrConst(const string_type &a_sName, const string_type &a_strVal);
    void DefineVar(const string_type &a_sName, value_type *a_fVar);
    void DefinePostfixOprt(const string_type &a_strFun, fun_type1 a_pOprt, bool a_bAllowOpt=true);
    void DefineInfixOprt(const string_type &a_strName, fun_type1 a_pOprt, int a_iPrec=prINFIX, bool a_bAllowOpt=true);

    // Clear user defined variables, constants or functions
    void ClearVar();
    void ClearFun();
    void ClearConst();
    void ClearInfixOprt();
    void ClearPostfixOprt();
    void ClearOprt();
    
    void RemoveVar(const string_type &a_strVarName);
    const varmap_type& GetUsedVar() const;
    const varmap_type& GetVar() const;
    const valmap_type& GetConst() const;
    const string_type& GetExpr() const;
    const funmap_type& GetFunDef() const;
    string_type GetVersion(EParserVersionInfo eInfo = pviFULL) const;

    const char_type ** GetOprtDef() const;
    void DefineNameChars(const char_type *a_szCharset);
    void DefineOprtChars(const char_type *a_szCharset);
    void DefineInfixOprtChars(const char_type *a_szCharset);

    const char_type* ValidNameChars() const;
    const char_type* ValidOprtChars() const;
    const char_type* ValidInfixOprtChars() const;

    void SetArgSep(char_type cArgSep);
    char_type GetArgSep() const;
    
    void  Error(EErrorCodes a_iErrc, 
                int a_iPos = (int)mu::string_type::npos, 
                const string_type &a_strTok = string_type() ) const;

 protected:
	  
    void Init();

    virtual void InitCharSets() = 0;
    virtual void InitFun() = 0;
    virtual void InitConst() = 0;
    virtual void InitOprt() = 0; 

    virtual void OnDetectVar(string_type *pExpr, int &nStart, int &nEnd);

    static const char_type *c_DefaultOprt[]; 
    static std::locale s_locale;  ///< The locale used by the parser
    static bool g_DbgDumpCmdCode;
    static bool g_DbgDumpStack;

    /** \brief A facet class used to change decimal and thousands separator. */
    template<class TChar>
    class change_dec_sep : public std::numpunct<TChar>
    {
    public:
      
      explicit change_dec_sep(char_type cDecSep, char_type cThousandsSep = 0, int nGroup = 3)
        :std::numpunct<TChar>()
        ,m_nGroup(nGroup)
        ,m_cDecPoint(cDecSep)
        ,m_cThousandsSep(cThousandsSep)
      {}
      
    protected:
      
      virtual char_type do_decimal_point() const
      {
        return m_cDecPoint;
      }

      virtual char_type do_thousands_sep() const
      {
        return m_cThousandsSep;
      }

      virtual std::string do_grouping() const 
      { 
		// fix for issue 4: https://code.google.com/p/muparser/issues/detail?id=4
		// courtesy of Jens Bartsch
		// original code:
		//        return std::string(1, (char)m_nGroup); 
		// new code:
		return std::string(1, (char)(m_cThousandsSep > 0 ? m_nGroup : CHAR_MAX));
      }

    private:

      int m_nGroup;
      char_type m_cDecPoint;  
      char_type m_cThousandsSep;
    };

 private:

    void Assign(const ParserBase &a_Parser);
    void InitTokenReader();
    void ReInit() const;

    void AddCallback( const string_type &a_strName, 
                      const ParserCallback &a_Callback, 
                      funmap_type &a_Storage,
                      const char_type *a_szCharSet );

    void ApplyRemainingOprt(ParserStack<token_type> &a_stOpt,
                                ParserStack<token_type> &a_stVal) const;
    void ApplyBinOprt(ParserStack<token_type> &a_stOpt,
                      ParserStack<token_type> &a_stVal) const;

    void ApplyIfElse(ParserStack<token_type> &a_stOpt,
                     ParserStack<token_type> &a_stVal) const;

    void ApplyFunc(ParserStack<token_type> &a_stOpt,
                   ParserStack<token_type> &a_stVal, 
                   int iArgCount) const; 

    token_type ApplyStrFunc(const token_type &a_FunTok,
                            const std::vector<token_type> &a_vArg) const;

    int GetOprtPrecedence(const token_type &a_Tok) const;
    EOprtAssociativity GetOprtAssociativity(const token_type &a_Tok) const;

    void CreateRPN() const;

    value_type ParseString() const; 
    value_type ParseCmdCode() const;
    value_type ParseCmdCodeBulk(int nOffset, int nThreadID) const;

    void  CheckName(const string_type &a_strName, const string_type &a_CharSet) const;
    void  CheckOprt(const string_type &a_sName,
                    const ParserCallback &a_Callback,
                    const string_type &a_szCharSet) const;

    void StackDump(const ParserStack<token_type > &a_stVal, 
                   const ParserStack<token_type > &a_stOprt) const;

    /** \brief Pointer to the parser function. 
    
      Eval() calls the function whose address is stored there.
    */
    mutable ParseFunction  m_pParseFormula;
    mutable ParserByteCode m_vRPN;        ///< The Bytecode class.
    mutable stringbuf_type  m_vStringBuf; ///< String buffer, used for storing string function arguments
    stringbuf_type  m_vStringVarBuf;

    std::unique_ptr<token_reader_type> m_pTokenReader; ///< Managed pointer to the token reader object.

    funmap_type  m_FunDef;         ///< Map of function names and pointers.
    funmap_type  m_PostOprtDef;    ///< Postfix operator callbacks
    funmap_type  m_InfixOprtDef;   ///< unary infix operator.
    funmap_type  m_OprtDef;        ///< Binary operator callbacks
    valmap_type  m_ConstDef;       ///< user constants.
    strmap_type  m_StrVarDef;      ///< user defined string constants
    varmap_type  m_VarDef;         ///< user defind variables.

    bool m_bBuiltInOp;             ///< Flag that can be used for switching built in operators on and off

    string_type m_sNameChars;      ///< Charset for names
    string_type m_sOprtChars;      ///< Charset for postfix/ binary operator tokens
    string_type m_sInfixOprtChars; ///< Charset for infix operator tokens
    
    mutable int m_nIfElseCounter;  ///< Internal counter for keeping track of nested if-then-else clauses

    // items merely used for caching state information
    mutable valbuf_type m_vStackBuffer; ///< This is merely a buffer used for the stack in the cmd parsing routine
    mutable int m_nFinalResultIdx;
};

} // namespace mu

#endif

// #include "muParserTemplateMagic.h"
#ifndef MU_PARSER_TEMPLATE_MAGIC_H
#define MU_PARSER_TEMPLATE_MAGIC_H

#include <cmath>
// #include "muParserError.h"



namespace mu
{
  //-----------------------------------------------------------------------------------------------
  //
  // Compile time type detection
  //
  //-----------------------------------------------------------------------------------------------

  /** \brief A class singling out integer types at compile time using 
             template meta programming.
  */
  template<typename T>
  struct TypeInfo
  {
    static bool IsInteger() { return false; }
  };

  template<>
  struct TypeInfo<char>
  {
    static bool IsInteger() { return true;  }
  };

  template<>
  struct TypeInfo<short>
  {
    static bool IsInteger() { return true;  }
  };

  template<>
  struct TypeInfo<int>
  {
    static bool IsInteger() { return true;  }
  };

  template<>
  struct TypeInfo<long>
  {
    static bool IsInteger() { return true;  }
  };

  template<>
  struct TypeInfo<unsigned char>
  {
    static bool IsInteger() { return true;  }
  };

  template<>
  struct TypeInfo<unsigned short>
  {
    static bool IsInteger() { return true;  }
  };

  template<>
  struct TypeInfo<unsigned int>
  {
    static bool IsInteger() { return true;  }
  };

  template<>
  struct TypeInfo<unsigned long>
  {
    static bool IsInteger() { return true;  }
  };


  //-----------------------------------------------------------------------------------------------
  //
  // Standard math functions with dummy overload for integer types
  //
  //-----------------------------------------------------------------------------------------------

  /** \brief A template class for providing wrappers for essential math functions.

    This template is spezialized for several types in order to provide a unified interface
    for parser internal math function calls regardless of the data type.
  */
  template<typename T>
  struct MathImpl
  {
    static T Sin(T v)   { return sin(v);  }
    static T Cos(T v)   { return cos(v);  }
    static T Tan(T v)   { return tan(v);  }
    static T ASin(T v)  { return asin(v); }
    static T ACos(T v)  { return acos(v); }
    static T ATan(T v)  { return atan(v); }
    static T ATan2(T v1, T v2) { return atan2(v1, v2); }
    static T Sinh(T v)  { return sinh(v); }
    static T Cosh(T v)  { return cosh(v); }
    static T Tanh(T v)  { return tanh(v); }
    static T ASinh(T v) { return log(v + sqrt(v * v + 1)); }
    static T ACosh(T v) { return log(v + sqrt(v * v - 1)); }
    static T ATanh(T v) { return ((T)0.5 * log((1 + v) / (1 - v))); }
    static T Log(T v)   { return log(v); } 
    static T Log2(T v)  { return log(v)/log((T)2); } // Logarithm base 2
    static T Log10(T v) { return log10(v); }         // Logarithm base 10
    static T Exp(T v)   { return exp(v);   }
    static T Abs(T v)   { return (v>=0) ? v : -v; }
    static T Sqrt(T v)  { return sqrt(v); }
    static T Rint(T v)  { return floor(v + (T)0.5); }
    static T Sign(T v)  { return (T)((v<0) ? -1 : (v>0) ? 1 : 0); }
    static T Pow(T v1, T v2) { return std::pow(v1, v2); }
  };
}

#endif


/** \file
    \brief Definition of the standard floating point parser.
*/

namespace mu
{
  /** \brief Mathematical expressions parser.
    
    Standard implementation of the mathematical expressions parser. 
    Can be used as a reference implementation for subclassing the parser.

    <small>
    (C) 2011 Ingo Berg<br>
    muparser(at)beltoforion.de
    </small>
  */
  /* final */ class API_EXPORT_CXX Parser : public ParserBase
  {
  public:

    Parser();

    virtual void InitCharSets();
    virtual void InitFun();
    virtual void InitConst();
    virtual void InitOprt();
    virtual void OnDetectVar(string_type *pExpr, int &nStart, int &nEnd);

    value_type Diff(value_type *a_Var, 
                    value_type a_fPos, 
                    value_type a_fEpsilon = 0) const;

  protected:

    // Trigonometric functions
    static value_type  Sin(value_type);
    static value_type  Cos(value_type);
    static value_type  Tan(value_type);
    static value_type  Tan2(value_type, value_type);
    // arcus functions
    static value_type  ASin(value_type);
    static value_type  ACos(value_type);
    static value_type  ATan(value_type);
    static value_type  ATan2(value_type, value_type);

    // hyperbolic functions
    static value_type  Sinh(value_type);
    static value_type  Cosh(value_type);
    static value_type  Tanh(value_type);
    // arcus hyperbolic functions
    static value_type  ASinh(value_type);
    static value_type  ACosh(value_type);
    static value_type  ATanh(value_type);
    // Logarithm functions
    static value_type  Log2(value_type);  // Logarithm Base 2
    static value_type  Log10(value_type); // Logarithm Base 10
    static value_type  Ln(value_type);    // Logarithm Base e (natural logarithm)
    // misc
    static value_type  Exp(value_type);
    static value_type  Abs(value_type);
    static value_type  Sqrt(value_type);
    static value_type  Rint(value_type);
    static value_type  Sign(value_type);

    // Prefix operators
    // !!! Unary Minus is a MUST if you want to use negative signs !!!
    static value_type  UnaryMinus(value_type);
    static value_type  UnaryPlus(value_type);

    // Functions with variable number of arguments
    static value_type Sum(const value_type*, int);  // sum
    static value_type Avg(const value_type*, int);  // mean value
    static value_type Min(const value_type*, int);  // minimum
    static value_type Max(const value_type*, int);  // maximum

    static int IsVal(const char_type* a_szExpr, int *a_iPos, value_type *a_fVal);
  };
} // namespace mu

#endif

/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef MU_PARSER_INT_H
#define MU_PARSER_INT_H

// #include "muParserBase.h"

#include <vector>


/** \file
    \brief Definition of a parser using integer value.
*/


namespace mu
{

/** \brief Mathematical expressions parser.
  
  This version of the parser handles only integer numbers. It disables the built in operators thus it is 
  slower than muParser. Integer values are stored in the double value_type and converted if needed.
*/
class ParserInt : public ParserBase
{
private:
    static int  Round(value_type v) { return (int)(v + ((v>=0) ? 0.5 : -0.5) ); };
  
    static value_type  Abs(value_type);
    static value_type  Sign(value_type);
    static value_type  Ite(value_type, value_type, value_type);
    // !! The unary Minus is a MUST, otherwise you can't use negative signs !!
    static value_type  UnaryMinus(value_type);
    // Functions with variable number of arguments
    static value_type  Sum(const value_type* a_afArg, int a_iArgc);  // sum
    static value_type  Min(const value_type* a_afArg, int a_iArgc);  // minimum
    static value_type  Max(const value_type* a_afArg, int a_iArgc);  // maximum
    // binary operator callbacks
    static value_type  Add(value_type v1, value_type v2);
    static value_type  Sub(value_type v1, value_type v2);
    static value_type  Mul(value_type v1, value_type v2);
    static value_type  Div(value_type v1, value_type v2);
    static value_type  Mod(value_type v1, value_type v2);
    static value_type  Pow(value_type v1, value_type v2);
    static value_type  Shr(value_type v1, value_type v2);
    static value_type  Shl(value_type v1, value_type v2);
    static value_type  LogAnd(value_type v1, value_type v2);
    static value_type  LogOr(value_type v1, value_type v2);
    static value_type  And(value_type v1, value_type v2);
    static value_type  Or(value_type v1, value_type v2);
    static value_type  Xor(value_type v1, value_type v2);
    static value_type  Less(value_type v1, value_type v2);
    static value_type  Greater(value_type v1, value_type v2);
    static value_type  LessEq(value_type v1, value_type v2);
    static value_type  GreaterEq(value_type v1, value_type v2);
    static value_type  Equal(value_type v1, value_type v2);
    static value_type  NotEqual(value_type v1, value_type v2);
    static value_type  Not(value_type v1);

    static int IsHexVal(const char_type* a_szExpr, int *a_iPos, value_type *a_iVal);
    static int IsBinVal(const char_type* a_szExpr, int *a_iPos, value_type *a_iVal);
    static int IsVal   (const char_type* a_szExpr, int *a_iPos, value_type *a_iVal);

    /** \brief A facet class used to change decimal and thousands separator. */
    template<class TChar>
    class change_dec_sep : public std::numpunct<TChar>
    {
    public:
      
      explicit change_dec_sep(char_type cDecSep, char_type cThousandsSep = 0, int nGroup = 3)
        :std::numpunct<TChar>()
        ,m_cDecPoint(cDecSep)
        ,m_cThousandsSep(cThousandsSep)
        ,m_nGroup(nGroup)
      {}
      
    protected:
      
      virtual char_type do_decimal_point() const
      {
        return m_cDecPoint;
      }

      virtual char_type do_thousands_sep() const
      {
        return m_cThousandsSep;
      }

      virtual std::string do_grouping() const 
      { 
        // fix for issue 4: https://code.google.com/p/muparser/issues/detail?id=4
        // courtesy of Jens Bartsch
        // original code:
        //        return std::string(1, (char)m_nGroup); 
        // new code:
        return std::string(1, (char)(m_cThousandsSep > 0 ? m_nGroup : CHAR_MAX));
      }

    private:

      int m_nGroup;
      char_type m_cDecPoint;  
      char_type m_cThousandsSep;
    };

public:
    ParserInt();

    virtual void InitFun();
    virtual void InitOprt();
    virtual void InitConst();
    virtual void InitCharSets();
};

} // namespace mu

#endif

/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef MU_PARSER_TEST_H
#define MU_PARSER_TEST_H

#include <string>
#include <cstdlib>
#include <numeric> // for accumulate
// #include "muParser.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MU_PARSER_H
#define MU_PARSER_H

//--- Standard includes ------------------------------------------------------------------------
#include <vector>

//--- Parser includes --------------------------------------------------------------------------
// #include "muParserBase.h"

// #include "muParserTemplateMagic.h"


/** \file
    \brief Definition of the standard floating point parser.
*/

namespace mu
{
  /** \brief Mathematical expressions parser.
    
    Standard implementation of the mathematical expressions parser. 
    Can be used as a reference implementation for subclassing the parser.

    <small>
    (C) 2011 Ingo Berg<br>
    muparser(at)beltoforion.de
    </small>
  */
  /* final */ class API_EXPORT_CXX Parser : public ParserBase
  {
  public:

    Parser();

    virtual void InitCharSets();
    virtual void InitFun();
    virtual void InitConst();
    virtual void InitOprt();
    virtual void OnDetectVar(string_type *pExpr, int &nStart, int &nEnd);

    value_type Diff(value_type *a_Var, 
                    value_type a_fPos, 
                    value_type a_fEpsilon = 0) const;

  protected:

    // Trigonometric functions
    static value_type  Sin(value_type);
    static value_type  Cos(value_type);
    static value_type  Tan(value_type);
    static value_type  Tan2(value_type, value_type);
    // arcus functions
    static value_type  ASin(value_type);
    static value_type  ACos(value_type);
    static value_type  ATan(value_type);
    static value_type  ATan2(value_type, value_type);

    // hyperbolic functions
    static value_type  Sinh(value_type);
    static value_type  Cosh(value_type);
    static value_type  Tanh(value_type);
    // arcus hyperbolic functions
    static value_type  ASinh(value_type);
    static value_type  ACosh(value_type);
    static value_type  ATanh(value_type);
    // Logarithm functions
    static value_type  Log2(value_type);  // Logarithm Base 2
    static value_type  Log10(value_type); // Logarithm Base 10
    static value_type  Ln(value_type);    // Logarithm Base e (natural logarithm)
    // misc
    static value_type  Exp(value_type);
    static value_type  Abs(value_type);
    static value_type  Sqrt(value_type);
    static value_type  Rint(value_type);
    static value_type  Sign(value_type);

    // Prefix operators
    // !!! Unary Minus is a MUST if you want to use negative signs !!!
    static value_type  UnaryMinus(value_type);
    static value_type  UnaryPlus(value_type);

    // Functions with variable number of arguments
    static value_type Sum(const value_type*, int);  // sum
    static value_type Avg(const value_type*, int);  // mean value
    static value_type Min(const value_type*, int);  // minimum
    static value_type Max(const value_type*, int);  // maximum

    static int IsVal(const char_type* a_szExpr, int *a_iPos, value_type *a_fVal);
  };
} // namespace mu

#endif


// #include "muParserInt.h"
/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef MU_PARSER_INT_H
#define MU_PARSER_INT_H

// #include "muParserBase.h"

#include <vector>


/** \file
    \brief Definition of a parser using integer value.
*/


namespace mu
{

/** \brief Mathematical expressions parser.
  
  This version of the parser handles only integer numbers. It disables the built in operators thus it is 
  slower than muParser. Integer values are stored in the double value_type and converted if needed.
*/
class ParserInt : public ParserBase
{
private:
    static int  Round(value_type v) { return (int)(v + ((v>=0) ? 0.5 : -0.5) ); };
  
    static value_type  Abs(value_type);
    static value_type  Sign(value_type);
    static value_type  Ite(value_type, value_type, value_type);
    // !! The unary Minus is a MUST, otherwise you can't use negative signs !!
    static value_type  UnaryMinus(value_type);
    // Functions with variable number of arguments
    static value_type  Sum(const value_type* a_afArg, int a_iArgc);  // sum
    static value_type  Min(const value_type* a_afArg, int a_iArgc);  // minimum
    static value_type  Max(const value_type* a_afArg, int a_iArgc);  // maximum
    // binary operator callbacks
    static value_type  Add(value_type v1, value_type v2);
    static value_type  Sub(value_type v1, value_type v2);
    static value_type  Mul(value_type v1, value_type v2);
    static value_type  Div(value_type v1, value_type v2);
    static value_type  Mod(value_type v1, value_type v2);
    static value_type  Pow(value_type v1, value_type v2);
    static value_type  Shr(value_type v1, value_type v2);
    static value_type  Shl(value_type v1, value_type v2);
    static value_type  LogAnd(value_type v1, value_type v2);
    static value_type  LogOr(value_type v1, value_type v2);
    static value_type  And(value_type v1, value_type v2);
    static value_type  Or(value_type v1, value_type v2);
    static value_type  Xor(value_type v1, value_type v2);
    static value_type  Less(value_type v1, value_type v2);
    static value_type  Greater(value_type v1, value_type v2);
    static value_type  LessEq(value_type v1, value_type v2);
    static value_type  GreaterEq(value_type v1, value_type v2);
    static value_type  Equal(value_type v1, value_type v2);
    static value_type  NotEqual(value_type v1, value_type v2);
    static value_type  Not(value_type v1);

    static int IsHexVal(const char_type* a_szExpr, int *a_iPos, value_type *a_iVal);
    static int IsBinVal(const char_type* a_szExpr, int *a_iPos, value_type *a_iVal);
    static int IsVal   (const char_type* a_szExpr, int *a_iPos, value_type *a_iVal);

    /** \brief A facet class used to change decimal and thousands separator. */
    template<class TChar>
    class change_dec_sep : public std::numpunct<TChar>
    {
    public:
      
      explicit change_dec_sep(char_type cDecSep, char_type cThousandsSep = 0, int nGroup = 3)
        :std::numpunct<TChar>()
        ,m_cDecPoint(cDecSep)
        ,m_cThousandsSep(cThousandsSep)
        ,m_nGroup(nGroup)
      {}
      
    protected:
      
      virtual char_type do_decimal_point() const
      {
        return m_cDecPoint;
      }

      virtual char_type do_thousands_sep() const
      {
        return m_cThousandsSep;
      }

      virtual std::string do_grouping() const 
      { 
        // fix for issue 4: https://code.google.com/p/muparser/issues/detail?id=4
        // courtesy of Jens Bartsch
        // original code:
        //        return std::string(1, (char)m_nGroup); 
        // new code:
        return std::string(1, (char)(m_cThousandsSep > 0 ? m_nGroup : CHAR_MAX));
      }

    private:

      int m_nGroup;
      char_type m_cDecPoint;  
      char_type m_cThousandsSep;
    };

public:
    ParserInt();

    virtual void InitFun();
    virtual void InitOprt();
    virtual void InitConst();
    virtual void InitCharSets();
};

} // namespace mu

#endif



/** \file
    \brief This file contains the parser test class.
*/

namespace mu
{
  /** \brief Namespace for test cases. */
  namespace Test
  {
    //------------------------------------------------------------------------------
    /** \brief Test cases for unit testing.

      (C) 2004-2011 Ingo Berg
    */
    class API_EXPORT_CXX ParserTester // final
    {
    private:
        static int c_iCount;

        // Multiarg callbacks
        static value_type f1of1(value_type v) { return v;};
      	
        static value_type f1of2(value_type v, value_type  ) {return v;};
        static value_type f2of2(value_type  , value_type v) {return v;};

        static value_type f1of3(value_type v, value_type  , value_type  ) {return v;};
        static value_type f2of3(value_type  , value_type v, value_type  ) {return v;};
        static value_type f3of3(value_type  , value_type  , value_type v) {return v;};
      	
        static value_type f1of4(value_type v, value_type,   value_type  , value_type  ) {return v;}
        static value_type f2of4(value_type  , value_type v, value_type  , value_type  ) {return v;}
        static value_type f3of4(value_type  , value_type,   value_type v, value_type  ) {return v;}
        static value_type f4of4(value_type  , value_type,   value_type  , value_type v) {return v;}

        static value_type f1of5(value_type v, value_type,   value_type  , value_type  , value_type  ) { return v; }
        static value_type f2of5(value_type  , value_type v, value_type  , value_type  , value_type  ) { return v; }
        static value_type f3of5(value_type  , value_type,   value_type v, value_type  , value_type  ) { return v; }
        static value_type f4of5(value_type  , value_type,   value_type  , value_type v, value_type  ) { return v; }
        static value_type f5of5(value_type  , value_type,   value_type  , value_type  , value_type v) { return v; }

        static value_type Min(value_type a_fVal1, value_type a_fVal2) { return (a_fVal1<a_fVal2) ? a_fVal1 : a_fVal2; }
  	    static value_type Max(value_type a_fVal1, value_type a_fVal2) { return (a_fVal1>a_fVal2) ? a_fVal1 : a_fVal2; }

        static value_type plus2(value_type v1) { return v1+2; }
        static value_type times3(value_type v1) { return v1*3; }
        static value_type sqr(value_type v1) { return v1*v1; }
        static value_type sign(value_type v) { return -v; }
        static value_type add(value_type v1, value_type v2) { return v1+v2; }
        static value_type land(value_type v1, value_type v2) { return (int)v1 & (int)v2; }
        

        static value_type FirstArg(const value_type* a_afArg, int a_iArgc)
        {
          if (!a_iArgc)	
            throw mu::Parser::exception_type( _T("too few arguments for function FirstArg.") );

          return  a_afArg[0];
        }

        static value_type LastArg(const value_type* a_afArg, int a_iArgc)
        {
          if (!a_iArgc)	
            throw mu::Parser::exception_type( _T("too few arguments for function LastArg.") );

          return  a_afArg[a_iArgc-1];
        }

        static value_type Sum(const value_type* a_afArg, int a_iArgc)
        { 
          if (!a_iArgc)	
            throw mu::Parser::exception_type( _T("too few arguments for function sum.") );

          value_type fRes=0;
          for (int i=0; i<a_iArgc; ++i) fRes += a_afArg[i];
          return fRes;
        }

        static value_type Rnd(value_type v)
        {
          return (value_type)(1+(v*std::rand()/(RAND_MAX+1.0)));
        }

        static value_type RndWithString(const char_type*)
        {
          return (value_type)( 1 + (1000.0f * std::rand() / (RAND_MAX + 1.0) ) );
        }

        static value_type Ping()
        { 
          return 10; 
        }

        static value_type ValueOf(const char_type*)      
        { 
          return 123; 
        }

        static value_type StrFun1(const char_type* v1)                               
        { 
          int val(0);
          stringstream_type(v1) >> val;
          return (value_type)val;
        }

        static value_type StrFun2(const char_type* v1, value_type v2)                
        { 
          int val(0);
          stringstream_type(v1) >> val;
          return (value_type)(val + v2);
        }
        
        static value_type StrFun3(const char_type* v1, value_type v2, value_type v3) 
        { 
          int val(0);
          stringstream_type(v1) >> val;
          return val + v2 + v3;
        }

        static value_type StrToFloat(const char_type* a_szMsg)
        {
          value_type val(0);
          stringstream_type(a_szMsg) >> val;
          return val;
        }

        // postfix operator callback
        static value_type Mega(value_type a_fVal)  { return a_fVal * (value_type)1e6; }
        static value_type Micro(value_type a_fVal) { return a_fVal * (value_type)1e-6; }
        static value_type Milli(value_type a_fVal) { return a_fVal / (value_type)1e3; }

        // Custom value recognition
        static int IsHexVal(const char_type *a_szExpr, int *a_iPos, value_type *a_fVal);

        int TestNames();
        int TestSyntax();
        int TestMultiArg();
        int TestPostFix();
        int TestExpression();
        int TestInfixOprt();
        int TestBinOprt();
        int TestVarConst();
        int TestInterface();
        int TestException();
        int TestStrArg();
        int TestIfThenElse();
        int TestBulkMode();

        void Abort() const;

    public:
        typedef int (ParserTester::*testfun_type)();

	      ParserTester();
	      void Run();

    private:
        std::vector<testfun_type> m_vTestFun;
	      void AddTest(testfun_type a_pFun);

        // Test Double Parser
        int EqnTest(const string_type& a_str, double a_fRes, bool a_fPass);
        int EqnTestWithVarChange(const string_type& a_str, 
                                 double a_fRes1, 
                                 double a_fVar1, 
                                 double a_fRes2, 
                                 double a_fVar2);
        int ThrowTest(const string_type& a_str, int a_iErrc, bool a_bFail = true);

        // Test Int Parser
        int EqnTestInt(const string_type& a_str, double a_fRes, bool a_fPass);

        // Test Bulkmode
        int EqnTestBulk(const string_type& a_str, double a_fRes[4], bool a_fPass);
    };
  } // namespace Test
} // namespace mu

#endif


/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2011 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MU_PARSER_DLL_H
#define MU_PARSER_DLL_H

// #include "muParserFixes.h"


#ifdef __cplusplus
extern "C"
{
#endif

/** \file 
    \brief This file contains the DLL interface of muparser.
*/

// Basic types
typedef void*  muParserHandle_t;    // parser handle

#ifndef _UNICODE
    typedef char   muChar_t;            // character type
#else
    typedef wchar_t   muChar_t;            // character type
#endif

typedef int    muBool_t;            // boolean type
typedef int    muInt_t;             // integer type 
typedef double muFloat_t;           // floating point type

// function types for calculation
typedef muFloat_t (*muFun0_t )(); 
typedef muFloat_t (*muFun1_t )(muFloat_t); 
typedef muFloat_t (*muFun2_t )(muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun3_t )(muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun4_t )(muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun5_t )(muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun6_t )(muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun7_t )(muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun8_t )(muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun9_t )(muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun10_t)(muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 

// Function prototypes for bulkmode functions
typedef muFloat_t (*muBulkFun0_t )(int, int); 
typedef muFloat_t (*muBulkFun1_t )(int, int, muFloat_t); 
typedef muFloat_t (*muBulkFun2_t )(int, int, muFloat_t, muFloat_t); 
typedef muFloat_t (*muBulkFun3_t )(int, int, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muBulkFun4_t )(int, int, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muBulkFun5_t )(int, int, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muBulkFun6_t )(int, int, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muBulkFun7_t )(int, int, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muBulkFun8_t )(int, int, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muBulkFun9_t )(int, int, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muBulkFun10_t)(int, int, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 

typedef muFloat_t (*muMultFun_t)(const muFloat_t*, muInt_t);
typedef muFloat_t (*muStrFun1_t)(const muChar_t*);
typedef muFloat_t (*muStrFun2_t)(const muChar_t*, muFloat_t);
typedef muFloat_t (*muStrFun3_t)(const muChar_t*, muFloat_t, muFloat_t);

// Functions for parser management
typedef void (*muErrorHandler_t)(muParserHandle_t a_hParser);           // [optional] callback to an error handler
typedef muFloat_t* (*muFacFun_t)(const muChar_t*, void*);               // [optional] callback for creating new variables
typedef muInt_t (*muIdentFun_t)(const muChar_t*, muInt_t*, muFloat_t*); // [optional] value identification callbacks

//-----------------------------------------------------------------------------------------------------
// Constants
static const int muOPRT_ASCT_LEFT  = 0;
static const int muOPRT_ASCT_RIGHT = 1;

static const int muBASETYPE_FLOAT  = 0;
static const int muBASETYPE_INT    = 1;

//-----------------------------------------------------------------------------------------------------
//
//
// muParser C compatible bindings
//
//
//-----------------------------------------------------------------------------------------------------


// Basic operations / initialization  
API_EXPORT(muParserHandle_t) mupCreate(int nBaseType);
API_EXPORT(void) mupRelease(muParserHandle_t a_hParser);
API_EXPORT(const muChar_t*) mupGetExpr(muParserHandle_t a_hParser);
API_EXPORT(void) mupSetExpr(muParserHandle_t a_hParser, const muChar_t *a_szExpr);
API_EXPORT(void) mupSetVarFactory(muParserHandle_t a_hParser, muFacFun_t a_pFactory, void* pUserData);
API_EXPORT(const muChar_t*) mupGetVersion(muParserHandle_t a_hParser);
API_EXPORT(muFloat_t) mupEval(muParserHandle_t a_hParser);
API_EXPORT(muFloat_t*) mupEvalMulti(muParserHandle_t a_hParser, int *nNum);
API_EXPORT(void) mupEvalBulk(muParserHandle_t a_hParser, muFloat_t *a_fResult, int nSize);

// Defining callbacks / variables / constants
API_EXPORT(void) mupDefineFun0(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun0_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun1(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun1_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun2(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun2_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun3(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun3_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun4(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun4_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun5(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun5_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun6(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun6_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun7(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun7_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun8(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun8_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun9(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun9_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun10(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun10_t a_pFun, muBool_t a_bOptimize);

// Defining bulkmode functions
API_EXPORT(void) mupDefineBulkFun0(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun0_t a_pFun);
API_EXPORT(void) mupDefineBulkFun1(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun1_t a_pFun);
API_EXPORT(void) mupDefineBulkFun2(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun2_t a_pFun);
API_EXPORT(void) mupDefineBulkFun3(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun3_t a_pFun);
API_EXPORT(void) mupDefineBulkFun4(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun4_t a_pFun);
API_EXPORT(void) mupDefineBulkFun5(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun5_t a_pFun);
API_EXPORT(void) mupDefineBulkFun6(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun6_t a_pFun);
API_EXPORT(void) mupDefineBulkFun7(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun7_t a_pFun);
API_EXPORT(void) mupDefineBulkFun8(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun8_t a_pFun);
API_EXPORT(void) mupDefineBulkFun9(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun9_t a_pFun);
API_EXPORT(void) mupDefineBulkFun10(muParserHandle_t a_hParser, const muChar_t *a_szName, muBulkFun10_t a_pFun);

// string functions
API_EXPORT(void) mupDefineStrFun1(muParserHandle_t a_hParser, const muChar_t *a_szName, muStrFun1_t a_pFun);
API_EXPORT(void) mupDefineStrFun2(muParserHandle_t a_hParser, const muChar_t *a_szName, muStrFun2_t a_pFun);
API_EXPORT(void) mupDefineStrFun3(muParserHandle_t a_hParser, const muChar_t *a_szName, muStrFun3_t a_pFun);

API_EXPORT(void) mupDefineMultFun( muParserHandle_t a_hParser, 
                                   const muChar_t* a_szName, 
                                   muMultFun_t a_pFun, 
                                   muBool_t a_bOptimize);

API_EXPORT(void) mupDefineOprt( muParserHandle_t a_hParser, 
                                const muChar_t* a_szName, 
                                muFun2_t a_pFun, 
                                muInt_t a_nPrec, 
                                muInt_t a_nOprtAsct,
                                muBool_t a_bOptimize);

API_EXPORT(void) mupDefineConst( muParserHandle_t a_hParser, 
                                 const muChar_t* a_szName, 
                                 muFloat_t a_fVal );

API_EXPORT(void) mupDefineStrConst( muParserHandle_t a_hParser, 
                                    const muChar_t* a_szName, 
                                    const muChar_t *a_sVal );

API_EXPORT(void) mupDefineVar( muParserHandle_t a_hParser, 
                               const muChar_t* a_szName, 
                               muFloat_t *a_fVar);

API_EXPORT(void) mupDefineBulkVar( muParserHandle_t a_hParser, 
                               const muChar_t* a_szName, 
                               muFloat_t *a_fVar);

API_EXPORT(void) mupDefinePostfixOprt( muParserHandle_t a_hParser, 
                                       const muChar_t* a_szName, 
                                       muFun1_t a_pOprt, 
                                       muBool_t a_bOptimize);


API_EXPORT(void) mupDefineInfixOprt( muParserHandle_t a_hParser, 
                                     const muChar_t* a_szName, 
                                     muFun1_t a_pOprt, 
                                     muBool_t a_bOptimize);

// Define character sets for identifiers
API_EXPORT(void) mupDefineNameChars(muParserHandle_t a_hParser, const muChar_t* a_szCharset);
API_EXPORT(void) mupDefineOprtChars(muParserHandle_t a_hParser, const muChar_t* a_szCharset);
API_EXPORT(void) mupDefineInfixOprtChars(muParserHandle_t a_hParser, const muChar_t* a_szCharset);

// Remove all / single variables
API_EXPORT(void) mupRemoveVar(muParserHandle_t a_hParser, const muChar_t* a_szName);
API_EXPORT(void) mupClearVar(muParserHandle_t a_hParser);
API_EXPORT(void) mupClearConst(muParserHandle_t a_hParser);
API_EXPORT(void) mupClearOprt(muParserHandle_t a_hParser);
API_EXPORT(void) mupClearFun(muParserHandle_t a_hParser);

// Querying variables / expression variables / constants
API_EXPORT(int) mupGetExprVarNum(muParserHandle_t a_hParser);
API_EXPORT(int) mupGetVarNum(muParserHandle_t a_hParser);
API_EXPORT(int) mupGetConstNum(muParserHandle_t a_hParser);
API_EXPORT(void) mupGetExprVar(muParserHandle_t a_hParser, unsigned a_iVar, const muChar_t** a_pszName, muFloat_t** a_pVar);
API_EXPORT(void) mupGetVar(muParserHandle_t a_hParser, unsigned a_iVar, const muChar_t** a_pszName, muFloat_t** a_pVar);
API_EXPORT(void) mupGetConst(muParserHandle_t a_hParser, unsigned a_iVar, const muChar_t** a_pszName, muFloat_t* a_pVar);
API_EXPORT(void) mupSetArgSep(muParserHandle_t a_hParser, const muChar_t cArgSep);
API_EXPORT(void) mupSetDecSep(muParserHandle_t a_hParser, const muChar_t cArgSep);
API_EXPORT(void) mupSetThousandsSep(muParserHandle_t a_hParser, const muChar_t cArgSep);
API_EXPORT(void) mupResetLocale(muParserHandle_t a_hParser);

// Add value recognition callbacks
API_EXPORT(void) mupAddValIdent(muParserHandle_t a_hParser, muIdentFun_t);

// Error handling
API_EXPORT(muBool_t) mupError(muParserHandle_t a_hParser);
API_EXPORT(void) mupErrorReset(muParserHandle_t a_hParser);
API_EXPORT(void) mupSetErrorHandler(muParserHandle_t a_hParser, muErrorHandler_t a_pErrHandler);
API_EXPORT(const muChar_t*) mupGetErrorMsg(muParserHandle_t a_hParser);
API_EXPORT(muInt_t) mupGetErrorCode(muParserHandle_t a_hParser);
API_EXPORT(muInt_t) mupGetErrorPos(muParserHandle_t a_hParser);
API_EXPORT(const muChar_t*) mupGetErrorToken(muParserHandle_t a_hParser);
//API_EXPORT(const muChar_t*) mupGetErrorExpr(muParserHandle_t a_hParser);

// This is used for .NET only. It creates a new variable allowing the dll to
// manage the variable rather than the .NET garbage collector.
API_EXPORT(muFloat_t*) mupCreateVar();
API_EXPORT(void) mupReleaseVar(muFloat_t*);

#ifdef __cplusplus
}
#endif

#endif // include guard
