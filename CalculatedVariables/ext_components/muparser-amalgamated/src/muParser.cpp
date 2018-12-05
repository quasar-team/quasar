#include <muParser.h>
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

//amalgamated header include by Piotrs script #include "muParserBase.h"
//amalgamated header include by Piotrs script #include "muParserTemplateMagic.h"

//--- Standard includes ------------------------------------------------------------------------
#include <cassert>
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <deque>
#include <sstream>
#include <locale>

#ifdef MUP_USE_OPENMP
  #include <omp.h>
#endif

using namespace std;

/** \file
    \brief This file contains the basic implementation of the muparser engine.
*/

namespace mu
{
  std::locale ParserBase::s_locale = std::locale(std::locale::classic(), new change_dec_sep<char_type>('.'));

  bool ParserBase::g_DbgDumpCmdCode = false;
  bool ParserBase::g_DbgDumpStack = false;

  //------------------------------------------------------------------------------
  /** \brief Identifiers for built in binary operators. 

      When defining custom binary operators with #AddOprt(...) make sure not to choose 
      names conflicting with these definitions. 
  */
  const char_type* ParserBase::c_DefaultOprt[] = 
  { 
    _T("<="), _T(">="),  _T("!="), 
    _T("=="), _T("<"),   _T(">"), 
    _T("+"),  _T("-"),   _T("*"), 
    _T("/"),  _T("^"),   _T("&&"), 
    _T("||"), _T("="),   _T("("),  
    _T(")"),   _T("?"),  _T(":"), 0 
  };

  const int ParserBase::s_MaxNumOpenMPThreads = 16;

  //------------------------------------------------------------------------------
  /** \brief Constructor.
      \param a_szFormula the formula to interpret.
      \throw ParserException if a_szFormula is null.
  */
  ParserBase::ParserBase()
    :m_pParseFormula(&ParserBase::ParseString)
    ,m_vRPN()
    ,m_vStringBuf()
    ,m_pTokenReader()
    ,m_FunDef()
    ,m_PostOprtDef()
    ,m_InfixOprtDef()
    ,m_OprtDef()
    ,m_ConstDef()
    ,m_StrVarDef()
    ,m_VarDef()
    ,m_bBuiltInOp(true)
    ,m_sNameChars()
    ,m_sOprtChars()
    ,m_sInfixOprtChars()
    ,m_nIfElseCounter(0)
    ,m_vStackBuffer()
    ,m_nFinalResultIdx(0)
  {
    InitTokenReader();
  }

  //---------------------------------------------------------------------------
  /** \brief Copy constructor. 

    The parser can be safely copy constructed but the bytecode is reset during
    copy construction.
  */
  ParserBase::ParserBase(const ParserBase &a_Parser)
    :m_pParseFormula(&ParserBase::ParseString)
    ,m_vRPN()
    ,m_vStringBuf()
    ,m_pTokenReader()
    ,m_FunDef()
    ,m_PostOprtDef()
    ,m_InfixOprtDef()
    ,m_OprtDef()
    ,m_ConstDef()
    ,m_StrVarDef()
    ,m_VarDef()
    ,m_bBuiltInOp(true)
    ,m_sNameChars()
    ,m_sOprtChars()
    ,m_sInfixOprtChars()
    ,m_nIfElseCounter(0)
  {
    m_pTokenReader.reset(new token_reader_type(this));
    Assign(a_Parser);
  }

  //---------------------------------------------------------------------------
  ParserBase::~ParserBase()
  {}

  //---------------------------------------------------------------------------
  /** \brief Assignment operator. 

    Implemented by calling Assign(a_Parser). Self assignment is suppressed.
    \param a_Parser Object to copy to this.
    \return *this
    \throw nothrow
  */
  ParserBase& ParserBase::operator=(const ParserBase &a_Parser)
  {
    Assign(a_Parser);
    return *this;
  }

  //---------------------------------------------------------------------------
  /** \brief Copy state of a parser object to this. 

    Clears Variables and Functions of this parser.
    Copies the states of all internal variables.
    Resets parse function to string parse mode.

    \param a_Parser the source object.
  */
  void ParserBase::Assign(const ParserBase &a_Parser)
  {
    if (&a_Parser==this)
      return;

    // Don't copy bytecode instead cause the parser to create new bytecode
    // by resetting the parse function.
    ReInit();

    m_ConstDef        = a_Parser.m_ConstDef;         // Copy user define constants
    m_VarDef          = a_Parser.m_VarDef;           // Copy user defined variables
    m_bBuiltInOp      = a_Parser.m_bBuiltInOp;
    m_vStringBuf      = a_Parser.m_vStringBuf;
    m_vStackBuffer    = a_Parser.m_vStackBuffer;
    m_nFinalResultIdx = a_Parser.m_nFinalResultIdx;
    m_StrVarDef       = a_Parser.m_StrVarDef;
    m_vStringVarBuf   = a_Parser.m_vStringVarBuf;
    m_nIfElseCounter  = a_Parser.m_nIfElseCounter;
    m_pTokenReader.reset(a_Parser.m_pTokenReader->Clone(this));

    // Copy function and operator callbacks
    m_FunDef = a_Parser.m_FunDef;             // Copy function definitions
    m_PostOprtDef = a_Parser.m_PostOprtDef;   // post value unary operators
    m_InfixOprtDef = a_Parser.m_InfixOprtDef; // unary operators for infix notation
    m_OprtDef = a_Parser.m_OprtDef;           // binary operators

    m_sNameChars = a_Parser.m_sNameChars;
    m_sOprtChars = a_Parser.m_sOprtChars;
    m_sInfixOprtChars = a_Parser.m_sInfixOprtChars;
  }

  //---------------------------------------------------------------------------
  /** \brief Set the decimal separator.
      \param cDecSep Decimal separator as a character value.
      \sa SetThousandsSep

      By default muparser uses the "C" locale. The decimal separator of this
      locale is overwritten by the one provided here.
  */
  void ParserBase::SetDecSep(char_type cDecSep)
  {
    char_type cThousandsSep = std::use_facet< change_dec_sep<char_type> >(s_locale).thousands_sep();
    s_locale = std::locale(std::locale("C"), new change_dec_sep<char_type>(cDecSep, cThousandsSep));
  }
  
  //---------------------------------------------------------------------------
  /** \brief Sets the thousands operator. 
      \param cThousandsSep The thousands separator as a character
      \sa SetDecSep

      By default muparser uses the "C" locale. The thousands separator of this
      locale is overwritten by the one provided here.
  */
  void ParserBase::SetThousandsSep(char_type cThousandsSep)
  {
    char_type cDecSep = std::use_facet< change_dec_sep<char_type> >(s_locale).decimal_point();
    s_locale = std::locale(std::locale("C"), new change_dec_sep<char_type>(cDecSep, cThousandsSep));
  }

  //---------------------------------------------------------------------------
  /** \brief Resets the locale. 

    The default locale used "." as decimal separator, no thousands separator and
    "," as function argument separator.
  */
  void ParserBase::ResetLocale()
  {
    s_locale = std::locale(std::locale("C"), new change_dec_sep<char_type>('.'));
    SetArgSep(',');
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize the token reader. 

    Create new token reader object and submit pointers to function, operator,
    constant and variable definitions.

    \post m_pTokenReader.get()!=0
    \throw nothrow
  */
  void ParserBase::InitTokenReader()
  {
    m_pTokenReader.reset(new token_reader_type(this));
  }

  //---------------------------------------------------------------------------
  /** \brief Reset parser to string parsing mode and clear internal buffers.

      Clear bytecode, reset the token reader.
      \throw nothrow
  */
  void ParserBase::ReInit() const
  {
    m_pParseFormula = &ParserBase::ParseString;
    m_vStringBuf.clear();
    m_vRPN.clear();
    m_pTokenReader->ReInit();
    m_nIfElseCounter = 0;
  }

  //---------------------------------------------------------------------------
  void ParserBase::OnDetectVar(string_type * /*pExpr*/, int & /*nStart*/, int & /*nEnd*/)
  {}

  //---------------------------------------------------------------------------
  /** \brief Returns the version of muparser. 
      \param eInfo A flag indicating whether the full version info should be 
                   returned or not.

    Format is as follows: "MAJOR.MINOR (COMPILER_FLAGS)" The COMPILER_FLAGS
    are returned only if eInfo==pviFULL.
  */
  string_type ParserBase::GetVersion(EParserVersionInfo eInfo) const
  {
    stringstream_type ss;

    ss << MUP_VERSION;

    if (eInfo==pviFULL)
    {
      ss << _T(" (") << MUP_VERSION_DATE;
      ss << std::dec << _T("; ") << sizeof(void*)*8 << _T("BIT");

#ifdef _DEBUG
      ss << _T("; DEBUG");
#else 
      ss << _T("; RELEASE");
#endif

#ifdef _UNICODE
      ss << _T("; UNICODE");
#else
  #ifdef _MBCS
      ss << _T("; MBCS");
  #else
      ss << _T("; ASCII");
  #endif
#endif

#ifdef MUP_USE_OPENMP
      ss << _T("; OPENMP");
//#else
//      ss << _T("; NO_OPENMP");
#endif

#if defined(MUP_MATH_EXCEPTIONS)
      ss << _T("; MATHEXC");
//#else
//      ss << _T("; NO_MATHEXC");
#endif

      ss << _T(")");
    }

    return ss.str();
  }

  //---------------------------------------------------------------------------
  /** \brief Add a value parsing function. 
      
      When parsing an expression muParser tries to detect values in the expression
      string using different valident callbacks. Thus it's possible to parse
      for hex values, binary values and floating point values. 
  */
  void ParserBase::AddValIdent(identfun_type a_pCallback)
  {
    m_pTokenReader->AddValIdent(a_pCallback);
  }

  //---------------------------------------------------------------------------
  /** \brief Set a function that can create variable pointer for unknown expression variables. 
      \param a_pFactory A pointer to the variable factory.
      \param pUserData A user defined context pointer.
  */
  void ParserBase::SetVarFactory(facfun_type a_pFactory, void *pUserData)
  {
    m_pTokenReader->SetVarCreator(a_pFactory, pUserData);  
  }

  //---------------------------------------------------------------------------
  /** \brief Add a function or operator callback to the parser. */
  void ParserBase::AddCallback( const string_type &a_strName,
                                const ParserCallback &a_Callback, 
                                funmap_type &a_Storage,
                                const char_type *a_szCharSet )
  {
    if (a_Callback.GetAddr()==0)
        Error(ecINVALID_FUN_PTR);

    const funmap_type *pFunMap = &a_Storage;

    // Check for conflicting operator or function names
    if ( pFunMap!=&m_FunDef && m_FunDef.find(a_strName)!=m_FunDef.end() )
      Error(ecNAME_CONFLICT, -1, a_strName);

    if ( pFunMap!=&m_PostOprtDef && m_PostOprtDef.find(a_strName)!=m_PostOprtDef.end() )
      Error(ecNAME_CONFLICT, -1, a_strName);

    if ( pFunMap!=&m_InfixOprtDef && pFunMap!=&m_OprtDef && m_InfixOprtDef.find(a_strName)!=m_InfixOprtDef.end() )
      Error(ecNAME_CONFLICT, -1, a_strName);

    if ( pFunMap!=&m_InfixOprtDef && pFunMap!=&m_OprtDef && m_OprtDef.find(a_strName)!=m_OprtDef.end() )
      Error(ecNAME_CONFLICT, -1, a_strName);

    CheckOprt(a_strName, a_Callback, a_szCharSet);
    a_Storage[a_strName] = a_Callback;
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Check if a name contains invalid characters. 

      \throw ParserException if the name contains invalid characters.
  */
  void ParserBase::CheckOprt(const string_type &a_sName,
                             const ParserCallback &a_Callback,
                             const string_type &a_szCharSet) const
  {
    if ( !a_sName.length() ||
        (a_sName.find_first_not_of(a_szCharSet)!=string_type::npos) ||
        (a_sName[0]>='0' && a_sName[0]<='9'))
    {
      switch(a_Callback.GetCode())
      {
      case cmOPRT_POSTFIX: Error(ecINVALID_POSTFIX_IDENT, -1, a_sName); break;
      case cmOPRT_INFIX:   Error(ecINVALID_INFIX_IDENT, -1, a_sName); break;
      default:             Error(ecINVALID_NAME, -1, a_sName);
      }
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Check if a name contains invalid characters. 

      \throw ParserException if the name contains invalid characters.
  */
  void ParserBase::CheckName(const string_type &a_sName,
                             const string_type &a_szCharSet) const
  {
    if ( !a_sName.length() ||
        (a_sName.find_first_not_of(a_szCharSet)!=string_type::npos) ||
        (a_sName[0]>='0' && a_sName[0]<='9'))
    {
      Error(ecINVALID_NAME);
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Set the formula. 
      \param a_strFormula Formula as string_type
      \throw ParserException in case of syntax errors.

      Triggers first time calculation thus the creation of the bytecode and
      scanning of used variables.
  */
  void ParserBase::SetExpr(const string_type &a_sExpr)
  {
    // Check locale compatibility
    if (m_pTokenReader->GetArgSep()==std::use_facet<numpunct<char_type> >(s_locale).decimal_point())
      Error(ecLOCALE);

    // <ibg> 20060222: Bugfix for Borland-Kylix:
    // adding a space to the expression will keep Borlands KYLIX from going wild
    // when calling tellg on a stringstream created from the expression after 
    // reading a value at the end of an expression. (mu::Parser::IsVal function)
    // (tellg returns -1 otherwise causing the parser to ignore the value)
    string_type sBuf(a_sExpr + _T(" ") );
    m_pTokenReader->SetFormula(sBuf);
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Get the default symbols used for the built in operators. 
      \sa c_DefaultOprt
  */
  const char_type** ParserBase::GetOprtDef() const
  {
    return (const char_type **)(&c_DefaultOprt[0]);
  }

  //---------------------------------------------------------------------------
  /** \brief Define the set of valid characters to be used in names of
             functions, variables, constants.
  */
  void ParserBase::DefineNameChars(const char_type *a_szCharset)
  {
    m_sNameChars = a_szCharset;
  }

  //---------------------------------------------------------------------------
  /** \brief Define the set of valid characters to be used in names of
             binary operators and postfix operators.
  */
  void ParserBase::DefineOprtChars(const char_type *a_szCharset)
  {
    m_sOprtChars = a_szCharset;
  }

  //---------------------------------------------------------------------------
  /** \brief Define the set of valid characters to be used in names of
             infix operators.
  */
  void ParserBase::DefineInfixOprtChars(const char_type *a_szCharset)
  {
    m_sInfixOprtChars = a_szCharset;
  }

  //---------------------------------------------------------------------------
  /** \brief Virtual function that defines the characters allowed in name identifiers. 
      \sa #ValidOprtChars, #ValidPrefixOprtChars
  */ 
  const char_type* ParserBase::ValidNameChars() const
  {
    assert(m_sNameChars.size());
    return m_sNameChars.c_str();
  }

  //---------------------------------------------------------------------------
  /** \brief Virtual function that defines the characters allowed in operator definitions. 
      \sa #ValidNameChars, #ValidPrefixOprtChars
  */
  const char_type* ParserBase::ValidOprtChars() const
  {
    assert(m_sOprtChars.size());
    return m_sOprtChars.c_str();
  }

  //---------------------------------------------------------------------------
  /** \brief Virtual function that defines the characters allowed in infix operator definitions.
      \sa #ValidNameChars, #ValidOprtChars
  */
  const char_type* ParserBase::ValidInfixOprtChars() const
  {
    assert(m_sInfixOprtChars.size());
    return m_sInfixOprtChars.c_str();
  }

  //---------------------------------------------------------------------------
  /** \brief Add a user defined operator. 
      \post Will reset the Parser to string parsing mode.
  */
  void ParserBase::DefinePostfixOprt(const string_type &a_sName, 
                                     fun_type1 a_pFun,
                                     bool a_bAllowOpt)
  {
    AddCallback(a_sName, 
                ParserCallback(a_pFun, a_bAllowOpt, prPOSTFIX, cmOPRT_POSTFIX),
                m_PostOprtDef, 
                ValidOprtChars() );
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize user defined functions. 
   
    Calls the virtual functions InitFun(), InitConst() and InitOprt().
  */
  void ParserBase::Init()
  {
    InitCharSets();
    InitFun();
    InitConst();
    InitOprt();
  }

  //---------------------------------------------------------------------------
  /** \brief Add a user defined operator. 
      \post Will reset the Parser to string parsing mode.
      \param [in] a_sName  operator Identifier 
      \param [in] a_pFun  Operator callback function
      \param [in] a_iPrec  Operator Precedence (default=prSIGN)
      \param [in] a_bAllowOpt  True if operator is volatile (default=false)
      \sa EPrec
  */
  void ParserBase::DefineInfixOprt(const string_type &a_sName, 
                                  fun_type1 a_pFun, 
                                  int a_iPrec, 
                                  bool a_bAllowOpt)
  {
    AddCallback(a_sName, 
                ParserCallback(a_pFun, a_bAllowOpt, a_iPrec, cmOPRT_INFIX), 
                m_InfixOprtDef, 
                ValidInfixOprtChars() );
  }


  //---------------------------------------------------------------------------
  /** \brief Define a binary operator. 
      \param [in] a_sName The identifier of the operator.
      \param [in] a_pFun Pointer to the callback function.
      \param [in] a_iPrec Precedence of the operator.
      \param [in] a_eAssociativity The associativity of the operator.
      \param [in] a_bAllowOpt If this is true the operator may be optimized away.
      
      Adds a new Binary operator the the parser instance. 
  */
  void ParserBase::DefineOprt( const string_type &a_sName, 
                               fun_type2 a_pFun, 
                               unsigned a_iPrec, 
                               EOprtAssociativity a_eAssociativity,
                               bool a_bAllowOpt )
  {
    // Check for conflicts with built in operator names
    for (int i=0; m_bBuiltInOp && i<cmENDIF; ++i)
      if (a_sName == string_type(c_DefaultOprt[i]))
        Error(ecBUILTIN_OVERLOAD, -1, a_sName);

    AddCallback(a_sName, 
                ParserCallback(a_pFun, a_bAllowOpt, a_iPrec, a_eAssociativity), 
                m_OprtDef, 
                ValidOprtChars() );
  }

  //---------------------------------------------------------------------------
  /** \brief Define a new string constant.
      \param [in] a_strName The name of the constant.
      \param [in] a_strVal the value of the constant. 
  */
  void ParserBase::DefineStrConst(const string_type &a_strName, const string_type &a_strVal)
  {
    // Test if a constant with that names already exists
    if (m_StrVarDef.find(a_strName)!=m_StrVarDef.end())
      Error(ecNAME_CONFLICT);

    CheckName(a_strName, ValidNameChars());
    
    m_vStringVarBuf.push_back(a_strVal);                // Store variable string in internal buffer
    m_StrVarDef[a_strName] = m_vStringVarBuf.size()-1;  // bind buffer index to variable name

    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Add a user defined variable. 
      \param [in] a_sName the variable name
      \param [in] a_pVar A pointer to the variable value.
      \post Will reset the Parser to string parsing mode.
      \throw ParserException in case the name contains invalid signs or a_pVar is NULL.
  */
  void ParserBase::DefineVar(const string_type &a_sName, value_type *a_pVar)
  {
    if (a_pVar==0)
      Error(ecINVALID_VAR_PTR);

    // Test if a constant with that names already exists
    if (m_ConstDef.find(a_sName)!=m_ConstDef.end())
      Error(ecNAME_CONFLICT);

    CheckName(a_sName, ValidNameChars());
    m_VarDef[a_sName] = a_pVar;
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Add a user defined constant. 
      \param [in] a_sName The name of the constant.
      \param [in] a_fVal the value of the constant.
      \post Will reset the Parser to string parsing mode.
      \throw ParserException in case the name contains invalid signs.
  */
  void ParserBase::DefineConst(const string_type &a_sName, value_type a_fVal)
  {
    CheckName(a_sName, ValidNameChars());
    m_ConstDef[a_sName] = a_fVal;
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Get operator priority.
      \throw ParserException if a_Oprt is no operator code
  */
  int ParserBase::GetOprtPrecedence(const token_type &a_Tok) const
  {
    switch (a_Tok.GetCode())
    {
    // built in operators
    case cmEND:      return -5;
    case cmARG_SEP:  return -4;
    case cmASSIGN:   return -1;               
    case cmELSE:
    case cmIF:       return  0;
    case cmLAND:     return  prLAND;
    case cmLOR:      return  prLOR;
    case cmLT:
    case cmGT:
    case cmLE:
    case cmGE:
    case cmNEQ:
    case cmEQ:       return  prCMP; 
    case cmADD:
    case cmSUB:      return  prADD_SUB;
    case cmMUL:
    case cmDIV:      return  prMUL_DIV;
    case cmPOW:      return  prPOW;

    // user defined binary operators
    case cmOPRT_INFIX: 
    case cmOPRT_BIN: return a_Tok.GetPri();
    default:  Error(ecINTERNAL_ERROR, 5);
              return 999;
    }  
  }

  //---------------------------------------------------------------------------
  /** \brief Get operator priority.
      \throw ParserException if a_Oprt is no operator code
  */
  EOprtAssociativity ParserBase::GetOprtAssociativity(const token_type &a_Tok) const
  {
    switch (a_Tok.GetCode())
    {
    case cmASSIGN:
    case cmLAND:
    case cmLOR:
    case cmLT:
    case cmGT:
    case cmLE:
    case cmGE:
    case cmNEQ:
    case cmEQ: 
    case cmADD:
    case cmSUB:
    case cmMUL:
    case cmDIV:      return oaLEFT;
    case cmPOW:      return oaRIGHT;
    case cmOPRT_BIN: return a_Tok.GetAssociativity();
    default:         return oaNONE;
    }  
  }

  //---------------------------------------------------------------------------
  /** \brief Return a map containing the used variables only. */
  const varmap_type& ParserBase::GetUsedVar() const
  {
    try
    {
      m_pTokenReader->IgnoreUndefVar(true);
      CreateRPN(); // try to create bytecode, but don't use it for any further calculations since it
                   // may contain references to nonexisting variables.
      m_pParseFormula = &ParserBase::ParseString;
      m_pTokenReader->IgnoreUndefVar(false);
    }
    catch(exception_type & /*e*/)
    {
      // Make sure to stay in string parse mode, don't call ReInit()
      // because it deletes the array with the used variables
      m_pParseFormula = &ParserBase::ParseString;
      m_pTokenReader->IgnoreUndefVar(false);
      throw;
    }
    
    return m_pTokenReader->GetUsedVar();
  }

  //---------------------------------------------------------------------------
  /** \brief Return a map containing the used variables only. */
  const varmap_type& ParserBase::GetVar() const
  {
    return m_VarDef;
  }

  //---------------------------------------------------------------------------
  /** \brief Return a map containing all parser constants. */
  const valmap_type& ParserBase::GetConst() const
  {
    return m_ConstDef;
  }

  //---------------------------------------------------------------------------
  /** \brief Return prototypes of all parser functions.
      \return #m_FunDef
      \sa FunProt
      \throw nothrow
      
      The return type is a map of the public type #funmap_type containing the prototype
      definitions for all numerical parser functions. String functions are not part of 
      this map. The Prototype definition is encapsulated in objects of the class FunProt
      one per parser function each associated with function names via a map construct.
  */
  const funmap_type& ParserBase::GetFunDef() const
  {
    return m_FunDef;
  }

  //---------------------------------------------------------------------------
  /** \brief Retrieve the formula. */
  const string_type& ParserBase::GetExpr() const
  {
    return m_pTokenReader->GetExpr();
  }

  //---------------------------------------------------------------------------
  /** \brief Execute a function that takes a single string argument.
      \param a_FunTok Function token.
      \throw exception_type If the function token is not a string function
  */
  ParserBase::token_type ParserBase::ApplyStrFunc(const token_type &a_FunTok,
                                                  const std::vector<token_type> &a_vArg) const
  {
    if (a_vArg.back().GetCode()!=cmSTRING)
      Error(ecSTRING_EXPECTED, m_pTokenReader->GetPos(), a_FunTok.GetAsString());

    token_type  valTok;
    generic_fun_type pFunc = a_FunTok.GetFuncAddr();
    assert(pFunc);

    try
    {
      // Check function arguments; write dummy value into valtok to represent the result
      switch(a_FunTok.GetArgCount())
      {
      case 0: valTok.SetVal(1); a_vArg[0].GetAsString();  break;
      case 1: valTok.SetVal(1); a_vArg[1].GetAsString();  a_vArg[0].GetVal();  break;
      case 2: valTok.SetVal(1); a_vArg[2].GetAsString();  a_vArg[1].GetVal();  a_vArg[0].GetVal();  break;
      default: Error(ecINTERNAL_ERROR);
      }
    }
    catch(ParserError& )
    {
      Error(ecVAL_EXPECTED, m_pTokenReader->GetPos(), a_FunTok.GetAsString());
    }

    // string functions won't be optimized
    m_vRPN.AddStrFun(pFunc, a_FunTok.GetArgCount(), a_vArg.back().GetIdx());
    
    // Push dummy value representing the function result to the stack
    return valTok;
  }

  //---------------------------------------------------------------------------
  /** \brief Apply a function token. 
      \param iArgCount Number of Arguments actually gathered used only for multiarg functions.
      \post The result is pushed to the value stack
      \post The function token is removed from the stack
      \throw exception_type if Argument count does not match function requirements.
  */
  void ParserBase::ApplyFunc( ParserStack<token_type> &a_stOpt,
                              ParserStack<token_type> &a_stVal, 
                              int a_iArgCount) const
  { 
    assert(m_pTokenReader.get());

    // Operator stack empty or does not contain tokens with callback functions
    if (a_stOpt.empty() || a_stOpt.top().GetFuncAddr()==0 )
      return;

    token_type funTok = a_stOpt.pop();
    assert(funTok.GetFuncAddr());

    // Binary operators must rely on their internal operator number
    // since counting of operators relies on commas for function arguments
    // binary operators do not have commas in their expression
    int iArgCount = (funTok.GetCode()==cmOPRT_BIN) ? funTok.GetArgCount() : a_iArgCount;

    // determine how many parameters the function needs. To remember iArgCount includes the 
    // string parameter whilst GetArgCount() counts only numeric parameters.
    int iArgRequired = funTok.GetArgCount() + ((funTok.GetType()==tpSTR) ? 1 : 0);

    // That's the number of numerical parameters
    int iArgNumerical = iArgCount - ((funTok.GetType()==tpSTR) ? 1 : 0);

    if (funTok.GetCode()==cmFUNC_STR && iArgCount-iArgNumerical>1)
      Error(ecINTERNAL_ERROR);

    if (funTok.GetArgCount()>=0 && iArgCount>iArgRequired) 
      Error(ecTOO_MANY_PARAMS, m_pTokenReader->GetPos()-1, funTok.GetAsString());

    if (funTok.GetCode()!=cmOPRT_BIN && iArgCount<iArgRequired )
      Error(ecTOO_FEW_PARAMS, m_pTokenReader->GetPos()-1, funTok.GetAsString());

    if (funTok.GetCode()==cmFUNC_STR && iArgCount>iArgRequired )
      Error(ecTOO_MANY_PARAMS, m_pTokenReader->GetPos()-1, funTok.GetAsString());

    // Collect the numeric function arguments from the value stack and store them
    // in a vector
    std::vector<token_type> stArg;  
    for (int i=0; i<iArgNumerical; ++i)
    {
      stArg.push_back( a_stVal.pop() );
      if ( stArg.back().GetType()==tpSTR && funTok.GetType()!=tpSTR )
        Error(ecVAL_EXPECTED, m_pTokenReader->GetPos(), funTok.GetAsString());
    }

    switch(funTok.GetCode())
    {
    case  cmFUNC_STR:  
          stArg.push_back(a_stVal.pop());
          
          if ( stArg.back().GetType()==tpSTR && funTok.GetType()!=tpSTR )
            Error(ecVAL_EXPECTED, m_pTokenReader->GetPos(), funTok.GetAsString());

          ApplyStrFunc(funTok, stArg); 
          break;

    case  cmFUNC_BULK: 
          m_vRPN.AddBulkFun(funTok.GetFuncAddr(), (int)stArg.size()); 
          break;

    case  cmOPRT_BIN:
    case  cmOPRT_POSTFIX:
    case  cmOPRT_INFIX:
    case  cmFUNC:
          if (funTok.GetArgCount()==-1 && iArgCount==0)
            Error(ecTOO_FEW_PARAMS, m_pTokenReader->GetPos(), funTok.GetAsString());

          m_vRPN.AddFun(funTok.GetFuncAddr(), (funTok.GetArgCount()==-1) ? -iArgNumerical : iArgNumerical);
          break;
    default:
        break;
    }

    // Push dummy value representing the function result to the stack
    token_type token;
    token.SetVal(1);  
    a_stVal.push(token);
  }

  //---------------------------------------------------------------------------
  void ParserBase::ApplyIfElse(ParserStack<token_type> &a_stOpt,
                               ParserStack<token_type> &a_stVal) const
  {
    // Check if there is an if Else clause to be calculated
    while (a_stOpt.size() && a_stOpt.top().GetCode()==cmELSE)
    {
      token_type opElse = a_stOpt.pop();
      MUP_ASSERT(a_stOpt.size()>0);

      // Take the value associated with the else branch from the value stack
      token_type vVal2 = a_stVal.pop();

      MUP_ASSERT(a_stOpt.size()>0);
      MUP_ASSERT(a_stVal.size()>=2);

      // it then else is a ternary operator Pop all three values from the value s
      // tack and just return the right value
      token_type vVal1 = a_stVal.pop();
      token_type vExpr = a_stVal.pop();

      a_stVal.push( (vExpr.GetVal()!=0) ? vVal1 : vVal2);

      token_type opIf = a_stOpt.pop();
      MUP_ASSERT(opElse.GetCode()==cmELSE);
      MUP_ASSERT(opIf.GetCode()==cmIF);

      m_vRPN.AddIfElse(cmENDIF);
    } // while pending if-else-clause found
  }

  //---------------------------------------------------------------------------
  /** \brief Performs the necessary steps to write code for
             the execution of binary operators into the bytecode. 
  */
  void ParserBase::ApplyBinOprt(ParserStack<token_type> &a_stOpt,
                                ParserStack<token_type> &a_stVal) const
  {
    // is it a user defined binary operator?
    if (a_stOpt.top().GetCode()==cmOPRT_BIN)
    {
      ApplyFunc(a_stOpt, a_stVal, 2);
    }
    else
    {
      MUP_ASSERT(a_stVal.size()>=2);
      token_type valTok1 = a_stVal.pop(),
                 valTok2 = a_stVal.pop(),
                 optTok  = a_stOpt.pop(),
                 resTok; 

      if ( valTok1.GetType()!=valTok2.GetType() || 
          (valTok1.GetType()==tpSTR && valTok2.GetType()==tpSTR) )
        Error(ecOPRT_TYPE_CONFLICT, m_pTokenReader->GetPos(), optTok.GetAsString());

      if (optTok.GetCode()==cmASSIGN)
      {
        if (valTok2.GetCode()!=cmVAR)
          Error(ecUNEXPECTED_OPERATOR, -1, _T("="));
                      
        m_vRPN.AddAssignOp(valTok2.GetVar());
      }
      else
        m_vRPN.AddOp(optTok.GetCode());

      resTok.SetVal(1);
      a_stVal.push(resTok);
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Apply a binary operator. 
      \param a_stOpt The operator stack
      \param a_stVal The value stack
  */
  void ParserBase::ApplyRemainingOprt(ParserStack<token_type> &stOpt,
                                      ParserStack<token_type> &stVal) const
  {
    while (stOpt.size() && 
           stOpt.top().GetCode() != cmBO &&
           stOpt.top().GetCode() != cmIF)
    {
      token_type tok = stOpt.top();
      switch (tok.GetCode())
      {
      case cmOPRT_INFIX:
      case cmOPRT_BIN:
      case cmLE:
      case cmGE:
      case cmNEQ:
      case cmEQ:
      case cmLT:
      case cmGT:
      case cmADD:
      case cmSUB:
      case cmMUL:
      case cmDIV:
      case cmPOW:
      case cmLAND:
      case cmLOR:
      case cmASSIGN:
          if (stOpt.top().GetCode()==cmOPRT_INFIX)
            ApplyFunc(stOpt, stVal, 1);
          else
            ApplyBinOprt(stOpt, stVal);
          break;

      case cmELSE:
          ApplyIfElse(stOpt, stVal);
          break;

      default:
          Error(ecINTERNAL_ERROR);
      }
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Parse the command code.
      \sa ParseString(...)

      Command code contains precalculated stack positions of the values and the
      associated operators. The Stack is filled beginning from index one the 
      value at index zero is not used at all.
  */
  value_type ParserBase::ParseCmdCode() const
  {
    return ParseCmdCodeBulk(0, 0);
  }

  //---------------------------------------------------------------------------
  /** \brief Evaluate the RPN. 
      \param nOffset The offset added to variable addresses (for bulk mode)
      \param nThreadID OpenMP Thread id of the calling thread
  */
  value_type ParserBase::ParseCmdCodeBulk(int nOffset, int nThreadID) const
  {
    assert(nThreadID<=s_MaxNumOpenMPThreads);

    // Note: The check for nOffset==0 and nThreadID here is not necessary but 
    //       brings a minor performance gain when not in bulk mode.
    value_type *Stack = ((nOffset==0) && (nThreadID==0)) ? &m_vStackBuffer[0] : &m_vStackBuffer[nThreadID * (m_vStackBuffer.size() / s_MaxNumOpenMPThreads)];
    value_type buf;
    int sidx(0);
    for (const SToken *pTok = m_vRPN.GetBase(); pTok->Cmd!=cmEND ; ++pTok)
    {
      switch (pTok->Cmd)
      {
      // built in binary operators
      case  cmLE:   --sidx; Stack[sidx]  = Stack[sidx] <= Stack[sidx+1]; continue;
      case  cmGE:   --sidx; Stack[sidx]  = Stack[sidx] >= Stack[sidx+1]; continue;
      case  cmNEQ:  --sidx; Stack[sidx]  = Stack[sidx] != Stack[sidx+1]; continue;
      case  cmEQ:   --sidx; Stack[sidx]  = Stack[sidx] == Stack[sidx+1]; continue;
      case  cmLT:   --sidx; Stack[sidx]  = Stack[sidx] < Stack[sidx+1];  continue;
      case  cmGT:   --sidx; Stack[sidx]  = Stack[sidx] > Stack[sidx+1];  continue;
      case  cmADD:  --sidx; Stack[sidx] += Stack[1+sidx]; continue;
      case  cmSUB:  --sidx; Stack[sidx] -= Stack[1+sidx]; continue;
      case  cmMUL:  --sidx; Stack[sidx] *= Stack[1+sidx]; continue;
      case  cmDIV:  --sidx;

  #if defined(MUP_MATH_EXCEPTIONS)
                  if (Stack[1+sidx]==0)
                    Error(ecDIV_BY_ZERO);
  #endif
                  Stack[sidx] /= Stack[1+sidx]; 
                  continue;

      case  cmPOW: 
              --sidx; Stack[sidx] = MathImpl<value_type>::Pow(Stack[sidx], Stack[1+sidx]);
              continue;

      case  cmLAND: --sidx; Stack[sidx]  = Stack[sidx] && Stack[sidx+1]; continue;
      case  cmLOR:  --sidx; Stack[sidx]  = Stack[sidx] || Stack[sidx+1]; continue;

      case  cmASSIGN: 
          // Bugfix for Bulkmode:
          // for details see:
          //    https://groups.google.com/forum/embed/?place=forum/muparser-dev&showsearch=true&showpopout=true&showtabs=false&parenturl=http://muparser.beltoforion.de/mup_forum.html&afterlogin&pli=1#!topic/muparser-dev/szgatgoHTws
          --sidx; Stack[sidx] = *(pTok->Oprt.ptr + nOffset) = Stack[sidx + 1]; continue;
          // original code:
          //--sidx; Stack[sidx] = *pTok->Oprt.ptr = Stack[sidx+1]; continue;

      //case  cmBO:  // unused, listed for compiler optimization purposes
      //case  cmBC:
      //      MUP_FAIL(INVALID_CODE_IN_BYTECODE);
      //      continue;

      case  cmIF:
            if (Stack[sidx--]==0)
              pTok += pTok->Oprt.offset;
            continue;

      case  cmELSE:
            pTok += pTok->Oprt.offset;
            continue;

      case  cmENDIF:
            continue;

      //case  cmARG_SEP:
      //      MUP_FAIL(INVALID_CODE_IN_BYTECODE);
      //      continue;

      // value and variable tokens
      case  cmVAR:    Stack[++sidx] = *(pTok->Val.ptr + nOffset);  continue;
      case  cmVAL:    Stack[++sidx] =  pTok->Val.data2;  continue;
      
      case  cmVARPOW2: buf = *(pTok->Val.ptr + nOffset);
                       Stack[++sidx] = buf*buf;
                       continue;

      case  cmVARPOW3: buf = *(pTok->Val.ptr + nOffset);
                       Stack[++sidx] = buf*buf*buf;
                       continue;

      case  cmVARPOW4: buf = *(pTok->Val.ptr + nOffset);
                       Stack[++sidx] = buf*buf*buf*buf;
                       continue;
      
      case  cmVARMUL:  Stack[++sidx] = *(pTok->Val.ptr + nOffset) * pTok->Val.data + pTok->Val.data2;
                       continue;

      // Next is treatment of numeric functions
      case  cmFUNC:
            {
              int iArgCount = pTok->Fun.argc;

              // switch according to argument count
              switch(iArgCount)  
              {
              case 0: sidx += 1; Stack[sidx] = (*(fun_type0)pTok->Fun.ptr)(); continue;
              case 1:            Stack[sidx] = (*(fun_type1)pTok->Fun.ptr)(Stack[sidx]);   continue;
              case 2: sidx -= 1; Stack[sidx] = (*(fun_type2)pTok->Fun.ptr)(Stack[sidx], Stack[sidx+1]); continue;
              case 3: sidx -= 2; Stack[sidx] = (*(fun_type3)pTok->Fun.ptr)(Stack[sidx], Stack[sidx+1], Stack[sidx+2]); continue;
              case 4: sidx -= 3; Stack[sidx] = (*(fun_type4)pTok->Fun.ptr)(Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3]); continue;
              case 5: sidx -= 4; Stack[sidx] = (*(fun_type5)pTok->Fun.ptr)(Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4]); continue;
              case 6: sidx -= 5; Stack[sidx] = (*(fun_type6)pTok->Fun.ptr)(Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5]); continue;
              case 7: sidx -= 6; Stack[sidx] = (*(fun_type7)pTok->Fun.ptr)(Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5], Stack[sidx+6]); continue;
              case 8: sidx -= 7; Stack[sidx] = (*(fun_type8)pTok->Fun.ptr)(Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5], Stack[sidx+6], Stack[sidx+7]); continue;
              case 9: sidx -= 8; Stack[sidx] = (*(fun_type9)pTok->Fun.ptr)(Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5], Stack[sidx+6], Stack[sidx+7], Stack[sidx+8]); continue;
              case 10:sidx -= 9; Stack[sidx] = (*(fun_type10)pTok->Fun.ptr)(Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5], Stack[sidx+6], Stack[sidx+7], Stack[sidx+8], Stack[sidx+9]); continue;
              default:
                if (iArgCount>0) // function with variable arguments store the number as a negative value
                  Error(ecINTERNAL_ERROR, 1);

                sidx -= -iArgCount - 1;
                Stack[sidx] =(*(multfun_type)pTok->Fun.ptr)(&Stack[sidx], -iArgCount);
                continue;
              }
            }

      // Next is treatment of string functions
      case  cmFUNC_STR:
            {
              sidx -= pTok->Fun.argc -1;

              // The index of the string argument in the string table
              int iIdxStack = pTok->Fun.idx;  
              MUP_ASSERT( iIdxStack>=0 && iIdxStack<(int)m_vStringBuf.size() );

              switch(pTok->Fun.argc)  // switch according to argument count
              {
              case 0: Stack[sidx] = (*(strfun_type1)pTok->Fun.ptr)(m_vStringBuf[iIdxStack].c_str()); continue;
              case 1: Stack[sidx] = (*(strfun_type2)pTok->Fun.ptr)(m_vStringBuf[iIdxStack].c_str(), Stack[sidx]); continue;
              case 2: Stack[sidx] = (*(strfun_type3)pTok->Fun.ptr)(m_vStringBuf[iIdxStack].c_str(), Stack[sidx], Stack[sidx+1]); continue;
              }

              continue;
            }

        case  cmFUNC_BULK:
              {
                int iArgCount = pTok->Fun.argc;

                // switch according to argument count
                switch(iArgCount)  
                {
                case 0: sidx += 1; Stack[sidx] = (*(bulkfun_type0 )pTok->Fun.ptr)(nOffset, nThreadID); continue;
                case 1:            Stack[sidx] = (*(bulkfun_type1 )pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx]); continue;
                case 2: sidx -= 1; Stack[sidx] = (*(bulkfun_type2 )pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx], Stack[sidx+1]); continue;
                case 3: sidx -= 2; Stack[sidx] = (*(bulkfun_type3 )pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx], Stack[sidx+1], Stack[sidx+2]); continue;
                case 4: sidx -= 3; Stack[sidx] = (*(bulkfun_type4 )pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3]); continue;
                case 5: sidx -= 4; Stack[sidx] = (*(bulkfun_type5 )pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4]); continue;
                case 6: sidx -= 5; Stack[sidx] = (*(bulkfun_type6 )pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5]); continue;
                case 7: sidx -= 6; Stack[sidx] = (*(bulkfun_type7 )pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5], Stack[sidx+6]); continue;
                case 8: sidx -= 7; Stack[sidx] = (*(bulkfun_type8 )pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5], Stack[sidx+6], Stack[sidx+7]); continue;
                case 9: sidx -= 8; Stack[sidx] = (*(bulkfun_type9 )pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5], Stack[sidx+6], Stack[sidx+7], Stack[sidx+8]); continue;
                case 10:sidx -= 9; Stack[sidx] = (*(bulkfun_type10)pTok->Fun.ptr)(nOffset, nThreadID, Stack[sidx], Stack[sidx+1], Stack[sidx+2], Stack[sidx+3], Stack[sidx+4], Stack[sidx+5], Stack[sidx+6], Stack[sidx+7], Stack[sidx+8], Stack[sidx+9]); continue;
                default:
                  Error(ecINTERNAL_ERROR, 2);
                  continue;
                }
              }

        default:
              Error(ecINTERNAL_ERROR, 3);
              return 0;
      } // switch CmdCode
    } // for all bytecode tokens

    return Stack[m_nFinalResultIdx];  
  }

  //---------------------------------------------------------------------------
  void ParserBase::CreateRPN() const
  {
    if (!m_pTokenReader->GetExpr().length())
      Error(ecUNEXPECTED_EOF, 0);

    ParserStack<token_type> stOpt, stVal;
    ParserStack<int> stArgCount;
    token_type opta, opt;  // for storing operators
    token_type val, tval;  // for storing value

    ReInit();
    
    // The outermost counter counts the number of separated items
    // such as in "a=10,b=20,c=c+a"
    stArgCount.push(1);
    
    for(;;)
    {
      opt = m_pTokenReader->ReadNextToken();

      switch (opt.GetCode())
      {
        //
        // Next three are different kind of value entries
        //
        case cmSTRING:
                opt.SetIdx((int)m_vStringBuf.size());      // Assign buffer index to token 
                stVal.push(opt);
		            m_vStringBuf.push_back(opt.GetAsString()); // Store string in internal buffer
                break;
   
        case cmVAR:
                stVal.push(opt);
                m_vRPN.AddVar( static_cast<value_type*>(opt.GetVar()) );
                break;

        case cmVAL:
		        stVal.push(opt);
                m_vRPN.AddVal( opt.GetVal() );
                break;

        case cmELSE:
                m_nIfElseCounter--;
                if (m_nIfElseCounter<0)
                  Error(ecMISPLACED_COLON, m_pTokenReader->GetPos());

                ApplyRemainingOprt(stOpt, stVal);
                m_vRPN.AddIfElse(cmELSE);
                stOpt.push(opt);
                break;


        case cmARG_SEP:
                if (stArgCount.empty())
                  Error(ecUNEXPECTED_ARG_SEP, m_pTokenReader->GetPos());

                ++stArgCount.top();
                // Falls through.
                // intentional (no break!)

        case cmEND:
                ApplyRemainingOprt(stOpt, stVal);
                break;

       case cmBC:
                {
                  // The argument count for parameterless functions is zero
                  // by default an opening bracket sets parameter count to 1
                  // in preparation of arguments to come. If the last token
                  // was an opening bracket we know better...
                  if (opta.GetCode()==cmBO)
                    --stArgCount.top();
                  
                  ApplyRemainingOprt(stOpt, stVal);

                  // Check if the bracket content has been evaluated completely
                  if (stOpt.size() && stOpt.top().GetCode()==cmBO)
                  {
                    // if opt is ")" and opta is "(" the bracket has been evaluated, now its time to check
                    // if there is either a function or a sign pending
                    // neither the opening nor the closing bracket will be pushed back to
                    // the operator stack
                    // Check if a function is standing in front of the opening bracket, 
                    // if yes evaluate it afterwards check for infix operators
                    assert(stArgCount.size());
                    int iArgCount = stArgCount.pop();
                    
                    stOpt.pop(); // Take opening bracket from stack

                    if (iArgCount>1 && ( stOpt.size()==0 || 
                                        (stOpt.top().GetCode()!=cmFUNC && 
                                         stOpt.top().GetCode()!=cmFUNC_BULK && 
                                         stOpt.top().GetCode()!=cmFUNC_STR) ) )
                      Error(ecUNEXPECTED_ARG, m_pTokenReader->GetPos());
                    
                    // The opening bracket was popped from the stack now check if there
                    // was a function before this bracket
                    if (stOpt.size() && 
                        stOpt.top().GetCode()!=cmOPRT_INFIX && 
                        stOpt.top().GetCode()!=cmOPRT_BIN && 
                        stOpt.top().GetFuncAddr()!=0)
                    {
                      ApplyFunc(stOpt, stVal, iArgCount);
                    }
                  }
                } // if bracket content is evaluated
                break;

        //
        // Next are the binary operator entries
        //
        //case cmAND:   // built in binary operators
        //case cmOR:
        //case cmXOR:
        case cmIF:
                m_nIfElseCounter++;
                // Falls through.
                // intentional (no break!)

        case cmLAND:
        case cmLOR:
        case cmLT:
        case cmGT:
        case cmLE:
        case cmGE:
        case cmNEQ:
        case cmEQ:
        case cmADD:
        case cmSUB:
        case cmMUL:
        case cmDIV:
        case cmPOW:
        case cmASSIGN:
        case cmOPRT_BIN:

                // A binary operator (user defined or built in) has been found. 
                while ( stOpt.size() && 
                        stOpt.top().GetCode() != cmBO &&
                        stOpt.top().GetCode() != cmELSE &&
                        stOpt.top().GetCode() != cmIF)
                {
                  int nPrec1 = GetOprtPrecedence(stOpt.top()),
                      nPrec2 = GetOprtPrecedence(opt);

                  if (stOpt.top().GetCode()==opt.GetCode())
                  {

                    // Deal with operator associativity
                    EOprtAssociativity eOprtAsct = GetOprtAssociativity(opt);
                    if ( (eOprtAsct==oaRIGHT && (nPrec1 <= nPrec2)) || 
                         (eOprtAsct==oaLEFT  && (nPrec1 <  nPrec2)) )
                    {
                      break;
                    }
                  }
                  else if (nPrec1 < nPrec2)
                  {
                    // In case the operators are not equal the precedence decides alone...
                    break;
                  }
                  
                  if (stOpt.top().GetCode()==cmOPRT_INFIX)
                    ApplyFunc(stOpt, stVal, 1);
                  else
                    ApplyBinOprt(stOpt, stVal);
                } // while ( ... )

                if (opt.GetCode()==cmIF)
                  m_vRPN.AddIfElse(opt.GetCode());

    			      // The operator can't be evaluated right now, push back to the operator stack
                stOpt.push(opt);
                break;

        //
        // Last section contains functions and operators implicitly mapped to functions
        //
        case cmBO:
                stArgCount.push(1);
                stOpt.push(opt);
                break;

        case cmOPRT_INFIX:
        case cmFUNC:
        case cmFUNC_BULK:
        case cmFUNC_STR:  
                stOpt.push(opt);
                break;

        case cmOPRT_POSTFIX:
                stOpt.push(opt);
                ApplyFunc(stOpt, stVal, 1);  // this is the postfix operator
                break;

        default:  Error(ecINTERNAL_ERROR, 3);
      } // end of switch operator-token

      opta = opt;

      if ( opt.GetCode() == cmEND )
      {
        m_vRPN.Finalize();
        break;
      }

      if (ParserBase::g_DbgDumpStack)
      {
        StackDump(stVal, stOpt);
        m_vRPN.AsciiDump();
      }
    } // while (true)

    if (ParserBase::g_DbgDumpCmdCode)
      m_vRPN.AsciiDump();

    if (m_nIfElseCounter>0)
      Error(ecMISSING_ELSE_CLAUSE);

    // get the last value (= final result) from the stack
    MUP_ASSERT(stArgCount.size()==1);
    m_nFinalResultIdx = stArgCount.top();
    if (m_nFinalResultIdx==0)
      Error(ecINTERNAL_ERROR, 9);

    if (stVal.size()==0)
      Error(ecEMPTY_EXPRESSION);

    if (stVal.top().GetType()!=tpDBL)
      Error(ecSTR_RESULT);

    m_vStackBuffer.resize(m_vRPN.GetMaxStackSize() * s_MaxNumOpenMPThreads);
  }

  //---------------------------------------------------------------------------
  /** \brief One of the two main parse functions.
      \sa ParseCmdCode(...)

    Parse expression from input string. Perform syntax checking and create 
    bytecode. After parsing the string and creating the bytecode the function 
    pointer #m_pParseFormula will be changed to the second parse routine the 
    uses bytecode instead of string parsing.
  */
  value_type ParserBase::ParseString() const
  {
    try
    {
      CreateRPN();
      m_pParseFormula = &ParserBase::ParseCmdCode;
      return (this->*m_pParseFormula)(); 
    }
    catch(ParserError &exc)
    {
      exc.SetFormula(m_pTokenReader->GetExpr());
      throw;
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Create an error containing the parse error position.

    This function will create an Parser Exception object containing the error text and
    its position.

    \param a_iErrc [in] The error code of type #EErrorCodes.
    \param a_iPos [in] The position where the error was detected.
    \param a_strTok [in] The token string representation associated with the error.
    \throw ParserException always throws that's the only purpose of this function.
  */
  void  ParserBase::Error(EErrorCodes a_iErrc, int a_iPos, const string_type &a_sTok) const
  {
    throw exception_type(a_iErrc, a_sTok, m_pTokenReader->GetExpr(), a_iPos);
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all user defined variables.
      \throw nothrow

      Resets the parser to string parsing mode by calling #ReInit.
  */
  void ParserBase::ClearVar()
  {
    m_VarDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Remove a variable from internal storage.
      \throw nothrow

      Removes a variable if it exists. If the Variable does not exist nothing will be done.
  */
  void ParserBase::RemoveVar(const string_type &a_strVarName)
  {
    varmap_type::iterator item = m_VarDef.find(a_strVarName);
    if (item!=m_VarDef.end())
    {
      m_VarDef.erase(item);
      ReInit();
    }
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all functions.
      \post Resets the parser to string parsing mode.
      \throw nothrow
  */
  void ParserBase::ClearFun()
  {
    m_FunDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all user defined constants.

      Both numeric and string constants will be removed from the internal storage.
      \post Resets the parser to string parsing mode.
      \throw nothrow
  */
  void ParserBase::ClearConst()
  {
    m_ConstDef.clear();
    m_StrVarDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all user defined postfix operators.
      \post Resets the parser to string parsing mode.
      \throw nothrow
  */
  void ParserBase::ClearPostfixOprt()
  {
    m_PostOprtDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all user defined binary operators.
      \post Resets the parser to string parsing mode.
      \throw nothrow
  */
  void ParserBase::ClearOprt()
  {
    m_OprtDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Clear the user defined Prefix operators. 
      \post Resets the parser to string parser mode.
      \throw nothrow
  */
  void ParserBase::ClearInfixOprt()
  {
    m_InfixOprtDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Enable or disable the formula optimization feature. 
      \post Resets the parser to string parser mode.
      \throw nothrow
  */
  void ParserBase::EnableOptimizer(bool a_bIsOn)
  {
    m_vRPN.EnableOptimizer(a_bIsOn);
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Enable the dumping of bytecode and stack content on the console. 
      \param bDumpCmd Flag to enable dumping of the current bytecode to the console.
      \param bDumpStack Flag to enable dumping of the stack content is written to the console.

     This function is for debug purposes only!
  */
  void ParserBase::EnableDebugDump(bool bDumpCmd, bool bDumpStack)
  {
    ParserBase::g_DbgDumpCmdCode = bDumpCmd;
    ParserBase::g_DbgDumpStack   = bDumpStack;
  }

  //------------------------------------------------------------------------------
  /** \brief Enable or disable the built in binary operators.
      \throw nothrow
      \sa m_bBuiltInOp, ReInit()

    If you disable the built in binary operators there will be no binary operators
    defined. Thus you must add them manually one by one. It is not possible to
    disable built in operators selectively. This function will Reinitialize the
    parser by calling ReInit().
  */
  void ParserBase::EnableBuiltInOprt(bool a_bIsOn)
  {
    m_bBuiltInOp = a_bIsOn;
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Query status of built in variables.
      \return #m_bBuiltInOp; true if built in operators are enabled.
      \throw nothrow
  */
  bool ParserBase::HasBuiltInOprt() const
  {
    return m_bBuiltInOp;
  }

  //------------------------------------------------------------------------------
  /** \brief Get the argument separator character. 
  */
  char_type ParserBase::GetArgSep() const
  {
    return m_pTokenReader->GetArgSep();
  }

  //------------------------------------------------------------------------------
  /** \brief Set argument separator. 
      \param cArgSep the argument separator character.
  */
  void ParserBase::SetArgSep(char_type cArgSep)
  {
    m_pTokenReader->SetArgSep(cArgSep);
  }

  //------------------------------------------------------------------------------
  /** \brief Dump stack content. 

      This function is used for debugging only.
  */
  void ParserBase::StackDump(const ParserStack<token_type> &a_stVal, 
                             const ParserStack<token_type> &a_stOprt) const
  {
    ParserStack<token_type> stOprt(a_stOprt), 
                            stVal(a_stVal);

    mu::console() << _T("\nValue stack:\n");
    while ( !stVal.empty() ) 
    {
      token_type val = stVal.pop();
      if (val.GetType()==tpSTR)
        mu::console() << _T(" \"") << val.GetAsString() << _T("\" ");
      else
        mu::console() << _T(" ") << val.GetVal() << _T(" ");
    }
    mu::console() << "\nOperator stack:\n";

    while ( !stOprt.empty() )
    {
      if (stOprt.top().GetCode()<=cmASSIGN) 
      {
        mu::console() << _T("OPRT_INTRNL \"")
                      << ParserBase::c_DefaultOprt[stOprt.top().GetCode()] 
                      << _T("\" \n");
      }
      else
      {
        switch(stOprt.top().GetCode())
        {
        case cmVAR:   mu::console() << _T("VAR\n");  break;
        case cmVAL:   mu::console() << _T("VAL\n");  break;
        case cmFUNC:  mu::console() << _T("FUNC \"") 
                                    << stOprt.top().GetAsString() 
                                    << _T("\"\n");   break;
        case cmFUNC_BULK:  mu::console() << _T("FUNC_BULK \"") 
                                         << stOprt.top().GetAsString() 
                                         << _T("\"\n");   break;
        case cmOPRT_INFIX: mu::console() << _T("OPRT_INFIX \"")
                                         << stOprt.top().GetAsString() 
                                         << _T("\"\n");      break;
        case cmOPRT_BIN:   mu::console() << _T("OPRT_BIN \"") 
                                         << stOprt.top().GetAsString() 
                                         << _T("\"\n");           break;
        case cmFUNC_STR: mu::console() << _T("FUNC_STR\n");       break;
        case cmEND:      mu::console() << _T("END\n");            break;
        case cmUNKNOWN:  mu::console() << _T("UNKNOWN\n");        break;
        case cmBO:       mu::console() << _T("BRACKET \"(\"\n");  break;
        case cmBC:       mu::console() << _T("BRACKET \")\"\n");  break;
        case cmIF:       mu::console() << _T("IF\n");  break;
        case cmELSE:     mu::console() << _T("ELSE\n");  break;
        case cmENDIF:    mu::console() << _T("ENDIF\n");  break;
        default:         mu::console() << stOprt.top().GetCode() << _T(" ");  break;
        }
      }	
      stOprt.pop();
    }

    mu::console() << dec << endl;
  }

  //------------------------------------------------------------------------------
  /** \brief Evaluate an expression containing comma separated subexpressions 
      \param [out] nStackSize The total number of results available
      \return Pointer to the array containing all expression results

      This member function can be used to retrieve all results of an expression
      made up of multiple comma separated subexpressions (i.e. "x+y,sin(x),cos(y)")
  */
  value_type* ParserBase::Eval(int &nStackSize) const
  {
    (this->*m_pParseFormula)(); 
    nStackSize = m_nFinalResultIdx;

    // (for historic reasons the stack starts at position 1)
    return &m_vStackBuffer[1];
  }

  //---------------------------------------------------------------------------
  /** \brief Return the number of results on the calculation stack. 
  
    If the expression contains comma separated subexpressions (i.e. "sin(y), x+y"). 
    There may be more than one return value. This function returns the number of 
    available results.
  */
  int ParserBase::GetNumResults() const
  {
    return m_nFinalResultIdx;
  }

  //---------------------------------------------------------------------------
  /** \brief Calculate the result.

    A note on const correctness: 
    I consider it important that Calc is a const function.
    Due to caching operations Calc changes only the state of internal variables with one exception
    m_UsedVar this is reset during string parsing and accessible from the outside. Instead of making
    Calc non const GetUsedVar is non const because it explicitly calls Eval() forcing this update. 

    \pre A formula must be set.
    \pre Variables must have been set (if needed)

    \sa #m_pParseFormula
    \return The evaluation result
    \throw ParseException if no Formula is set or in case of any other error related to the formula.
  */
  value_type ParserBase::Eval() const
  {
    return (this->*m_pParseFormula)(); 
  }

  //---------------------------------------------------------------------------
  void ParserBase::Eval(value_type *results, int nBulkSize)
  {
/* <ibg 2014-09-24/> Commented because it is making a unit test impossible

    // Parallelization does not make sense for fewer than 10000 computations 
    // due to thread creation overhead. If the bulk size is below 2000
    // computation is refused. 
    if (nBulkSize<2000)
    {
      throw ParserError(ecUNREASONABLE_NUMBER_OF_COMPUTATIONS);
    }
*/
    CreateRPN();

    int i = 0;

#ifdef MUP_USE_OPENMP
//#define DEBUG_OMP_STUFF
    #ifdef DEBUG_OMP_STUFF
    int *pThread = new int[nBulkSize];
    int *pIdx = new int[nBulkSize];
    #endif

    int nMaxThreads = std::min(omp_get_max_threads(), s_MaxNumOpenMPThreads);
	int nThreadID = 0, ct = 0;
    omp_set_num_threads(nMaxThreads);

    #pragma omp parallel for schedule(static, nBulkSize/nMaxThreads) private(nThreadID)
    for (i=0; i<nBulkSize; ++i)
    {
      nThreadID = omp_get_thread_num();
      results[i] = ParseCmdCodeBulk(i, nThreadID);

      #ifdef DEBUG_OMP_STUFF
      #pragma omp critical
      {
        pThread[ct] = nThreadID;  
        pIdx[ct] = i; 
        ct++;
      }
      #endif
    }

#ifdef DEBUG_OMP_STUFF
    FILE *pFile = fopen("bulk_dbg.txt", "w");
    for (i=0; i<nBulkSize; ++i)
    {
      fprintf(pFile, "idx: %d  thread: %d \n", pIdx[i], pThread[i]);
    }
    
    delete [] pIdx;
    delete [] pThread;

    fclose(pFile);
#endif

#else
    for (i=0; i<nBulkSize; ++i)
    {
      results[i] = ParseCmdCodeBulk(i, 0);
    }
#endif

  }
} // namespace mu
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

//amalgamated header include by Piotrs script #include "muParserBytecode.h"

#include <algorithm>
#include <cassert>
#include <string>
#include <stack>
#include <vector>
#include <iostream>

//amalgamated header include by Piotrs script #include "muParserDef.h"
//amalgamated header include by Piotrs script #include "muParserError.h"
//amalgamated header include by Piotrs script #include "muParserToken.h"
//amalgamated header include by Piotrs script #include "muParserStack.h"
//amalgamated header include by Piotrs script #include "muParserTemplateMagic.h"


namespace mu
{
  //---------------------------------------------------------------------------
  /** \brief Bytecode default constructor. */
  ParserByteCode::ParserByteCode()
    :m_iStackPos(0)
    ,m_iMaxStackSize(0)
    ,m_vRPN()
    ,m_bEnableOptimizer(true)
  {
    m_vRPN.reserve(50);
  }

  //---------------------------------------------------------------------------
  /** \brief Copy constructor. 
    
      Implemented in Terms of Assign(const ParserByteCode &a_ByteCode)
  */
  ParserByteCode::ParserByteCode(const ParserByteCode &a_ByteCode)
  {
    Assign(a_ByteCode);
  }

  //---------------------------------------------------------------------------
  /** \brief Assignment operator.
    
      Implemented in Terms of Assign(const ParserByteCode &a_ByteCode)
  */
  ParserByteCode& ParserByteCode::operator=(const ParserByteCode &a_ByteCode)
  {
    Assign(a_ByteCode);
    return *this;
  }

  //---------------------------------------------------------------------------
  void ParserByteCode::EnableOptimizer(bool bStat)
  {
    m_bEnableOptimizer = bStat;
  }

  //---------------------------------------------------------------------------
  /** \brief Copy state of another object to this. 
    
      \throw nowthrow
  */
  void ParserByteCode::Assign(const ParserByteCode &a_ByteCode)
  {
    if (this==&a_ByteCode)    
      return;

    m_iStackPos = a_ByteCode.m_iStackPos;
    m_vRPN = a_ByteCode.m_vRPN;
    m_iMaxStackSize = a_ByteCode.m_iMaxStackSize;
	m_bEnableOptimizer = a_ByteCode.m_bEnableOptimizer;
  }

  //---------------------------------------------------------------------------
  /** \brief Add a Variable pointer to bytecode. 
      \param a_pVar Pointer to be added.
      \throw nothrow
  */
  void ParserByteCode::AddVar(value_type *a_pVar)
  {
    ++m_iStackPos;
    m_iMaxStackSize = std::max(m_iMaxStackSize, (size_t)m_iStackPos);

    // optimization does not apply
    SToken tok;
    tok.Cmd       = cmVAR;
    tok.Val.ptr   = a_pVar;
    tok.Val.data  = 1;
    tok.Val.data2 = 0;
    m_vRPN.push_back(tok);
  }

  //---------------------------------------------------------------------------
  /** \brief Add a Variable pointer to bytecode. 

      Value entries in byte code consist of:
      <ul>
        <li>value array position of the value</li>
        <li>the operator code according to ParserToken::cmVAL</li>
        <li>the value stored in #mc_iSizeVal number of bytecode entries.</li>
      </ul>

      \param a_pVal Value to be added.
      \throw nothrow
  */
  void ParserByteCode::AddVal(value_type a_fVal)
  {
    ++m_iStackPos;
    m_iMaxStackSize = std::max(m_iMaxStackSize, (size_t)m_iStackPos);

    // If optimization does not apply
    SToken tok;
    tok.Cmd = cmVAL;
    tok.Val.ptr   = NULL;
    tok.Val.data  = 0;
    tok.Val.data2 = a_fVal;
    m_vRPN.push_back(tok);
  }

  //---------------------------------------------------------------------------
  void ParserByteCode::ConstantFolding(ECmdCode a_Oprt)
  {
    std::size_t sz = m_vRPN.size();
    value_type &x = m_vRPN[sz-2].Val.data2,
               &y = m_vRPN[sz-1].Val.data2;
    switch (a_Oprt)
    {
    case cmLAND: x = (int)x && (int)y; m_vRPN.pop_back(); break;
    case cmLOR:  x = (int)x || (int)y; m_vRPN.pop_back(); break;
    case cmLT:   x = x < y;  m_vRPN.pop_back();  break;
    case cmGT:   x = x > y;  m_vRPN.pop_back();  break;
    case cmLE:   x = x <= y; m_vRPN.pop_back();  break;
    case cmGE:   x = x >= y; m_vRPN.pop_back();  break;
    case cmNEQ:  x = x != y; m_vRPN.pop_back();  break;
    case cmEQ:   x = x == y; m_vRPN.pop_back();  break;
    case cmADD:  x = x + y;  m_vRPN.pop_back();  break;
    case cmSUB:  x = x - y;  m_vRPN.pop_back();  break;
    case cmMUL:  x = x * y;  m_vRPN.pop_back();  break;
    case cmDIV: 

#if defined(MUP_MATH_EXCEPTIONS)
        if (y==0)
          throw ParserError(ecDIV_BY_ZERO, _T("0"));
#endif

        x = x / y;   
        m_vRPN.pop_back();
        break;

    case cmPOW: x = MathImpl<value_type>::Pow(x, y); 
                m_vRPN.pop_back();
                break;

    default:
        break;
    } // switch opcode
  }

  //---------------------------------------------------------------------------
  /** \brief Add an operator identifier to bytecode. 
    
      Operator entries in byte code consist of:
      <ul>
        <li>value array position of the result</li>
        <li>the operator code according to ParserToken::ECmdCode</li>
      </ul>

      \sa  ParserToken::ECmdCode
  */
  void ParserByteCode::AddOp(ECmdCode a_Oprt)
  {
    bool bOptimized = false;

    if (m_bEnableOptimizer)
    {
      std::size_t sz = m_vRPN.size();

      // Check for foldable constants like:
      //   cmVAL cmVAL cmADD 
      // where cmADD can stand fopr any binary operator applied to
      // two constant values.
      if (sz>=2 && m_vRPN[sz-2].Cmd == cmVAL && m_vRPN[sz-1].Cmd == cmVAL)
      {
        ConstantFolding(a_Oprt);
        bOptimized = true;
      }
      else
      {
        switch(a_Oprt)
        {
        case  cmPOW:
              // Optimization for polynomials of low order
              if (m_vRPN[sz-2].Cmd == cmVAR && m_vRPN[sz-1].Cmd == cmVAL)
              {
                if (m_vRPN[sz-1].Val.data2==2)
                  m_vRPN[sz-2].Cmd = cmVARPOW2;
                else if (m_vRPN[sz-1].Val.data2==3)
                  m_vRPN[sz-2].Cmd = cmVARPOW3;
                else if (m_vRPN[sz-1].Val.data2==4)
                  m_vRPN[sz-2].Cmd = cmVARPOW4;
                else
                  break;

                m_vRPN.pop_back();
                bOptimized = true;
              }
              break;

        case  cmSUB:
        case  cmADD:
              // Simple optimization based on pattern recognition for a shitload of different
              // bytecode combinations of addition/subtraction
              if ( (m_vRPN[sz-1].Cmd == cmVAR    && m_vRPN[sz-2].Cmd == cmVAL)    ||
                   (m_vRPN[sz-1].Cmd == cmVAL    && m_vRPN[sz-2].Cmd == cmVAR)    || 
                   (m_vRPN[sz-1].Cmd == cmVAL    && m_vRPN[sz-2].Cmd == cmVARMUL) ||
                   (m_vRPN[sz-1].Cmd == cmVARMUL && m_vRPN[sz-2].Cmd == cmVAL)    ||
                   (m_vRPN[sz-1].Cmd == cmVAR    && m_vRPN[sz-2].Cmd == cmVAR    && m_vRPN[sz-2].Val.ptr == m_vRPN[sz-1].Val.ptr) ||
                   (m_vRPN[sz-1].Cmd == cmVAR    && m_vRPN[sz-2].Cmd == cmVARMUL && m_vRPN[sz-2].Val.ptr == m_vRPN[sz-1].Val.ptr) ||
                   (m_vRPN[sz-1].Cmd == cmVARMUL && m_vRPN[sz-2].Cmd == cmVAR    && m_vRPN[sz-2].Val.ptr == m_vRPN[sz-1].Val.ptr) ||
                   (m_vRPN[sz-1].Cmd == cmVARMUL && m_vRPN[sz-2].Cmd == cmVARMUL && m_vRPN[sz-2].Val.ptr == m_vRPN[sz-1].Val.ptr) )
              {
                assert( (m_vRPN[sz-2].Val.ptr==NULL && m_vRPN[sz-1].Val.ptr!=NULL) ||
                        (m_vRPN[sz-2].Val.ptr!=NULL && m_vRPN[sz-1].Val.ptr==NULL) || 
                        (m_vRPN[sz-2].Val.ptr == m_vRPN[sz-1].Val.ptr) );

                m_vRPN[sz-2].Cmd = cmVARMUL;
                m_vRPN[sz-2].Val.ptr    = (value_type*)((long long)(m_vRPN[sz-2].Val.ptr) | (long long)(m_vRPN[sz-1].Val.ptr));    // variable
                m_vRPN[sz-2].Val.data2 += ((a_Oprt==cmSUB) ? -1 : 1) * m_vRPN[sz-1].Val.data2;  // offset
                m_vRPN[sz-2].Val.data  += ((a_Oprt==cmSUB) ? -1 : 1) * m_vRPN[sz-1].Val.data;   // multiplicand
                m_vRPN.pop_back();
                bOptimized = true;
              } 
              break;

        case  cmMUL:
              if ( (m_vRPN[sz-1].Cmd == cmVAR && m_vRPN[sz-2].Cmd == cmVAL) ||
                   (m_vRPN[sz-1].Cmd == cmVAL && m_vRPN[sz-2].Cmd == cmVAR) ) 
              {
                m_vRPN[sz-2].Cmd        = cmVARMUL;
                m_vRPN[sz-2].Val.ptr    = (value_type*)((long long)(m_vRPN[sz-2].Val.ptr) | (long long)(m_vRPN[sz-1].Val.ptr));
                m_vRPN[sz-2].Val.data   = m_vRPN[sz-2].Val.data2 + m_vRPN[sz-1].Val.data2;
                m_vRPN[sz-2].Val.data2  = 0;
                m_vRPN.pop_back();
                bOptimized = true;
              } 
              else if ( (m_vRPN[sz-1].Cmd == cmVAL    && m_vRPN[sz-2].Cmd == cmVARMUL) ||
                        (m_vRPN[sz-1].Cmd == cmVARMUL && m_vRPN[sz-2].Cmd == cmVAL) )
              {
                // Optimization: 2*(3*b+1) or (3*b+1)*2 -> 6*b+2
                m_vRPN[sz-2].Cmd     = cmVARMUL;
                m_vRPN[sz-2].Val.ptr = (value_type*)((long long)(m_vRPN[sz-2].Val.ptr) | (long long)(m_vRPN[sz-1].Val.ptr));
                if (m_vRPN[sz-1].Cmd == cmVAL)
                {
                  m_vRPN[sz-2].Val.data  *= m_vRPN[sz-1].Val.data2;
                  m_vRPN[sz-2].Val.data2 *= m_vRPN[sz-1].Val.data2;
                }
                else
                {
                  m_vRPN[sz-2].Val.data  = m_vRPN[sz-1].Val.data  * m_vRPN[sz-2].Val.data2;
                  m_vRPN[sz-2].Val.data2 = m_vRPN[sz-1].Val.data2 * m_vRPN[sz-2].Val.data2;
                }
                m_vRPN.pop_back();
                bOptimized = true;
              }
              else if (m_vRPN[sz-1].Cmd == cmVAR && m_vRPN[sz-2].Cmd == cmVAR &&
                       m_vRPN[sz-1].Val.ptr == m_vRPN[sz-2].Val.ptr)
              {
                // Optimization: a*a -> a^2
                m_vRPN[sz-2].Cmd = cmVARPOW2;
                m_vRPN.pop_back();
                bOptimized = true;
              }
              break;

        case cmDIV:
              if (m_vRPN[sz-1].Cmd == cmVAL && m_vRPN[sz-2].Cmd == cmVARMUL && m_vRPN[sz-1].Val.data2!=0)
              {
                // Optimization: 4*a/2 -> 2*a
                m_vRPN[sz-2].Val.data  /= m_vRPN[sz-1].Val.data2;
                m_vRPN[sz-2].Val.data2 /= m_vRPN[sz-1].Val.data2;
                m_vRPN.pop_back();
                bOptimized = true;
              }
              break;
        default:
            // no optimization for other opcodes
            break;
        } // switch a_Oprt
      }
    }

    // If optimization can't be applied just write the value
    if (!bOptimized)
    {
      --m_iStackPos;
      SToken tok;
      tok.Cmd = a_Oprt;
      m_vRPN.push_back(tok);
    }
  }

  //---------------------------------------------------------------------------
  void ParserByteCode::AddIfElse(ECmdCode a_Oprt)
  {
    SToken tok;
    tok.Cmd = a_Oprt;
    m_vRPN.push_back(tok);
  }

  //---------------------------------------------------------------------------
  /** \brief Add an assignment operator
    
      Operator entries in byte code consist of:
      <ul>
        <li>cmASSIGN code</li>
        <li>the pointer of the destination variable</li>
      </ul>

      \sa  ParserToken::ECmdCode
  */
  void ParserByteCode::AddAssignOp(value_type *a_pVar)
  {
    --m_iStackPos;

    SToken tok;
    tok.Cmd = cmASSIGN;
    tok.Oprt.ptr = a_pVar;
    m_vRPN.push_back(tok);
  }

  //---------------------------------------------------------------------------
  /** \brief Add function to bytecode. 

      \param a_iArgc Number of arguments, negative numbers indicate multiarg functions.
      \param a_pFun Pointer to function callback.
  */
  void ParserByteCode::AddFun(generic_fun_type a_pFun, int a_iArgc)
  {
    if (a_iArgc>=0)
    {
      m_iStackPos = m_iStackPos - a_iArgc + 1; 
    }
    else
    {
      // function with unlimited number of arguments
      m_iStackPos = m_iStackPos + a_iArgc + 1; 
    }
    m_iMaxStackSize = std::max(m_iMaxStackSize, (size_t)m_iStackPos);

    SToken tok;
    tok.Cmd = cmFUNC;
    tok.Fun.argc = a_iArgc;
    tok.Fun.ptr = a_pFun;
    m_vRPN.push_back(tok);
  }

  //---------------------------------------------------------------------------
  /** \brief Add a bulk function to bytecode. 

      \param a_iArgc Number of arguments, negative numbers indicate multiarg functions.
      \param a_pFun Pointer to function callback.
  */
  void ParserByteCode::AddBulkFun(generic_fun_type a_pFun, int a_iArgc)
  {
    m_iStackPos = m_iStackPos - a_iArgc + 1; 
    m_iMaxStackSize = std::max(m_iMaxStackSize, (size_t)m_iStackPos);

    SToken tok;
    tok.Cmd = cmFUNC_BULK;
    tok.Fun.argc = a_iArgc;
    tok.Fun.ptr = a_pFun;
    m_vRPN.push_back(tok);
  }

  //---------------------------------------------------------------------------
  /** \brief Add Strung function entry to the parser bytecode. 
      \throw nothrow

      A string function entry consists of the stack position of the return value,
      followed by a cmSTRFUNC code, the function pointer and an index into the 
      string buffer maintained by the parser.
  */
  void ParserByteCode::AddStrFun(generic_fun_type a_pFun, int a_iArgc, int a_iIdx)
  {
    m_iStackPos = m_iStackPos - a_iArgc + 1;

    SToken tok;
    tok.Cmd = cmFUNC_STR;
    tok.Fun.argc = a_iArgc;
    tok.Fun.idx = a_iIdx;
    tok.Fun.ptr = a_pFun;
    m_vRPN.push_back(tok);

    m_iMaxStackSize = std::max(m_iMaxStackSize, (size_t)m_iStackPos);
  }

  //---------------------------------------------------------------------------
  /** \brief Add end marker to bytecode.
      
      \throw nothrow 
  */
  void ParserByteCode::Finalize()
  {
    SToken tok;
    tok.Cmd = cmEND;
    m_vRPN.push_back(tok);
    rpn_type(m_vRPN).swap(m_vRPN);     // shrink bytecode vector to fit

    // Determine the if-then-else jump offsets
    ParserStack<int> stIf, stElse;
    int idx;
    for (int i=0; i<(int)m_vRPN.size(); ++i)
    {
      switch(m_vRPN[i].Cmd)
      {
      case cmIF:
            stIf.push(i);
            break;

      case  cmELSE:
            stElse.push(i);
            idx = stIf.pop();
            m_vRPN[idx].Oprt.offset = i - idx;
            break;
      
      case cmENDIF:
            idx = stElse.pop();
            m_vRPN[idx].Oprt.offset = i - idx;
            break;

      default:
            break;
      }
    }
  }

  //---------------------------------------------------------------------------
  const SToken* ParserByteCode::GetBase() const
  {
    if (m_vRPN.size()==0)
      throw ParserError(ecINTERNAL_ERROR);
    else
      return &m_vRPN[0];
  }

  //---------------------------------------------------------------------------
  std::size_t ParserByteCode::GetMaxStackSize() const
  {
    return m_iMaxStackSize+1;
  }

  //---------------------------------------------------------------------------
  /** \brief Returns the number of entries in the bytecode. */
  std::size_t ParserByteCode::GetSize() const
  {
    return m_vRPN.size();
  }

  //---------------------------------------------------------------------------
  /** \brief Delete the bytecode. 
  
      \throw nothrow

      The name of this function is a violation of my own coding guidelines
      but this way it's more in line with the STL functions thus more 
      intuitive.
  */
  void ParserByteCode::clear()
  {
    m_vRPN.clear();
    m_iStackPos = 0;
    m_iMaxStackSize = 0;
  }

  //---------------------------------------------------------------------------
  /** \brief Dump bytecode (for debugging only!). */
  void ParserByteCode::AsciiDump()
  {
    if (!m_vRPN.size()) 
    {
      mu::console() << _T("No bytecode available\n");
      return;
    }

    mu::console() << _T("Number of RPN tokens:") << (int)m_vRPN.size() << _T("\n");
    for (std::size_t i=0; i<m_vRPN.size() && m_vRPN[i].Cmd!=cmEND; ++i)
    {
      mu::console() << std::dec << i << _T(" : \t");
      switch (m_vRPN[i].Cmd)
      {
      case cmVAL:   mu::console() << _T("VAL \t");
                    mu::console() << _T("[") << m_vRPN[i].Val.data2 << _T("]\n");
                    break;

      case cmVAR:   mu::console() << _T("VAR \t");
	                  mu::console() << _T("[ADDR: 0x") << std::hex << m_vRPN[i].Val.ptr << _T("]\n"); 
                    break;

      case cmVARPOW2: mu::console() << _T("VARPOW2 \t");
	                    mu::console() << _T("[ADDR: 0x") << std::hex << m_vRPN[i].Val.ptr << _T("]\n"); 
                      break;

      case cmVARPOW3: mu::console() << _T("VARPOW3 \t");
	                    mu::console() << _T("[ADDR: 0x") << std::hex << m_vRPN[i].Val.ptr << _T("]\n"); 
                      break;

      case cmVARPOW4: mu::console() << _T("VARPOW4 \t");
	                    mu::console() << _T("[ADDR: 0x") << std::hex << m_vRPN[i].Val.ptr << _T("]\n"); 
                      break;

      case cmVARMUL:  mu::console() << _T("VARMUL \t");
	                    mu::console() << _T("[ADDR: 0x") << std::hex << m_vRPN[i].Val.ptr << _T("]"); 
                      mu::console() << _T(" * [") << m_vRPN[i].Val.data << _T("]");
                      mu::console() << _T(" + [") << m_vRPN[i].Val.data2 << _T("]\n");
                      break;

      case cmFUNC:  mu::console() << _T("CALL\t");
                    mu::console() << _T("[ARG:") << std::dec << m_vRPN[i].Fun.argc << _T("]"); 
                    mu::console() << _T("[ADDR: 0x") << std::hex << m_vRPN[i].Fun.ptr << _T("]"); 
                    mu::console() << _T("\n");
                    break;

      case cmFUNC_STR:
                    mu::console() << _T("CALL STRFUNC\t");
                    mu::console() << _T("[ARG:") << std::dec << m_vRPN[i].Fun.argc << _T("]");
                    mu::console() << _T("[IDX:") << std::dec << m_vRPN[i].Fun.idx << _T("]");
                    mu::console() << _T("[ADDR: 0x") << m_vRPN[i].Fun.ptr << _T("]\n"); 
                    break;

      case cmLT:    mu::console() << _T("LT\n");  break;
      case cmGT:    mu::console() << _T("GT\n");  break;
      case cmLE:    mu::console() << _T("LE\n");  break;
      case cmGE:    mu::console() << _T("GE\n");  break;
      case cmEQ:    mu::console() << _T("EQ\n");  break;
      case cmNEQ:   mu::console() << _T("NEQ\n"); break;
      case cmADD:   mu::console() << _T("ADD\n"); break;
      case cmLAND:  mu::console() << _T("&&\n"); break;
      case cmLOR:   mu::console() << _T("||\n"); break;
      case cmSUB:   mu::console() << _T("SUB\n"); break;
      case cmMUL:   mu::console() << _T("MUL\n"); break;
      case cmDIV:   mu::console() << _T("DIV\n"); break;
      case cmPOW:   mu::console() << _T("POW\n"); break;

      case cmIF:    mu::console() << _T("IF\t");
                    mu::console() << _T("[OFFSET:") << std::dec << m_vRPN[i].Oprt.offset << _T("]\n");
                    break;

      case cmELSE:  mu::console() << _T("ELSE\t");
                    mu::console() << _T("[OFFSET:") << std::dec << m_vRPN[i].Oprt.offset << _T("]\n");
                    break;

      case cmENDIF: mu::console() << _T("ENDIF\n"); break;

      case cmASSIGN: 
                    mu::console() << _T("ASSIGN\t");
                    mu::console() << _T("[ADDR: 0x") << m_vRPN[i].Oprt.ptr << _T("]\n"); 
                    break; 

      default:      mu::console() << _T("(unknown code: ") << m_vRPN[i].Cmd << _T(")\n"); 
                    break;
      } // switch cmdCode
    } // while bytecode

    mu::console() << _T("END") << std::endl;
  }
} // namespace mu
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

//amalgamated header include by Piotrs script #include "muParserCallback.h"

/** \file
    \brief Implementation of the parser callback class.
*/


namespace mu
{
  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type0 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(0)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type1 a_pFun, bool a_bAllowOpti, int a_iPrec, ECmdCode a_iCode)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(1)
    ,m_iPri(a_iPrec)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(a_iCode)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  /** \brief Constructor for constructing function callbacks taking two arguments. 
      \throw nothrow
  */
  ParserCallback::ParserCallback(fun_type2 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(2)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  /** \brief Constructor for constructing binary operator callbacks. 
      \param a_pFun Pointer to a static function taking two arguments
      \param a_bAllowOpti A flag indicating this function can be optimized
      \param a_iPrec The operator precedence
      \param a_eOprtAsct The operators associativity
      \throw nothrow
  */
  ParserCallback::ParserCallback(fun_type2 a_pFun, 
                                 bool a_bAllowOpti, 
                                 int a_iPrec, 
                                 EOprtAssociativity a_eOprtAsct)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(2)
    ,m_iPri(a_iPrec)
    ,m_eOprtAsct(a_eOprtAsct)
    ,m_iCode(cmOPRT_BIN)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type3 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(3)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type4 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(4)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type5 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(5)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type6 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(6)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type7 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(7)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type8 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(8)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type9 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(9)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(fun_type10 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(10)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type0 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(0)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type1 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(1)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  /** \brief Constructor for constructing function callbacks taking two arguments. 
      \throw nothrow
  */
  ParserCallback::ParserCallback(bulkfun_type2 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(2)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type3 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(3)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type4 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(4)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type5 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(5)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type6 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(6)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type7 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(7)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type8 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(8)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type9 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(9)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}

  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(bulkfun_type10 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(10)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_BULK)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(multfun_type a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(-1)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC)
    ,m_iType(tpDBL)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(strfun_type1 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(0)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_STR)
    ,m_iType(tpSTR)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(strfun_type2 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(1)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_STR)
    ,m_iType(tpSTR)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  ParserCallback::ParserCallback(strfun_type3 a_pFun, bool a_bAllowOpti)
    :m_pFun((void*)a_pFun)
    ,m_iArgc(2)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmFUNC_STR)
    ,m_iType(tpSTR)
    ,m_bAllowOpti(a_bAllowOpti)
  {}


  //---------------------------------------------------------------------------
  /** \brief Default constructor. 
      \throw nothrow
  */
  ParserCallback::ParserCallback()
    :m_pFun(0)
    ,m_iArgc(0)
    ,m_iPri(-1)
    ,m_eOprtAsct(oaNONE)
    ,m_iCode(cmUNKNOWN)
    ,m_iType(tpVOID)
    ,m_bAllowOpti(0)
  {}


  //---------------------------------------------------------------------------
  /** \brief Copy constructor. 
      \throw nothrow
  */
  ParserCallback::ParserCallback(const ParserCallback &ref)
  {
    m_pFun       = ref.m_pFun;
    m_iArgc      = ref.m_iArgc;
    m_bAllowOpti = ref.m_bAllowOpti;
    m_iCode      = ref.m_iCode;
    m_iType      = ref.m_iType;
    m_iPri       = ref.m_iPri;
    m_eOprtAsct  = ref.m_eOprtAsct;
  }

  //---------------------------------------------------------------------------
  /** \brief Clone this instance and return a pointer to the new instance. */
  ParserCallback* ParserCallback::Clone() const
  {
    return new ParserCallback(*this);
  }

  //---------------------------------------------------------------------------
  /** \brief Return tru if the function is conservative.

      Conservative functions return always the same result for the same argument.
      \throw nothrow
  */
  bool ParserCallback::IsOptimizable() const  
  { 
    return m_bAllowOpti; 
  }

  //---------------------------------------------------------------------------
  /** \brief Get the callback address for the parser function. 
  
      The type of the address is void. It needs to be recasted according to the
      argument number to the right type.

      \throw nothrow
      \return #pFun
  */
  void* ParserCallback::GetAddr() const 
  { 
    return m_pFun;  
  }

  //---------------------------------------------------------------------------
  /** \brief Return the callback code. */
  ECmdCode  ParserCallback::GetCode() const 
  { 
    return m_iCode; 
  }
  
  //---------------------------------------------------------------------------
  ETypeCode ParserCallback::GetType() const 
  { 
    return m_iType; 
  }


  //---------------------------------------------------------------------------
  /** \brief Return the operator precedence. 
      \throw nothrown

     Only valid if the callback token is an operator token (binary or infix).
  */
  int ParserCallback::GetPri()  const 
  { 
    return m_iPri;  
  }

  //---------------------------------------------------------------------------
  /** \brief Return the operators associativity. 
      \throw nothrown

     Only valid if the callback token is a binary operator token.
  */
  EOprtAssociativity ParserCallback::GetAssociativity() const
  {
    return m_eOprtAsct;
  }

  //---------------------------------------------------------------------------
  /** \brief Returns the number of function Arguments. */
  int ParserCallback::GetArgc() const 
  { 
    return m_iArgc; 
  }
} // namespace mu
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
//amalgamated header include by Piotrs script #include "muParser.h"
//amalgamated header include by Piotrs script #include "muParserTemplateMagic.h"

//--- Standard includes ------------------------------------------------------------------------
#include <cmath>
#include <algorithm>
#include <numeric>

/** \brief Pi (what else?). */
#define PARSER_CONST_PI  3.141592653589793238462643

/** \brief The Eulerian number. */
#define PARSER_CONST_E   2.718281828459045235360287

using namespace std;

/** \file
    \brief Implementation of the standard floating point parser.
*/



/** \brief Namespace for mathematical applications. */
namespace mu
{


  //---------------------------------------------------------------------------
  // Trigonometric function
  value_type Parser::Sin(value_type v)   { return MathImpl<value_type>::Sin(v);  }
  value_type Parser::Cos(value_type v)   { return MathImpl<value_type>::Cos(v);  }
  value_type Parser::Tan(value_type v)   { return MathImpl<value_type>::Tan(v);  }
  value_type Parser::ASin(value_type v)  { return MathImpl<value_type>::ASin(v); }
  value_type Parser::ACos(value_type v)  { return MathImpl<value_type>::ACos(v); }
  value_type Parser::ATan(value_type v)  { return MathImpl<value_type>::ATan(v); }
  value_type Parser::ATan2(value_type v1, value_type v2) { return MathImpl<value_type>::ATan2(v1, v2); }
  value_type Parser::Sinh(value_type v)  { return MathImpl<value_type>::Sinh(v); }
  value_type Parser::Cosh(value_type v)  { return MathImpl<value_type>::Cosh(v); }
  value_type Parser::Tanh(value_type v)  { return MathImpl<value_type>::Tanh(v); }
  value_type Parser::ASinh(value_type v) { return MathImpl<value_type>::ASinh(v); }
  value_type Parser::ACosh(value_type v) { return MathImpl<value_type>::ACosh(v); }
  value_type Parser::ATanh(value_type v) { return MathImpl<value_type>::ATanh(v); }

  //---------------------------------------------------------------------------
  // Logarithm functions

  // Logarithm base 2
  value_type Parser::Log2(value_type v)  
  { 
    #ifdef MUP_MATH_EXCEPTIONS
        if (v<=0)
          throw ParserError(ecDOMAIN_ERROR, _T("Log2"));
    #endif

    return MathImpl<value_type>::Log2(v);  
  }  

  // Logarithm base 10
  value_type Parser::Log10(value_type v) 
  { 
    #ifdef MUP_MATH_EXCEPTIONS
        if (v<=0)
          throw ParserError(ecDOMAIN_ERROR, _T("Log10"));
    #endif

    return MathImpl<value_type>::Log10(v); 
  } 

// Logarithm base e (natural logarithm)
  value_type Parser::Ln(value_type v)    
  { 
    #ifdef MUP_MATH_EXCEPTIONS
        if (v<=0)
          throw ParserError(ecDOMAIN_ERROR, _T("Ln"));
    #endif

    return MathImpl<value_type>::Log(v);   
  } 

  //---------------------------------------------------------------------------
  //  misc
  value_type Parser::Exp(value_type v)  { return MathImpl<value_type>::Exp(v);  }
  value_type Parser::Abs(value_type v)  { return MathImpl<value_type>::Abs(v);  }
  value_type Parser::Sqrt(value_type v) 
  { 
    #ifdef MUP_MATH_EXCEPTIONS
        if (v<0)
          throw ParserError(ecDOMAIN_ERROR, _T("sqrt"));
    #endif

    return MathImpl<value_type>::Sqrt(v); 
  }
  value_type Parser::Rint(value_type v) { return MathImpl<value_type>::Rint(v); }
  value_type Parser::Sign(value_type v) { return MathImpl<value_type>::Sign(v); }

  //---------------------------------------------------------------------------
  /** \brief Callback for the unary minus operator.
      \param v The value to negate
      \return -v
  */
  value_type Parser::UnaryMinus(value_type v) 
  { 
    return -v; 
  }

  //---------------------------------------------------------------------------
  /** \brief Callback for the unary minus operator.
      \param v The value to negate
      \return -v
  */
  value_type Parser::UnaryPlus(value_type v) 
  { 
    return v; 
  }

  //---------------------------------------------------------------------------
  /** \brief Callback for adding multiple values. 
      \param [in] a_afArg Vector with the function arguments
      \param [in] a_iArgc The size of a_afArg
  */
  value_type Parser::Sum(const value_type *a_afArg, int a_iArgc)
  { 
    if (!a_iArgc)	
      throw exception_type(_T("too few arguments for function sum."));

    value_type fRes=0;
    for (int i=0; i<a_iArgc; ++i) fRes += a_afArg[i];
    return fRes;
  }

  //---------------------------------------------------------------------------
  /** \brief Callback for averaging multiple values. 
      \param [in] a_afArg Vector with the function arguments
      \param [in] a_iArgc The size of a_afArg
  */
  value_type Parser::Avg(const value_type *a_afArg, int a_iArgc)
  { 
    if (!a_iArgc)	
      throw exception_type(_T("too few arguments for function sum."));

    value_type fRes=0;
    for (int i=0; i<a_iArgc; ++i) fRes += a_afArg[i];
    return fRes/(value_type)a_iArgc;
  }


  //---------------------------------------------------------------------------
  /** \brief Callback for determining the minimum value out of a vector. 
      \param [in] a_afArg Vector with the function arguments
      \param [in] a_iArgc The size of a_afArg
  */
  value_type Parser::Min(const value_type *a_afArg, int a_iArgc)
  { 
    if (!a_iArgc)	
      throw exception_type(_T("too few arguments for function min."));

    value_type fRes=a_afArg[0];
    for (int i=0; i<a_iArgc; ++i) 
      fRes = std::min(fRes, a_afArg[i]);

    return fRes;
  }


  //---------------------------------------------------------------------------
  /** \brief Callback for determining the maximum value out of a vector. 
      \param [in] a_afArg Vector with the function arguments
      \param [in] a_iArgc The size of a_afArg
  */
  value_type Parser::Max(const value_type *a_afArg, int a_iArgc)
  { 
    if (!a_iArgc)	
      throw exception_type(_T("too few arguments for function min."));

    value_type fRes=a_afArg[0];
    for (int i=0; i<a_iArgc; ++i) fRes = std::max(fRes, a_afArg[i]);

    return fRes;
  }


  //---------------------------------------------------------------------------
  /** \brief Default value recognition callback. 
      \param [in] a_szExpr Pointer to the expression
      \param [in, out] a_iPos Pointer to an index storing the current position within the expression
      \param [out] a_fVal Pointer where the value should be stored in case one is found.
      \return 1 if a value was found 0 otherwise.
  */
  int Parser::IsVal(const char_type* a_szExpr, int *a_iPos, value_type *a_fVal)
  {
    value_type fVal(0);

    stringstream_type stream(a_szExpr);
    stream.seekg(0);        // todo:  check if this really is necessary
    stream.imbue(Parser::s_locale);
    stream >> fVal;
    stringstream_type::pos_type iEnd = stream.tellg(); // Position after reading

    if (iEnd==(stringstream_type::pos_type)-1)
      return 0;

    *a_iPos += (int)iEnd;
    *a_fVal = fVal;
    return 1;
  }


  //---------------------------------------------------------------------------
  /** \brief Constructor. 

    Call ParserBase class constructor and trigger Function, Operator and Constant initialization.
  */
  Parser::Parser()
    :ParserBase()
  {
    AddValIdent(IsVal);

    InitCharSets();
    InitFun();
    InitConst();
    InitOprt();
  }

  //---------------------------------------------------------------------------
  /** \brief Define the character sets. 
      \sa DefineNameChars, DefineOprtChars, DefineInfixOprtChars
    
    This function is used for initializing the default character sets that define
    the characters to be useable in function and variable names and operators.
  */
  void Parser::InitCharSets()
  {
    DefineNameChars( _T("0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") );
    DefineOprtChars( _T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-*^/?<>=#!$%&|~'_{}") );
    DefineInfixOprtChars( _T("/+-*^?<>=#!$%&|~'_") );
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize the default functions. */
  void Parser::InitFun()
  {
    if (mu::TypeInfo<mu::value_type>::IsInteger())
    {
      // When setting MUP_BASETYPE to an integer type
      // Place functions for dealing with integer values here
      // ...
      // ...
      // ...
    }
    else
    {
      // trigonometric functions
      DefineFun(_T("sin"), Sin);
      DefineFun(_T("cos"), Cos);
      DefineFun(_T("tan"), Tan);
      // arcus functions
      DefineFun(_T("asin"), ASin);
      DefineFun(_T("acos"), ACos);
      DefineFun(_T("atan"), ATan);
      DefineFun(_T("atan2"), ATan2);
      // hyperbolic functions
      DefineFun(_T("sinh"), Sinh);
      DefineFun(_T("cosh"), Cosh);
      DefineFun(_T("tanh"), Tanh);
      // arcus hyperbolic functions
      DefineFun(_T("asinh"), ASinh);
      DefineFun(_T("acosh"), ACosh);
      DefineFun(_T("atanh"), ATanh);
      // Logarithm functions
      DefineFun(_T("log2"), Log2);
      DefineFun(_T("log10"), Log10);
      DefineFun(_T("log"), Ln);
      DefineFun(_T("ln"), Ln);
      // misc
      DefineFun(_T("exp"), Exp);
      DefineFun(_T("sqrt"), Sqrt);
      DefineFun(_T("sign"), Sign);
      DefineFun(_T("rint"), Rint);
      DefineFun(_T("abs"), Abs);
      // Functions with variable number of arguments
      DefineFun(_T("sum"), Sum);
      DefineFun(_T("avg"), Avg);
      DefineFun(_T("min"), Min);
      DefineFun(_T("max"), Max);
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize constants.
  
    By default the parser recognizes two constants. Pi ("pi") and the Eulerian
    number ("_e").
  */
  void Parser::InitConst()
  {
    DefineConst(_T("_pi"), (value_type)PARSER_CONST_PI);
    DefineConst(_T("_e"), (value_type)PARSER_CONST_E);
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize operators. 
  
    By default only the unary minus operator is added.
  */
  void Parser::InitOprt()
  {
    DefineInfixOprt(_T("-"), UnaryMinus);
    DefineInfixOprt(_T("+"), UnaryPlus);
  }

  //---------------------------------------------------------------------------
  void Parser::OnDetectVar(string_type * /*pExpr*/, int & /*nStart*/, int & /*nEnd*/)
  {
    // this is just sample code to illustrate modifying variable names on the fly.
    // I'm not sure anyone really needs such a feature...
    /*


    string sVar(pExpr->begin()+nStart, pExpr->begin()+nEnd);
    string sRepl = std::string("_") + sVar + "_";
  
    int nOrigVarEnd = nEnd;
    cout << "variable detected!\n";
    cout << "  Expr: " << *pExpr << "\n";
    cout << "  Start: " << nStart << "\n";
    cout << "  End: " << nEnd << "\n";
    cout << "  Var: \"" << sVar << "\"\n";
    cout << "  Repl: \"" << sRepl << "\"\n";
    nEnd = nStart + sRepl.length();
    cout << "  End: " << nEnd << "\n";
    pExpr->replace(pExpr->begin()+nStart, pExpr->begin()+nOrigVarEnd, sRepl);
    cout << "  New expr: " << *pExpr << "\n";
    */
  }

  //---------------------------------------------------------------------------
  /** \brief Numerically differentiate with regard to a variable. 
      \param [in] a_Var Pointer to the differentiation variable.
      \param [in] a_fPos Position at which the differentiation should take place.
      \param [in] a_fEpsilon Epsilon used for the numerical differentiation.

    Numerical differentiation uses a 5 point operator yielding a 4th order 
    formula. The default value for epsilon is 0.00074 which is
    numeric_limits<double>::epsilon() ^ (1/5) as suggested in the muparser
    forum:

    http://sourceforge.net/forum/forum.php?thread_id=1994611&forum_id=462843
  */
  value_type Parser::Diff(value_type *a_Var, 
                          value_type  a_fPos, 
                          value_type  a_fEpsilon) const
  {
    value_type fRes(0), 
               fBuf(*a_Var),
               f[4] = {0,0,0,0},
               fEpsilon(a_fEpsilon);

    // Backwards compatible calculation of epsilon inc case the user doesn't provide
    // his own epsilon
    if (fEpsilon==0)
      fEpsilon = (a_fPos==0) ? (value_type)1e-10 : (value_type)1e-7 * a_fPos;

    *a_Var = a_fPos+2 * fEpsilon;  f[0] = Eval();
    *a_Var = a_fPos+1 * fEpsilon;  f[1] = Eval();
    *a_Var = a_fPos-1 * fEpsilon;  f[2] = Eval();
    *a_Var = a_fPos-2 * fEpsilon;  f[3] = Eval();
    *a_Var = fBuf; // restore variable

    fRes = (-f[0] + 8*f[1] - 8*f[2] + f[3]) / (12*fEpsilon);
    return fRes;
  }
} // namespace mu
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
#if defined(MUPARSER_DLL) 

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//amalgamated header include by Piotrs script #include "muParserDLL.h"
//amalgamated header include by Piotrs script #include "muParser.h"
//amalgamated header include by Piotrs script #include "muParserInt.h"
//amalgamated header include by Piotrs script #include "muParserError.h"


#define MU_TRY  \
    try     \
        {

#define MU_CATCH                                                 \
        }                                                        \
        catch (muError_t &e)                                      \
        {                                                        \
        ParserTag *pTag = static_cast<ParserTag*>(a_hParser);  \
        pTag->exc = e;                                         \
        pTag->bError = true;                                   \
if (pTag->errHandler)                                  \
    (pTag->errHandler)(a_hParser);                       \
        }                                                        \
        catch (...)                                               \
        {                                                        \
        ParserTag *pTag = static_cast<ParserTag*>(a_hParser);  \
        pTag->exc = muError_t(mu::ecINTERNAL_ERROR);           \
        pTag->bError = true;                                   \
if (pTag->errHandler)                                  \
    (pTag->errHandler)(a_hParser);                       \
        }

/** \file
    \brief This file contains the implementation of the DLL interface of muparser.
    */

//---------------------------------------------------------------------------
// private types
typedef mu::ParserBase::exception_type muError_t;
typedef mu::ParserBase muParser_t;

int g_nBulkSize;

//---------------------------------------------------------------------------
class ParserTag
{
public:
    ParserTag(int nType)
        :pParser((nType == muBASETYPE_FLOAT) ? (mu::ParserBase*)new mu::Parser() :
        (nType == muBASETYPE_INT) ? (mu::ParserBase*)new mu::ParserInt() : NULL)
        , exc()
        , errHandler(NULL)
        , bError(false)
        , m_nParserType(nType)
    {}

    ~ParserTag()
    {
        delete pParser;
    }

    mu::ParserBase *pParser;
    mu::ParserBase::exception_type exc;
    muErrorHandler_t errHandler;
    bool bError;

private:
    ParserTag(const ParserTag &ref);
    ParserTag& operator=(const ParserTag &ref);

    int m_nParserType;
};

static muChar_t s_tmpOutBuf[2048];

//---------------------------------------------------------------------------
//
//
//  unexported functions
//
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
inline muParser_t* AsParser(muParserHandle_t a_hParser)
{
    return static_cast<ParserTag*>(a_hParser)->pParser;
}

//---------------------------------------------------------------------------
inline ParserTag* AsParserTag(muParserHandle_t a_hParser)
{
    return static_cast<ParserTag*>(a_hParser);
}

//---------------------------------------------------------------------------
#if defined(_WIN32)
#define _CRT_SECURE_NO_DEPRECATE

BOOL APIENTRY DllMain(HANDLE /*hModule*/,
    DWORD ul_reason_for_call,
    LPVOID /*lpReserved*/)
{
    switch (ul_reason_for_call)
    {
    case  DLL_PROCESS_ATTACH:
        break;

    case  DLL_THREAD_ATTACH:
    case  DLL_THREAD_DETACH:
    case  DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

#endif

//---------------------------------------------------------------------------
//
//
//  exported functions
//
//
//---------------------------------------------------------------------------

API_EXPORT(void) mupSetVarFactory(muParserHandle_t a_hParser, muFacFun_t a_pFactory, void *pUserData)
{
    MU_TRY
        muParser_t* p(AsParser(a_hParser));
    p->SetVarFactory(a_pFactory, pUserData);
    MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Create a new Parser instance and return its handle.
*/
API_EXPORT(muParserHandle_t) mupCreate(int nBaseType)
{
    switch (nBaseType)
    {
    case  muBASETYPE_FLOAT:   return (void*)(new ParserTag(muBASETYPE_FLOAT));
    case  muBASETYPE_INT:     return (void*)(new ParserTag(muBASETYPE_INT));
    default:                  return NULL;
    }
}

//---------------------------------------------------------------------------
/** \brief Release the parser instance related with a parser handle.
*/
API_EXPORT(void) mupRelease(muParserHandle_t a_hParser)
{
    MU_TRY
        ParserTag* p = static_cast<ParserTag*>(a_hParser);
    delete p;
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(const muChar_t*) mupGetVersion(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));

#ifndef _UNICODE
    sprintf(s_tmpOutBuf, "%s", p->GetVersion().c_str());
#else
    wsprintf(s_tmpOutBuf, _T("%s"), p->GetVersion().c_str());
#endif

    return s_tmpOutBuf;
    MU_CATCH

        return _T("");
}

//---------------------------------------------------------------------------
/** \brief Evaluate the expression.
*/
API_EXPORT(muFloat_t) mupEval(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    return p->Eval();
    MU_CATCH

        return 0;
}

//---------------------------------------------------------------------------
API_EXPORT(muFloat_t*) mupEvalMulti(muParserHandle_t a_hParser, int *nNum)
{
    MU_TRY
        assert(nNum != NULL);

    muParser_t* const p(AsParser(a_hParser));
    return p->Eval(*nNum);
    MU_CATCH

        return 0;
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupEvalBulk(muParserHandle_t a_hParser, muFloat_t *a_res, int nSize)
{
    MU_TRY
        muParser_t* p(AsParser(a_hParser));
    p->Eval(a_res, nSize);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupSetExpr(muParserHandle_t a_hParser, const muChar_t* a_szExpr)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->SetExpr(a_szExpr);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupRemoveVar(muParserHandle_t a_hParser, const muChar_t* a_szName)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->RemoveVar(a_szName);
    MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Release all parser variables.
    \param a_hParser Handle to the parser instance.
    */
API_EXPORT(void) mupClearVar(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->ClearVar();
    MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Release all parser variables.
    \param a_hParser Handle to the parser instance.
    */
API_EXPORT(void) mupClearConst(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->ClearConst();
    MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Clear all user defined operators.
    \param a_hParser Handle to the parser instance.
    */
API_EXPORT(void) mupClearOprt(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->ClearOprt();
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupClearFun(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->ClearFun();
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun0(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muFun0_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun1(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muFun1_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun2(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muFun2_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun3(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFun3_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun4(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFun4_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun5(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFun5_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun6(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFun6_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun7(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFun7_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun8(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFun8_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun9(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFun9_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun10(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFun10_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun0(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muBulkFun0_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun1(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muBulkFun1_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun2(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muBulkFun2_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun3(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muBulkFun3_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun4(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muBulkFun4_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun5(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muBulkFun5_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun6(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muBulkFun6_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun7(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muBulkFun7_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun8(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muBulkFun8_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun9(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muBulkFun9_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkFun10(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muBulkFun10_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineStrFun1(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muStrFun1_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineStrFun2(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muStrFun2_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineStrFun3(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muStrFun3_t a_pFun)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineMultFun(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muMultFun_t a_pFun,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineOprt(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muFun2_t a_pFun,
    muInt_t a_nPrec,
    muInt_t a_nOprtAsct,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineOprt(a_szName,
        a_pFun,
        a_nPrec,
        (mu::EOprtAssociativity)a_nOprtAsct,
        a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineVar(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFloat_t *a_pVar)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineVar(a_szName, a_pVar);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineBulkVar(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFloat_t *a_pVar)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineVar(a_szName, a_pVar);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineConst(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    muFloat_t a_fVal)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineConst(a_szName, a_fVal);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineStrConst(muParserHandle_t a_hParser,
    const muChar_t *a_szName,
    const muChar_t *a_szVal)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineStrConst(a_szName, a_szVal);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(const muChar_t*) mupGetExpr(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));

    // C# explodes when pMsg is returned directly. For some reason it can't access
    // the memory where the message lies directly.
#ifndef _UNICODE
    sprintf(s_tmpOutBuf, "%s", p->GetExpr().c_str());
#else
    wsprintf(s_tmpOutBuf, _T("%s"), p->GetExpr().c_str());
#endif

    return s_tmpOutBuf;

    MU_CATCH

        return _T("");
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefinePostfixOprt(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muFun1_t a_pOprt,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefinePostfixOprt(a_szName, a_pOprt, a_bAllowOpt != 0);
    MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineInfixOprt(muParserHandle_t a_hParser,
    const muChar_t* a_szName,
    muFun1_t a_pOprt,
    muBool_t a_bAllowOpt)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->DefineInfixOprt(a_szName, a_pOprt, a_bAllowOpt != 0);
    MU_CATCH
}

// Define character sets for identifiers
//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineNameChars(muParserHandle_t a_hParser,
    const muChar_t* a_szCharset)
{
    muParser_t* const p(AsParser(a_hParser));
    p->DefineNameChars(a_szCharset);
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineOprtChars(muParserHandle_t a_hParser,
    const muChar_t* a_szCharset)
{
    muParser_t* const p(AsParser(a_hParser));
    p->DefineOprtChars(a_szCharset);
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineInfixOprtChars(muParserHandle_t a_hParser,
    const muChar_t *a_szCharset)
{
    muParser_t* const p(AsParser(a_hParser));
    p->DefineInfixOprtChars(a_szCharset);
}

//---------------------------------------------------------------------------
/** \brief Get the number of variables defined in the parser.
    \param a_hParser [in] Must be a valid parser handle.
    \return The number of used variables.
    \sa mupGetExprVar
    */
API_EXPORT(int) mupGetVarNum(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    const mu::varmap_type VarMap = p->GetVar();
    return (int)VarMap.size();
    MU_CATCH

        return 0; // never reached
}

//---------------------------------------------------------------------------
/** \brief Return a variable that is used in an expression.
    \param a_hParser [in] A valid parser handle.
    \param a_iVar [in] The index of the variable to return.
    \param a_szName [out] Pointer to the variable name.
    \param a_pVar [out] Pointer to the variable.
    \throw nothrow

    Prior to calling this function call mupGetExprVarNum in order to get the
    number of variables in the expression. If the parameter a_iVar is greater
    than the number of variables both a_szName and a_pVar will be set to zero.
    As a side effect this function will trigger an internal calculation of the
    expression undefined variables will be set to zero during this calculation.
    During the calculation user defined callback functions present in the expression
    will be called, this is unavoidable.
    */
API_EXPORT(void) mupGetVar(muParserHandle_t a_hParser,
    unsigned a_iVar,
    const muChar_t **a_szName,
    muFloat_t **a_pVar)
{
    // A static buffer is needed for the name since i can't return the
    // pointer from the map.
    static muChar_t  szName[1024];

    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    const mu::varmap_type VarMap = p->GetVar();

    if (a_iVar >= VarMap.size())
    {
        *a_szName = 0;
        *a_pVar = 0;
        return;
    }
    mu::varmap_type::const_iterator item;

    item = VarMap.begin();
    for (unsigned i = 0; i < a_iVar; ++i)
        ++item;

#ifndef _UNICODE
    strncpy(szName, item->first.c_str(), sizeof(szName));
#else
    wcsncpy(szName, item->first.c_str(), sizeof(szName));
#endif

    szName[sizeof(szName)-1] = 0;

    *a_szName = &szName[0];
    *a_pVar = item->second;
    return;

    MU_CATCH

        *a_szName = 0;
    *a_pVar = 0;
}

//---------------------------------------------------------------------------
/** \brief Get the number of variables used in the expression currently set in the parser.
    \param a_hParser [in] Must be a valid parser handle.
    \return The number of used variables.
    \sa mupGetExprVar
    */
API_EXPORT(int) mupGetExprVarNum(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    const mu::varmap_type VarMap = p->GetUsedVar();
    return (int)VarMap.size();
    MU_CATCH

        return 0; // never reached
}

//---------------------------------------------------------------------------
/** \brief Return a variable that is used in an expression.

    Prior to calling this function call mupGetExprVarNum in order to get the
    number of variables in the expression. If the parameter a_iVar is greater
    than the number of variables both a_szName and a_pVar will be set to zero.
    As a side effect this function will trigger an internal calculation of the
    expression undefined variables will be set to zero during this calculation.
    During the calculation user defined callback functions present in the expression
    will be called, this is unavoidable.

    \param a_hParser [in] A valid parser handle.
    \param a_iVar [in] The index of the variable to return.
    \param a_szName [out] Pointer to the variable name.
    \param a_pVar [out] Pointer to the variable.
    \throw nothrow
    */
API_EXPORT(void) mupGetExprVar(muParserHandle_t a_hParser,
    unsigned a_iVar,
    const muChar_t **a_szName,
    muFloat_t **a_pVar)
{
    // A static buffer is needed for the name since i can't return the
    // pointer from the map.
    static muChar_t  szName[1024];

    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    const mu::varmap_type VarMap = p->GetUsedVar();

    if (a_iVar >= VarMap.size())
    {
        *a_szName = 0;
        *a_pVar = 0;
        return;
    }
    mu::varmap_type::const_iterator item;

    item = VarMap.begin();
    for (unsigned i = 0; i < a_iVar; ++i)
        ++item;

#ifndef _UNICODE
    strncpy(szName, item->first.c_str(), sizeof(szName));
#else
    wcsncpy(szName, item->first.c_str(), sizeof(szName));
#endif

    szName[sizeof(szName)-1] = 0;

    *a_szName = &szName[0];
    *a_pVar = item->second;
    return;

    MU_CATCH

        *a_szName = 0;
    *a_pVar = 0;
}

//---------------------------------------------------------------------------
/** \brief Return the number of constants defined in a parser. */
API_EXPORT(int) mupGetConstNum(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    const mu::valmap_type ValMap = p->GetConst();
    return (int)ValMap.size();
    MU_CATCH

        return 0; // never reached
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(void) mupSetArgSep(muParserHandle_t a_hParser, const muChar_t cArgSep)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->SetArgSep(cArgSep);
    MU_CATCH
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(void) mupResetLocale(muParserHandle_t a_hParser)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->ResetLocale();
    MU_CATCH
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(void) mupSetDecSep(muParserHandle_t a_hParser, const muChar_t cDecSep)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->SetDecSep(cDecSep);
    MU_CATCH
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(void) mupSetThousandsSep(muParserHandle_t a_hParser, const muChar_t cThousandsSep)
{
    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    p->SetThousandsSep(cThousandsSep);
    MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Retrieve name and value of a single parser constant.
    \param a_hParser [in] a valid parser handle
    \param a_iVar [in] Index of the constant to query
    \param a_pszName [out] pointer to a null terminated string with the constant name
    \param [out] The constant value
    */
API_EXPORT(void) mupGetConst(muParserHandle_t a_hParser,
    unsigned a_iVar,
    const muChar_t **a_pszName,
    muFloat_t *a_fVal)
{
    // A static buffer is needed for the name since i can't return the
    // pointer from the map.
    static muChar_t szName[1024];

    MU_TRY
        muParser_t* const p(AsParser(a_hParser));
    const mu::valmap_type ValMap = p->GetConst();

    if (a_iVar >= ValMap.size())
    {
        *a_pszName = 0;
        *a_fVal = 0;
        return;
    }

    mu::valmap_type::const_iterator item;
    item = ValMap.begin();
    for (unsigned i = 0; i < a_iVar; ++i)
        ++item;

#ifndef _UNICODE
    strncpy(szName, item->first.c_str(), sizeof(szName));
#else
    wcsncpy(szName, item->first.c_str(), sizeof(szName));
#endif

    szName[sizeof(szName)-1] = 0;

    *a_pszName = &szName[0];
    *a_fVal = item->second;
    return;

    MU_CATCH

        *a_pszName = 0;
    *a_fVal = 0;
}

//---------------------------------------------------------------------------
/** \brief Add a custom value recognition function.
*/
API_EXPORT(void) mupAddValIdent(muParserHandle_t a_hParser,
    muIdentFun_t a_pFun)
{
    MU_TRY
        muParser_t* p(AsParser(a_hParser));
    p->AddValIdent(a_pFun);
    MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Query if an error occurred.

    After querying the internal error bit will be reset. So a consecutive call
    will return false.
    */
API_EXPORT(muBool_t) mupError(muParserHandle_t a_hParser)
{
    bool bError(AsParserTag(a_hParser)->bError);
    AsParserTag(a_hParser)->bError = false;
    return bError;
}

//---------------------------------------------------------------------------
/** \brief Reset the internal error flag.
*/
API_EXPORT(void) mupErrorReset(muParserHandle_t a_hParser)
{
    AsParserTag(a_hParser)->bError = false;
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupSetErrorHandler(muParserHandle_t a_hParser, muErrorHandler_t a_pHandler)
{
    AsParserTag(a_hParser)->errHandler = a_pHandler;
}

//---------------------------------------------------------------------------
/** \brief Return the message associated with the last error.
*/
API_EXPORT(const muChar_t*) mupGetErrorMsg(muParserHandle_t a_hParser)
{
    ParserTag* const p(AsParserTag(a_hParser));
    const muChar_t *pMsg = p->exc.GetMsg().c_str();

    // C# explodes when pMsg is returned directly. For some reason it can't access
    // the memory where the message lies directly.
#ifndef _UNICODE
    sprintf(s_tmpOutBuf, "%s", pMsg);
#else
    wsprintf(s_tmpOutBuf, _T("%s"), pMsg);
#endif

    return s_tmpOutBuf;
}

//---------------------------------------------------------------------------
/** \brief Return the message associated with the last error.
*/
API_EXPORT(const muChar_t*) mupGetErrorToken(muParserHandle_t a_hParser)
{
    ParserTag* const p(AsParserTag(a_hParser));
    const muChar_t *pToken = p->exc.GetToken().c_str();

    // C# explodes when pMsg is returned directly. For some reason it can't access
    // the memory where the message lies directly.
#ifndef _UNICODE
    sprintf(s_tmpOutBuf, "%s", pToken);
#else
    wsprintf(s_tmpOutBuf, _T("%s"), pToken);
#endif

    return s_tmpOutBuf;
}

//---------------------------------------------------------------------------
/** \brief Return the code associated with the last error.
*/
API_EXPORT(int) mupGetErrorCode(muParserHandle_t a_hParser)
{
    return AsParserTag(a_hParser)->exc.GetCode();
}

//---------------------------------------------------------------------------
/** \brief Return the position associated with the last error. */
API_EXPORT(int) mupGetErrorPos(muParserHandle_t a_hParser)
{
    return (int)AsParserTag(a_hParser)->exc.GetPos();
}

////-----------------------------------------------------------------------------------------------------
//API_EXPORT(const muChar_t*) mupGetErrorExpr(muParserHandle_t a_hParser)
//{
//  return AsParserTag(a_hParser)->exc.GetExpr().c_str();
//}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(muFloat_t*) mupCreateVar()
{
    return new muFloat_t(0);
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(void) mupReleaseVar(muFloat_t *ptr)
{
    delete ptr;
}

#endif      // MUPARSER_DLL
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
//amalgamated header include by Piotrs script #include "muParserError.h"


namespace mu
{
  //------------------------------------------------------------------------------
  const ParserErrorMsg& ParserErrorMsg::Instance()
  {
    static const ParserErrorMsg instance;
    return instance;
  }

  //------------------------------------------------------------------------------
  string_type ParserErrorMsg::operator[](unsigned a_iIdx) const
  {
    return (a_iIdx<m_vErrMsg.size()) ? m_vErrMsg[a_iIdx] : string_type();
  }

  //---------------------------------------------------------------------------
  ParserErrorMsg::ParserErrorMsg()
    :m_vErrMsg(0)
  {
    m_vErrMsg.resize(ecCOUNT);

    m_vErrMsg[ecUNASSIGNABLE_TOKEN]     = _T("Unexpected token \"$TOK$\" found at position $POS$.");
    m_vErrMsg[ecINTERNAL_ERROR]         = _T("Internal error");
    m_vErrMsg[ecINVALID_NAME]           = _T("Invalid function-, variable- or constant name: \"$TOK$\".");
    m_vErrMsg[ecINVALID_BINOP_IDENT]    = _T("Invalid binary operator identifier: \"$TOK$\".");
    m_vErrMsg[ecINVALID_INFIX_IDENT]    = _T("Invalid infix operator identifier: \"$TOK$\".");
    m_vErrMsg[ecINVALID_POSTFIX_IDENT]  = _T("Invalid postfix operator identifier: \"$TOK$\".");
    m_vErrMsg[ecINVALID_FUN_PTR]        = _T("Invalid pointer to callback function.");
    m_vErrMsg[ecEMPTY_EXPRESSION]       = _T("Expression is empty.");
    m_vErrMsg[ecINVALID_VAR_PTR]        = _T("Invalid pointer to variable.");
    m_vErrMsg[ecUNEXPECTED_OPERATOR]    = _T("Unexpected operator \"$TOK$\" found at position $POS$");
    m_vErrMsg[ecUNEXPECTED_EOF]         = _T("Unexpected end of expression at position $POS$");
    m_vErrMsg[ecUNEXPECTED_ARG_SEP]     = _T("Unexpected argument separator at position $POS$");
    m_vErrMsg[ecUNEXPECTED_PARENS]      = _T("Unexpected parenthesis \"$TOK$\" at position $POS$");
    m_vErrMsg[ecUNEXPECTED_FUN]         = _T("Unexpected function \"$TOK$\" at position $POS$");
    m_vErrMsg[ecUNEXPECTED_VAL]         = _T("Unexpected value \"$TOK$\" found at position $POS$");
    m_vErrMsg[ecUNEXPECTED_VAR]         = _T("Unexpected variable \"$TOK$\" found at position $POS$");
    m_vErrMsg[ecUNEXPECTED_ARG]         = _T("Function arguments used without a function (position: $POS$)");
    m_vErrMsg[ecMISSING_PARENS]         = _T("Missing parenthesis");
    m_vErrMsg[ecTOO_MANY_PARAMS]        = _T("Too many parameters for function \"$TOK$\" at expression position $POS$");
    m_vErrMsg[ecTOO_FEW_PARAMS]         = _T("Too few parameters for function \"$TOK$\" at expression position $POS$");
    m_vErrMsg[ecDIV_BY_ZERO]            = _T("Divide by zero");
    m_vErrMsg[ecDOMAIN_ERROR]           = _T("Domain error");
    m_vErrMsg[ecNAME_CONFLICT]          = _T("Name conflict");
    m_vErrMsg[ecOPT_PRI]                = _T("Invalid value for operator priority (must be greater or equal to zero).");
    m_vErrMsg[ecBUILTIN_OVERLOAD]       = _T("user defined binary operator \"$TOK$\" conflicts with a built in operator.");
    m_vErrMsg[ecUNEXPECTED_STR]         = _T("Unexpected string token found at position $POS$.");
    m_vErrMsg[ecUNTERMINATED_STRING]    = _T("Unterminated string starting at position $POS$.");
    m_vErrMsg[ecSTRING_EXPECTED]        = _T("String function called with a non string type of argument.");
    m_vErrMsg[ecVAL_EXPECTED]           = _T("String value used where a numerical argument is expected.");
    m_vErrMsg[ecOPRT_TYPE_CONFLICT]     = _T("No suitable overload for operator \"$TOK$\" at position $POS$.");
    m_vErrMsg[ecSTR_RESULT]             = _T("Function result is a string.");
    m_vErrMsg[ecGENERIC]                = _T("Parser error.");
    m_vErrMsg[ecLOCALE]                 = _T("Decimal separator is identic to function argument separator.");
    m_vErrMsg[ecUNEXPECTED_CONDITIONAL] = _T("The \"$TOK$\" operator must be preceded by a closing bracket.");
    m_vErrMsg[ecMISSING_ELSE_CLAUSE]    = _T("If-then-else operator is missing an else clause");
    m_vErrMsg[ecMISPLACED_COLON]        = _T("Misplaced colon at position $POS$");
    m_vErrMsg[ecUNREASONABLE_NUMBER_OF_COMPUTATIONS] = _T("Number of computations to small for bulk mode. (Vectorisation overhead too costly)");
    
    #if defined(_DEBUG)
      for (int i=0; i<ecCOUNT; ++i)
        if (!m_vErrMsg[i].length())
          assert(false);
    #endif
  }

  //---------------------------------------------------------------------------
  //
  //  ParserError class
  //
  //---------------------------------------------------------------------------

  /** \brief Default constructor. */
  ParserError::ParserError()
    :m_strMsg()
    ,m_strFormula()
    ,m_strTok()
    ,m_iPos(-1)
    ,m_iErrc(ecUNDEFINED)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
  }

  //------------------------------------------------------------------------------
  /** \brief This Constructor is used for internal exceptions only. 
      
    It does not contain any information but the error code.
  */
  ParserError::ParserError(EErrorCodes a_iErrc) 
    :m_strMsg()
    ,m_strFormula()
    ,m_strTok()
    ,m_iPos(-1)
    ,m_iErrc(a_iErrc)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
    m_strMsg = m_ErrMsg[m_iErrc];
    stringstream_type stream;
    stream << (int)m_iPos;
    ReplaceSubString(m_strMsg, _T("$POS$"), stream.str());
    ReplaceSubString(m_strMsg, _T("$TOK$"), m_strTok);
  }

  //------------------------------------------------------------------------------
  /** \brief Construct an error from a message text. */
  ParserError::ParserError(const string_type &sMsg) 
    :m_ErrMsg(ParserErrorMsg::Instance())
  {
    Reset();
    m_strMsg = sMsg;
  }

  //------------------------------------------------------------------------------
  /** \brief Construct an error object. 
      \param [in] a_iErrc the error code.
      \param [in] sTok The token string related to this error.
      \param [in] sExpr The expression related to the error.
      \param [in] a_iPos the position in the expression where the error occurred. 
  */
  ParserError::ParserError( EErrorCodes iErrc,
                            const string_type &sTok,
                            const string_type &sExpr,
                            int iPos )
    :m_strMsg()
    ,m_strFormula(sExpr)
    ,m_strTok(sTok)
    ,m_iPos(iPos)
    ,m_iErrc(iErrc)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
    m_strMsg = m_ErrMsg[m_iErrc];
    stringstream_type stream;
    stream << (int)m_iPos;
    ReplaceSubString(m_strMsg, _T("$POS$"), stream.str());
    ReplaceSubString(m_strMsg, _T("$TOK$"), m_strTok);
  }

  //------------------------------------------------------------------------------
  /** \brief Construct an error object. 
      \param [in] iErrc the error code.
      \param [in] iPos the position in the expression where the error occurred. 
      \param [in] sTok The token string related to this error.
  */
  ParserError::ParserError(EErrorCodes iErrc, int iPos, const string_type &sTok) 
    :m_strMsg()
    ,m_strFormula()
    ,m_strTok(sTok)
    ,m_iPos(iPos)
    ,m_iErrc(iErrc)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
    m_strMsg = m_ErrMsg[m_iErrc];
    stringstream_type stream;
    stream << (int)m_iPos;
    ReplaceSubString(m_strMsg, _T("$POS$"), stream.str());
    ReplaceSubString(m_strMsg, _T("$TOK$"), m_strTok);
  }

  //------------------------------------------------------------------------------
  /** \brief Construct an error object. 
      \param [in] szMsg The error message text.
      \param [in] iPos the position related to the error.
      \param [in] sTok The token string related to this error.
  */
  ParserError::ParserError(const char_type *szMsg, int iPos, const string_type &sTok) 
    :m_strMsg(szMsg)
    ,m_strFormula()
    ,m_strTok(sTok)
    ,m_iPos(iPos)
    ,m_iErrc(ecGENERIC)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
    stringstream_type stream;
    stream << (int)m_iPos;
    ReplaceSubString(m_strMsg, _T("$POS$"), stream.str());
    ReplaceSubString(m_strMsg, _T("$TOK$"), m_strTok);
  }

  //------------------------------------------------------------------------------
  /** \brief Copy constructor. */
  ParserError::ParserError(const ParserError &a_Obj)
    :m_strMsg(a_Obj.m_strMsg)
    ,m_strFormula(a_Obj.m_strFormula)
    ,m_strTok(a_Obj.m_strTok)
    ,m_iPos(a_Obj.m_iPos)
    ,m_iErrc(a_Obj.m_iErrc)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
  }

  //------------------------------------------------------------------------------
  /** \brief Assignment operator. */
  ParserError& ParserError::operator=(const ParserError &a_Obj)
  {
    if (this==&a_Obj)
      return *this;

    m_strMsg = a_Obj.m_strMsg;
    m_strFormula = a_Obj.m_strFormula;
    m_strTok = a_Obj.m_strTok;
    m_iPos = a_Obj.m_iPos;
    m_iErrc = a_Obj.m_iErrc;
    return *this;
  }

  //------------------------------------------------------------------------------
  ParserError::~ParserError()
  {}

  //------------------------------------------------------------------------------
  /** \brief Replace all occurrences of a substring with another string. 
      \param strFind The string that shall be replaced.
      \param strReplaceWith The string that should be inserted instead of strFind
  */
  void ParserError::ReplaceSubString( string_type &strSource,
                                      const string_type &strFind,
                                      const string_type &strReplaceWith)
  {
    string_type strResult;
    string_type::size_type iPos(0), iNext(0);

    for(;;)
    {
      iNext = strSource.find(strFind, iPos);
      strResult.append(strSource, iPos, iNext-iPos);

      if( iNext==string_type::npos )
        break;

      strResult.append(strReplaceWith);
      iPos = iNext + strFind.length();
    } 

    strSource.swap(strResult);
  }

  //------------------------------------------------------------------------------
  /** \brief Reset the erro object. */
  void ParserError::Reset()
  {
    m_strMsg = _T("");
    m_strFormula = _T("");
    m_strTok = _T("");
    m_iPos = -1;
    m_iErrc = ecUNDEFINED;
  }
      
  //------------------------------------------------------------------------------
  /** \brief Set the expression related to this error. */
  void ParserError::SetFormula(const string_type &a_strFormula)
  {
    m_strFormula = a_strFormula;
  }

  //------------------------------------------------------------------------------
  /** \brief gets the expression related tp this error.*/
  const string_type& ParserError::GetExpr() const 
  {
    return m_strFormula;
  }

  //------------------------------------------------------------------------------
  /** \brief Returns the message string for this error. */
  const string_type& ParserError::GetMsg() const
  {
    return m_strMsg;
  }

  //------------------------------------------------------------------------------
  /** \brief Return the formula position related to the error. 

    If the error is not related to a distinct position this will return -1
  */
  int ParserError::GetPos() const
  {
    return m_iPos;
  }

  //------------------------------------------------------------------------------
  /** \brief Return string related with this token (if available). */
  const string_type& ParserError::GetToken() const
  {
    return m_strTok;
  }

  //------------------------------------------------------------------------------
  /** \brief Return the error code. */
  EErrorCodes ParserError::GetCode() const
  {
    return m_iErrc;
  }
} // namespace mu
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

//amalgamated header include by Piotrs script #include "muParserInt.h"

#include <cmath>
#include <algorithm>
#include <numeric>

using namespace std;

/** \file
    \brief Implementation of a parser using integer value.
*/

/** \brief Namespace for mathematical applications. */
namespace mu
{
value_type ParserInt::Abs(value_type v)  { return (value_type)Round(fabs((double)v)); }
value_type ParserInt::Sign(value_type v) { return (Round(v)<0) ? -1 : (Round(v)>0) ? 1 : 0; }
value_type ParserInt::Ite(value_type v1, 
                          value_type v2, 
                          value_type v3) { return (Round(v1)==1) ? Round(v2) : Round(v3); }
value_type ParserInt::Add(value_type v1, value_type v2) { return Round(v1)  + Round(v2); }
value_type ParserInt::Sub(value_type v1, value_type v2) { return Round(v1)  - Round(v2); }
value_type ParserInt::Mul(value_type v1, value_type v2) { return Round(v1)  * Round(v2); }
value_type ParserInt::Div(value_type v1, value_type v2) { return Round(v1)  / Round(v2); }
value_type ParserInt::Mod(value_type v1, value_type v2) { return Round(v1)  % Round(v2); }
value_type ParserInt::Shr(value_type v1, value_type v2) { return Round(v1) >> Round(v2); }
value_type ParserInt::Shl(value_type v1, value_type v2) { return Round(v1) << Round(v2); }
value_type ParserInt::LogAnd(value_type v1, value_type v2) { return Round(v1) & Round(v2); }
value_type ParserInt::LogOr(value_type v1, value_type v2)  { return Round(v1) | Round(v2); }
value_type ParserInt::And(value_type v1, value_type v2) { return Round(v1) && Round(v2); }
value_type ParserInt::Or(value_type v1, value_type v2)  { return Round(v1) || Round(v2); }
value_type ParserInt::Less(value_type v1, value_type v2)      { return Round(v1)  < Round(v2); }
value_type ParserInt::Greater(value_type v1, value_type v2)   { return Round(v1)  > Round(v2); }
value_type ParserInt::LessEq(value_type v1, value_type v2)    { return Round(v1) <= Round(v2); }
value_type ParserInt::GreaterEq(value_type v1, value_type v2) { return Round(v1) >= Round(v2); }
value_type ParserInt::Equal(value_type v1, value_type v2)     { return Round(v1) == Round(v2); }
value_type ParserInt::NotEqual(value_type v1, value_type v2)  { return Round(v1) != Round(v2); }
value_type ParserInt::Not(value_type v) { return !Round(v); }

value_type ParserInt::Pow(value_type v1, value_type v2) 
{ 
  return std::pow((double)Round(v1), (double)Round(v2)); 
}

//---------------------------------------------------------------------------
// Unary operator Callbacks: Infix operators
value_type ParserInt::UnaryMinus(value_type v) 
{ 
  return -Round(v); 
}

//---------------------------------------------------------------------------
value_type ParserInt::Sum(const value_type* a_afArg, int a_iArgc)
{ 
  if (!a_iArgc)	
    throw ParserError(_T("too few arguments for function sum."));

  value_type fRes=0;
  for (int i=0; i<a_iArgc; ++i) 
    fRes += a_afArg[i];

  return fRes;
}

//---------------------------------------------------------------------------
value_type ParserInt::Min(const value_type* a_afArg, int a_iArgc)
{ 
  if (!a_iArgc)	
    throw ParserError( _T("too few arguments for function min.") );

  value_type fRes=a_afArg[0];
  for (int i=0; i<a_iArgc; ++i) 
    fRes = std::min(fRes, a_afArg[i]);

  return fRes;
}

//---------------------------------------------------------------------------
value_type ParserInt::Max(const value_type* a_afArg, int a_iArgc)
{ 
  if (!a_iArgc)	
    throw ParserError(_T("too few arguments for function min."));

  value_type fRes=a_afArg[0];
  for (int i=0; i<a_iArgc; ++i) 
    fRes = std::max(fRes, a_afArg[i]);

  return fRes;
}

//---------------------------------------------------------------------------
// Default value recognition callback
int ParserInt::IsVal(const char_type *a_szExpr, int *a_iPos, value_type *a_fVal)
{
  string_type buf(a_szExpr);
  std::size_t pos = buf.find_first_not_of(_T("0123456789"));

  if (pos==std::string::npos)
    return 0;

  stringstream_type stream( buf.substr(0, pos ) );
  int iVal(0);

  stream >> iVal;
  if (stream.fail())
    return 0;
      
  stringstream_type::pos_type iEnd = stream.tellg();   // Position after reading
  if (stream.fail())
    iEnd = stream.str().length();  

  if (iEnd==(stringstream_type::pos_type)-1)
    return 0;

  *a_iPos += (int)iEnd;
  *a_fVal = (value_type)iVal;
  return 1;
}

//---------------------------------------------------------------------------
/** \brief Check a given position in the expression for the presence of 
           a hex value. 
    \param a_szExpr Pointer to the expression string
    \param [in/out] a_iPos Pointer to an integer value holding the current parsing 
           position in the expression.
    \param [out] a_fVal Pointer to the position where the detected value shall be stored.

  Hey values must be prefixed with "0x" in order to be detected properly.
*/
int ParserInt::IsHexVal(const char_type *a_szExpr, int *a_iPos, value_type *a_fVal)
{
  if (a_szExpr[1]==0 || (a_szExpr[0]!='0' || a_szExpr[1]!='x') ) 
    return 0;

  unsigned iVal(0);

  // New code based on streams for UNICODE compliance:
  stringstream_type::pos_type nPos(0);
  stringstream_type ss(a_szExpr + 2);
  ss >> std::hex >> iVal;
  nPos = ss.tellg();

  if (nPos==(stringstream_type::pos_type)0)
    return 1;

  *a_iPos += (int)(2 + nPos);
  *a_fVal = (value_type)iVal;
  return 1;
}

//---------------------------------------------------------------------------
int ParserInt::IsBinVal(const char_type *a_szExpr, int *a_iPos, value_type *a_fVal)
{
  if (a_szExpr[0]!='#') 
    return 0;

  unsigned iVal(0), 
           iBits(sizeof(iVal)*8),
           i(0);

  for (i=0; (a_szExpr[i+1]=='0' || a_szExpr[i+1]=='1') && i<iBits; ++i)
    iVal |= (int)(a_szExpr[i+1]=='1') << ((iBits-1)-i);

  if (i==0) 
    return 0;

  if (i==iBits)
    throw exception_type(_T("Binary to integer conversion error (overflow)."));

  *a_fVal = (unsigned)(iVal >> (iBits-i) );
  *a_iPos += i+1;

  return 1;
}

//---------------------------------------------------------------------------
/** \brief Constructor. 

  Call ParserBase class constructor and trigger Function, Operator and Constant initialization.
*/
ParserInt::ParserInt()
  :ParserBase()
{
  AddValIdent(IsVal);    // lowest priority
  AddValIdent(IsBinVal);
  AddValIdent(IsHexVal); // highest priority

  InitCharSets();
  InitFun();
  InitOprt();
}

//---------------------------------------------------------------------------
void ParserInt::InitConst()
{
}

//---------------------------------------------------------------------------
void ParserInt::InitCharSets()
{
  DefineNameChars( _T("0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") );
  DefineOprtChars( _T("+-*^/?<>=!%&|~'_") );
  DefineInfixOprtChars( _T("/+-*^?<>=!%&|~'_") );
}

//---------------------------------------------------------------------------
/** \brief Initialize the default functions. */
void ParserInt::InitFun()
{
  DefineFun( _T("sign"), Sign);
  DefineFun( _T("abs"), Abs);
  DefineFun( _T("if"), Ite);
  DefineFun( _T("sum"), Sum);
  DefineFun( _T("min"), Min);
  DefineFun( _T("max"), Max);
}

//---------------------------------------------------------------------------
/** \brief Initialize operators. */
void ParserInt::InitOprt()
{
  // disable all built in operators, not all of them useful for integer numbers
  // (they don't do rounding of values)
  EnableBuiltInOprt(false);

  // Disable all built in operators, they won't work with integer numbers
  // since they are designed for floating point numbers
  DefineInfixOprt( _T("-"), UnaryMinus);
  DefineInfixOprt( _T("!"), Not);

  DefineOprt( _T("&"), LogAnd, prLOGIC);
  DefineOprt( _T("|"), LogOr, prLOGIC);
  DefineOprt( _T("&&"), And, prLOGIC);
  DefineOprt( _T("||"), Or, prLOGIC);

  DefineOprt( _T("<"), Less, prCMP);
  DefineOprt( _T(">"), Greater, prCMP);
  DefineOprt( _T("<="), LessEq, prCMP);
  DefineOprt( _T(">="), GreaterEq, prCMP);
  DefineOprt( _T("=="), Equal, prCMP);
  DefineOprt( _T("!="), NotEqual, prCMP);

  DefineOprt( _T("+"), Add, prADD_SUB);
  DefineOprt( _T("-"), Sub, prADD_SUB);

  DefineOprt( _T("*"), Mul, prMUL_DIV);
  DefineOprt( _T("/"), Div, prMUL_DIV);
  DefineOprt( _T("%"), Mod, prMUL_DIV);

  DefineOprt( _T("^"), Pow, prPOW, oaRIGHT);
  DefineOprt( _T(">>"), Shr, prMUL_DIV+1);
  DefineOprt( _T("<<"), Shl, prMUL_DIV+1);
}

} // namespace mu
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

//amalgamated header include by Piotrs script #include "muParserTest.h"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <limits>

#define PARSER_CONST_PI  3.141592653589793238462643
#define PARSER_CONST_E   2.718281828459045235360287

using namespace std;

/** \file
    \brief This file contains the implementation of parser test cases.
*/

namespace mu
{
  namespace Test
  {
    int ParserTester::c_iCount = 0;

    //---------------------------------------------------------------------------------------------
    ParserTester::ParserTester()
      :m_vTestFun()
    {
      AddTest(&ParserTester::TestNames);
      AddTest(&ParserTester::TestSyntax);
      AddTest(&ParserTester::TestPostFix);
      AddTest(&ParserTester::TestInfixOprt);
      AddTest(&ParserTester::TestVarConst);
      AddTest(&ParserTester::TestMultiArg);
      AddTest(&ParserTester::TestExpression);
      AddTest(&ParserTester::TestIfThenElse);
      AddTest(&ParserTester::TestInterface);
      AddTest(&ParserTester::TestBinOprt);
      AddTest(&ParserTester::TestException);
      AddTest(&ParserTester::TestStrArg);
      AddTest(&ParserTester::TestBulkMode);

      ParserTester::c_iCount = 0;
    }

    //---------------------------------------------------------------------------------------------
    int ParserTester::IsHexVal(const char_type *a_szExpr, int *a_iPos, value_type *a_fVal)
    {
      if (a_szExpr[1]==0 || (a_szExpr[0]!='0' || a_szExpr[1]!='x') ) 
        return 0;

      unsigned iVal(0);

      // New code based on streams for UNICODE compliance:
      stringstream_type::pos_type nPos(0);
      stringstream_type ss(a_szExpr + 2);
      ss >> std::hex >> iVal;
      nPos = ss.tellg();

      if (nPos==(stringstream_type::pos_type)0)
        return 1;

      *a_iPos += (int)(2 + nPos);
      *a_fVal = (value_type)iVal;
      return 1;
    }

    //---------------------------------------------------------------------------------------------
    int ParserTester::TestInterface()
    {
      int iStat = 0;
      mu::console() << _T("testing member functions...");
   
      // Test RemoveVar
      value_type afVal[3] = {1,2,3};
      Parser p;
  
      try
      {
        p.DefineVar( _T("a"), &afVal[0]);
        p.DefineVar( _T("b"), &afVal[1]);
        p.DefineVar( _T("c"), &afVal[2]);
        p.SetExpr( _T("a+b+c") );
        p.Eval();
      }
      catch(...)
      {
        iStat += 1;  // this is not supposed to happen 
      }

      try
      {
        p.RemoveVar( _T("c") );
        p.Eval();
        iStat += 1;  // not supposed to reach this, nonexisting variable "c" deleted...
      }
      catch(...)
      {
        // failure is expected...
      }

      if (iStat==0) 
        mu::console() << _T("passed") << endl;
      else 
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------------------------
    int ParserTester::TestStrArg()
    {
      int iStat = 0;
      mu::console() << _T("testing string arguments...");
 
      iStat += EqnTest(_T("valueof(\"\")"), 123, true);   // empty string arguments caused a crash
      iStat += EqnTest(_T("valueof(\"aaa\")+valueof(\"bbb\")  "), 246, true);
      iStat += EqnTest(_T("2*(valueof(\"aaa\")-23)+valueof(\"bbb\")"), 323, true);
      // use in expressions with variables
      iStat += EqnTest(_T("a*(atof(\"10\")-b)"), 8, true);
      iStat += EqnTest(_T("a-(atof(\"10\")*b)"), -19, true);
      // string + numeric arguments
      iStat += EqnTest(_T("strfun1(\"100\")"), 100, true);
      iStat += EqnTest(_T("strfun2(\"100\",1)"), 101, true);
      iStat += EqnTest(_T("strfun3(\"99\",1,2)"), 102, true);
      // string constants
      iStat += EqnTest(_T("atof(str1)+atof(str2)"), 3.33, true);

      if (iStat==0)
        mu::console() << _T("passed") << endl;
      else 
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------------------------
    int ParserTester::TestBulkMode()
    {
        int iStat = 0;
        mu::console() << _T("testing bulkmode...");

#define EQN_TEST_BULK(EXPR, R1, R2, R3, R4, PASS) \
        { \
          double res[] = { R1, R2, R3, R4 }; \
          iStat += EqnTestBulk(_T(EXPR), res, (PASS)); \
        }

        // Bulk Variables for the test:
        // a: 1,2,3,4
        // b: 2,2,2,2
        // c: 3,3,3,3
        // d: 5,4,3,2
        EQN_TEST_BULK("a",   1, 1, 1, 1, false)
        EQN_TEST_BULK("a",   1, 2, 3, 4, true)
        EQN_TEST_BULK("b=a", 1, 2, 3, 4, true)
        EQN_TEST_BULK("b=a, b*10", 10, 20, 30, 40, true)
        EQN_TEST_BULK("b=a, b*10, a", 1, 2, 3, 4, true)
        EQN_TEST_BULK("a+b", 3, 4, 5, 6, true)
        EQN_TEST_BULK("c*(a+b)", 9, 12, 15, 18, true)
#undef EQN_TEST_BULK

        if (iStat == 0)
            mu::console() << _T("passed") << endl;
        else
            mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

        return iStat;
    }

    //---------------------------------------------------------------------------------------------
    int ParserTester::TestBinOprt()
    {
      int iStat = 0;
      mu::console() << _T("testing binary operators...");
   
      // built in operators
      // xor operator

      iStat += EqnTest(_T("a++b"), 3, true);
      iStat += EqnTest(_T("a ++ b"), 3, true);
      iStat += EqnTest(_T("1++2"), 3, true);
      iStat += EqnTest(_T("1 ++ 2"), 3, true);
      iStat += EqnTest(_T("a add b"), 3, true);
      iStat += EqnTest(_T("1 add 2"), 3, true);
      iStat += EqnTest(_T("a<b"), 1, true);
      iStat += EqnTest(_T("b>a"), 1, true);
      iStat += EqnTest(_T("a>a"), 0, true);
      iStat += EqnTest(_T("a<a"), 0, true);
      iStat += EqnTest(_T("a>a"), 0, true);
      iStat += EqnTest(_T("a<=a"), 1, true);
      iStat += EqnTest(_T("a<=b"), 1, true);
      iStat += EqnTest(_T("b<=a"), 0, true);
      iStat += EqnTest(_T("a>=a"), 1, true);
      iStat += EqnTest(_T("b>=a"), 1, true);
      iStat += EqnTest(_T("a>=b"), 0, true);

      // Test logical operators, especially if user defined "&" and the internal "&&" collide
      iStat += EqnTest(_T("1 && 1"), 1, true); 
      iStat += EqnTest(_T("1 && 0"), 0, true); 
      iStat += EqnTest(_T("(a<b) && (b>a)"), 1, true); 
      iStat += EqnTest(_T("(a<b) && (a>b)"), 0, true); 
      //iStat += EqnTest(_T("12 and 255"), 12, true); 
      //iStat += EqnTest(_T("12 and 0"), 0, true); 
      iStat += EqnTest(_T("12 & 255"), 12, true); 
      iStat += EqnTest(_T("12 & 0"), 0, true); 
      iStat += EqnTest(_T("12&255"), 12, true); 
      iStat += EqnTest(_T("12&0"), 0, true); 

      // Assignment operator
      iStat += EqnTest(_T("a = b"), 2, true); 
      iStat += EqnTest(_T("a = sin(b)"), 0.909297, true); 
      iStat += EqnTest(_T("a = 1+sin(b)"), 1.909297, true);
      iStat += EqnTest(_T("(a=b)*2"), 4, true);
      iStat += EqnTest(_T("2*(a=b)"), 4, true);
      iStat += EqnTest(_T("2*(a=b+1)"), 6, true);
      iStat += EqnTest(_T("(a=b+1)*2"), 6, true);
      iStat += EqnTest(_T("a=c, a*10"), 30, true);

      iStat += EqnTest(_T("2^2^3"), 256, true); 
      iStat += EqnTest(_T("1/2/3"), 1.0/6.0, true); 

      // reference: http://www.wolframalpha.com/input/?i=3%2B4*2%2F%281-5%29^2^3
      iStat += EqnTest(_T("3+4*2/(1-5)^2^3"), 3.0001220703125, true); 

      // Test user defined binary operators
      iStat += EqnTestInt(_T("1 | 2"), 3, true);          
      iStat += EqnTestInt(_T("1 || 2"), 1, true);          
      iStat += EqnTestInt(_T("123 & 456"), 72, true);          
      iStat += EqnTestInt(_T("(123 & 456) % 10"), 2, true);
      iStat += EqnTestInt(_T("1 && 0"), 0, true);          
      iStat += EqnTestInt(_T("123 && 456"), 1, true);          
      iStat += EqnTestInt(_T("1 << 3"), 8, true);          
      iStat += EqnTestInt(_T("8 >> 3"), 1, true);          
      iStat += EqnTestInt(_T("9 / 4"), 2, true);  
      iStat += EqnTestInt(_T("9 % 4"), 1, true);  
      iStat += EqnTestInt(_T("if(5%2,1,0)"), 1, true);
      iStat += EqnTestInt(_T("if(4%2,1,0)"), 0, true);
      iStat += EqnTestInt(_T("-10+1"), -9, true);
      iStat += EqnTestInt(_T("1+2*3"), 7, true);
      iStat += EqnTestInt(_T("const1 != const2"), 1, true);
      iStat += EqnTestInt(_T("const1 != const2"), 0, false);
      iStat += EqnTestInt(_T("const1 == const2"), 0, true);
      iStat += EqnTestInt(_T("const1 == 1"), 1, true);
      iStat += EqnTestInt(_T("10*(const1 == 1)"), 10, true);
      iStat += EqnTestInt(_T("2*(const1 | const2)"), 6, true);
      iStat += EqnTestInt(_T("2*(const1 | const2)"), 7, false);
      iStat += EqnTestInt(_T("const1 < const2"), 1, true);
      iStat += EqnTestInt(_T("const2 > const1"), 1, true);
      iStat += EqnTestInt(_T("const1 <= 1"), 1, true);
      iStat += EqnTestInt(_T("const2 >= 2"), 1, true);
      iStat += EqnTestInt(_T("2*(const1 + const2)"), 6, true);
      iStat += EqnTestInt(_T("2*(const1 - const2)"), -2, true);
      iStat += EqnTestInt(_T("a != b"), 1, true);
      iStat += EqnTestInt(_T("a != b"), 0, false);
      iStat += EqnTestInt(_T("a == b"), 0, true);
      iStat += EqnTestInt(_T("a == 1"), 1, true);
      iStat += EqnTestInt(_T("10*(a == 1)"), 10, true);
      iStat += EqnTestInt(_T("2*(a | b)"), 6, true);
      iStat += EqnTestInt(_T("2*(a | b)"), 7, false);
      iStat += EqnTestInt(_T("a < b"), 1, true);
      iStat += EqnTestInt(_T("b > a"), 1, true);
      iStat += EqnTestInt(_T("a <= 1"), 1, true);
      iStat += EqnTestInt(_T("b >= 2"), 1, true);
      iStat += EqnTestInt(_T("2*(a + b)"), 6, true);
      iStat += EqnTestInt(_T("2*(a - b)"), -2, true);
      iStat += EqnTestInt(_T("a + (a << b)"), 5, true);
      iStat += EqnTestInt(_T("-2^2"), -4, true);
      iStat += EqnTestInt(_T("3--a"), 4, true);
      iStat += EqnTestInt(_T("3+-3^2"), -6, true);

      // Test reading of hex values:
      iStat += EqnTestInt(_T("0xff"), 255, true);
      iStat += EqnTestInt(_T("10+0xff"), 265, true);
      iStat += EqnTestInt(_T("0xff+10"), 265, true);
      iStat += EqnTestInt(_T("10*0xff"), 2550, true);
      iStat += EqnTestInt(_T("0xff*10"), 2550, true);
      iStat += EqnTestInt(_T("10+0xff+1"), 266, true);
      iStat += EqnTestInt(_T("1+0xff+10"), 266, true);

// incorrect: '^' is yor here, not power
//    iStat += EqnTestInt("-(1+2)^2", -9, true);
//    iStat += EqnTestInt("-1^3", -1, true);          

      // Test precedence
      // a=1, b=2, c=3
      iStat += EqnTestInt(_T("a + b * c"), 7, true);
      iStat += EqnTestInt(_T("a * b + c"), 5, true);
      iStat += EqnTestInt(_T("a<b && b>10"), 0, true);
      iStat += EqnTestInt(_T("a<b && b<10"), 1, true);

      iStat += EqnTestInt(_T("a + b << c"), 17, true);
      iStat += EqnTestInt(_T("a << b + c"), 7, true);
      iStat += EqnTestInt(_T("c * b < a"), 0, true);
      iStat += EqnTestInt(_T("c * b == 6 * a"), 1, true);
      iStat += EqnTestInt(_T("2^2^3"), 256, true); 


      if (iStat==0)
        mu::console() << _T("passed") << endl;
      else 
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------------------------
    /** \brief Check muParser name restriction enforcement. */
    int ParserTester::TestNames()
    {
      int  iStat= 0,
           iErr = 0;

      mu::console() << "testing name restriction enforcement...";
    
      Parser p;

  #define PARSER_THROWCHECK(DOMAIN, FAIL, EXPR, ARG) \
      iErr = 0;                                      \
      ParserTester::c_iCount++;                      \
      try                                            \
      {                                              \
        p.Define##DOMAIN(EXPR, ARG);                 \
      }                                              \
      catch(Parser::exception_type&)                 \
      {                                              \
        iErr = (FAIL==false) ? 0 : 1;                \
      }                                              \
      iStat += iErr;      
      
      // constant names
      PARSER_THROWCHECK(Const, false, _T("0a"), 1)
      PARSER_THROWCHECK(Const, false, _T("9a"), 1)
      PARSER_THROWCHECK(Const, false, _T("+a"), 1)
      PARSER_THROWCHECK(Const, false, _T("-a"), 1)
      PARSER_THROWCHECK(Const, false, _T("a-"), 1)
      PARSER_THROWCHECK(Const, false, _T("a*"), 1)
      PARSER_THROWCHECK(Const, false, _T("a?"), 1)
      PARSER_THROWCHECK(Const, true, _T("a"), 1)
      PARSER_THROWCHECK(Const, true, _T("a_min"), 1)
      PARSER_THROWCHECK(Const, true, _T("a_min0"), 1)
      PARSER_THROWCHECK(Const, true, _T("a_min9"), 1)
      // variable names
      value_type a;
      p.ClearConst();
      PARSER_THROWCHECK(Var, false, _T("123abc"), &a)
      PARSER_THROWCHECK(Var, false, _T("9a"), &a)
      PARSER_THROWCHECK(Var, false, _T("0a"), &a)
      PARSER_THROWCHECK(Var, false, _T("+a"), &a)
      PARSER_THROWCHECK(Var, false, _T("-a"), &a)
      PARSER_THROWCHECK(Var, false, _T("?a"), &a)
      PARSER_THROWCHECK(Var, false, _T("!a"), &a)
      PARSER_THROWCHECK(Var, false, _T("a+"), &a)
      PARSER_THROWCHECK(Var, false, _T("a-"), &a)
      PARSER_THROWCHECK(Var, false, _T("a*"), &a)
      PARSER_THROWCHECK(Var, false, _T("a?"), &a)
      PARSER_THROWCHECK(Var, true, _T("a"), &a)
      PARSER_THROWCHECK(Var, true, _T("a_min"), &a)
      PARSER_THROWCHECK(Var, true, _T("a_min0"), &a)
      PARSER_THROWCHECK(Var, true, _T("a_min9"), &a)
      PARSER_THROWCHECK(Var, false, _T("a_min9"), 0)
      // Postfix operators
      // fail
      PARSER_THROWCHECK(PostfixOprt, false, _T("(k"), f1of1)
      PARSER_THROWCHECK(PostfixOprt, false, _T("9+"), f1of1)
      PARSER_THROWCHECK(PostfixOprt, false, _T("+"), 0)
      // pass
      PARSER_THROWCHECK(PostfixOprt, true, _T("-a"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("?a"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("_"),   f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("#"),   f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("&&"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("||"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("&"),   f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("|"),   f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("++"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("--"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("?>"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("?<"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("**"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("xor"), f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("and"), f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("or"),  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("not"), f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, _T("!"),   f1of1)
      // Binary operator
      // The following must fail with builtin operators activated
      // p.EnableBuiltInOp(true); -> this is the default
      p.ClearPostfixOprt();
      PARSER_THROWCHECK(Oprt, false, _T("+"),  f1of2)
      PARSER_THROWCHECK(Oprt, false, _T("-"),  f1of2)
      PARSER_THROWCHECK(Oprt, false, _T("*"),  f1of2)
      PARSER_THROWCHECK(Oprt, false, _T("/"),  f1of2)
      PARSER_THROWCHECK(Oprt, false, _T("^"),  f1of2)
      PARSER_THROWCHECK(Oprt, false, _T("&&"),  f1of2)
      PARSER_THROWCHECK(Oprt, false, _T("||"),  f1of2)
      // without activated built in operators it should work
      p.EnableBuiltInOprt(false);
      PARSER_THROWCHECK(Oprt, true, _T("+"),  f1of2)
      PARSER_THROWCHECK(Oprt, true, _T("-"),  f1of2)
      PARSER_THROWCHECK(Oprt, true, _T("*"),  f1of2)
      PARSER_THROWCHECK(Oprt, true, _T("/"),  f1of2)
      PARSER_THROWCHECK(Oprt, true, _T("^"),  f1of2)
      PARSER_THROWCHECK(Oprt, true, _T("&&"),  f1of2)
      PARSER_THROWCHECK(Oprt, true, _T("||"),  f1of2)
  #undef PARSER_THROWCHECK

      if (iStat==0) 
        mu::console() << _T("passed") << endl;
      else 
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestSyntax()
    {
      int iStat = 0;
      mu::console() << _T("testing syntax engine...");

      iStat += ThrowTest(_T("1,"), ecUNEXPECTED_EOF);  // incomplete hex definition
      iStat += ThrowTest(_T("a,"), ecUNEXPECTED_EOF);  // incomplete hex definition
      iStat += ThrowTest(_T("sin(8),"), ecUNEXPECTED_EOF);  // incomplete hex definition
      iStat += ThrowTest(_T("(sin(8)),"), ecUNEXPECTED_EOF);  // incomplete hex definition
      iStat += ThrowTest(_T("a{m},"), ecUNEXPECTED_EOF);  // incomplete hex definition

      iStat += EqnTest(_T("(1+ 2*a)"), 3, true);   // Spaces within formula
      iStat += EqnTest(_T("sqrt((4))"), 2, true);  // Multiple brackets
      iStat += EqnTest(_T("sqrt((2)+2)"), 2, true);// Multiple brackets
      iStat += EqnTest(_T("sqrt(2+(2))"), 2, true);// Multiple brackets
      iStat += EqnTest(_T("sqrt(a+(3))"), 2, true);// Multiple brackets
      iStat += EqnTest(_T("sqrt((3)+a)"), 2, true);// Multiple brackets
      iStat += EqnTest(_T("order(1,2)"), 1, true); // May not cause name collision with operator "or"
      iStat += EqnTest(_T("(2+"), 0, false);       // missing closing bracket 
      iStat += EqnTest(_T("2++4"), 0, false);      // unexpected operator
      iStat += EqnTest(_T("2+-4"), 0, false);      // unexpected operator
      iStat += EqnTest(_T("(2+)"), 0, false);      // unexpected closing bracket
      iStat += EqnTest(_T("--2"), 0, false);       // double sign
      iStat += EqnTest(_T("ksdfj"), 0, false);     // unknown token
      iStat += EqnTest(_T("()"), 0, false);        // empty bracket without a function
      iStat += EqnTest(_T("5+()"), 0, false);      // empty bracket without a function
      iStat += EqnTest(_T("sin(cos)"), 0, false);  // unexpected function
      iStat += EqnTest(_T("5t6"), 0, false);       // unknown token
      iStat += EqnTest(_T("5 t 6"), 0, false);     // unknown token
      iStat += EqnTest(_T("8*"), 0, false);        // unexpected end of formula
      iStat += EqnTest(_T(",3"), 0, false);        // unexpected comma
      iStat += EqnTest(_T("3,5"), 0, false);       // unexpected comma
      iStat += EqnTest(_T("sin(8,8)"), 0, false);  // too many function args
      iStat += EqnTest(_T("(7,8)"), 0, false);     // too many function args
      iStat += EqnTest(_T("sin)"), 0, false);      // unexpected closing bracket
      iStat += EqnTest(_T("a)"), 0, false);        // unexpected closing bracket
      iStat += EqnTest(_T("pi)"), 0, false);       // unexpected closing bracket
      iStat += EqnTest(_T("sin(())"), 0, false);   // unexpected closing bracket
      iStat += EqnTest(_T("sin()"), 0, false);     // unexpected closing bracket

      if (iStat==0)
        mu::console() << _T("passed") << endl;
      else 
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestVarConst()
    {
      int iStat = 0;
      mu::console() << _T("testing variable/constant detection...");

      // Test if the result changes when a variable changes
      iStat += EqnTestWithVarChange( _T("a"), 1, 1, 2, 2 );
      iStat += EqnTestWithVarChange( _T("2*a"), 2, 4, 3, 6 );

      // distinguish constants with same basename
      iStat += EqnTest( _T("const"), 1, true);
      iStat += EqnTest( _T("const1"), 2, true);
      iStat += EqnTest( _T("const2"), 3, true);
      iStat += EqnTest( _T("2*const"), 2, true);
      iStat += EqnTest( _T("2*const1"), 4, true);
      iStat += EqnTest( _T("2*const2"), 6, true);
      iStat += EqnTest( _T("2*const+1"), 3, true);
      iStat += EqnTest( _T("2*const1+1"), 5, true);
      iStat += EqnTest( _T("2*const2+1"), 7, true);
      iStat += EqnTest( _T("const"), 0, false);
      iStat += EqnTest( _T("const1"), 0, false);
      iStat += EqnTest( _T("const2"), 0, false);

      // distinguish variables with same basename
      iStat += EqnTest( _T("a"), 1, true);
      iStat += EqnTest( _T("aa"), 2, true);
      iStat += EqnTest( _T("2*a"), 2, true);
      iStat += EqnTest( _T("2*aa"), 4, true);
      iStat += EqnTest( _T("2*a-1"), 1, true);
      iStat += EqnTest( _T("2*aa-1"), 3, true);

      // custom value recognition
      iStat += EqnTest( _T("0xff"), 255, true);
      iStat += EqnTest( _T("0x97 + 0xff"), 406, true);

      // Finally test querying of used variables
      try
      {
        int idx;
        mu::Parser p;
        mu::value_type vVarVal[] = { 1, 2, 3, 4, 5};
        p.DefineVar( _T("a"), &vVarVal[0]);
        p.DefineVar( _T("b"), &vVarVal[1]);
        p.DefineVar( _T("c"), &vVarVal[2]);
        p.DefineVar( _T("d"), &vVarVal[3]);
        p.DefineVar( _T("e"), &vVarVal[4]);

        // Test lookup of defined variables
        // 4 used variables
        p.SetExpr( _T("a+b+c+d") );
        mu::varmap_type UsedVar = p.GetUsedVar();
        int iCount = (int)UsedVar.size();
        if (iCount!=4) 
          throw false;
        
        // the next check will fail if the parser 
        // erroneously creates new variables internally
        if (p.GetVar().size()!=5)
          throw false;

        mu::varmap_type::const_iterator item = UsedVar.begin();
        for (idx=0; item!=UsedVar.end(); ++item)
        {
          if (&vVarVal[idx++]!=item->second) 
            throw false;
        }

        // Test lookup of undefined variables
        p.SetExpr( _T("undef1+undef2+undef3") );
        UsedVar = p.GetUsedVar();
        iCount = (int)UsedVar.size();
        if (iCount!=3) 
          throw false;

        // the next check will fail if the parser 
        // erroneously creates new variables internally
        if (p.GetVar().size()!=5)
          throw false;

        for (item = UsedVar.begin(); item!=UsedVar.end(); ++item)
        {
          if (item->second!=0) 
            throw false; // all pointers to undefined variables must be null
        }

        // 1 used variables
        p.SetExpr( _T("a+b") );
        UsedVar = p.GetUsedVar();
        iCount = (int)UsedVar.size();
        if (iCount!=2) throw false;
        item = UsedVar.begin();
        for (idx=0; item!=UsedVar.end(); ++item)
          if (&vVarVal[idx++]!=item->second) throw false;

      }
      catch(...)
      {
        iStat += 1;
      }

      if (iStat==0)  
        mu::console() << _T("passed") << endl;
      else
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestMultiArg()
    {
      int iStat = 0;
      mu::console() << _T("testing multiarg functions...");
    
      // Compound expressions
      iStat += EqnTest( _T("1,2,3"), 3, true);
      iStat += EqnTest( _T("a,b,c"), 3, true);
      iStat += EqnTest( _T("a=10,b=20,c=a*b"), 200, true);
      iStat += EqnTest( _T("1,\n2,\n3"), 3, true);
      iStat += EqnTest( _T("a,\nb,\nc"), 3, true);
      iStat += EqnTest( _T("a=10,\nb=20,\nc=a*b"), 200, true);
      iStat += EqnTest( _T("1,\r\n2,\r\n3"), 3, true);
      iStat += EqnTest( _T("a,\r\nb,\r\nc"), 3, true);
      iStat += EqnTest( _T("a=10,\r\nb=20,\r\nc=a*b"), 200, true);

      // picking the right argument
      iStat += EqnTest( _T("f1of1(1)"), 1, true);
      iStat += EqnTest( _T("f1of2(1, 2)"), 1, true);
      iStat += EqnTest( _T("f2of2(1, 2)"), 2, true);
      iStat += EqnTest( _T("f1of3(1, 2, 3)"), 1, true);
      iStat += EqnTest( _T("f2of3(1, 2, 3)"), 2, true);
      iStat += EqnTest( _T("f3of3(1, 2, 3)"), 3, true);
      iStat += EqnTest( _T("f1of4(1, 2, 3, 4)"), 1, true);
      iStat += EqnTest( _T("f2of4(1, 2, 3, 4)"), 2, true);
      iStat += EqnTest( _T("f3of4(1, 2, 3, 4)"), 3, true);
      iStat += EqnTest( _T("f4of4(1, 2, 3, 4)"), 4, true);
      iStat += EqnTest( _T("f1of5(1, 2, 3, 4, 5)"), 1, true);
      iStat += EqnTest( _T("f2of5(1, 2, 3, 4, 5)"), 2, true);
      iStat += EqnTest( _T("f3of5(1, 2, 3, 4, 5)"), 3, true);
      iStat += EqnTest( _T("f4of5(1, 2, 3, 4, 5)"), 4, true);
      iStat += EqnTest( _T("f5of5(1, 2, 3, 4, 5)"), 5, true);
      // Too few arguments / Too many arguments
      iStat += EqnTest( _T("1+ping()"), 11, true);
      iStat += EqnTest( _T("ping()+1"), 11, true);
      iStat += EqnTest( _T("2*ping()"), 20, true);
      iStat += EqnTest( _T("ping()*2"), 20, true);
      iStat += EqnTest( _T("ping(1,2)"), 0, false);
      iStat += EqnTest( _T("1+ping(1,2)"), 0, false);
      iStat += EqnTest( _T("f1of1(1,2)"), 0, false);
      iStat += EqnTest( _T("f1of1()"), 0, false);
      iStat += EqnTest( _T("f1of2(1, 2, 3)"), 0, false);
      iStat += EqnTest( _T("f1of2(1)"), 0, false);
      iStat += EqnTest( _T("f1of3(1, 2, 3, 4)"), 0, false);
      iStat += EqnTest( _T("f1of3(1)"), 0, false);
      iStat += EqnTest( _T("f1of4(1, 2, 3, 4, 5)"), 0, false);
      iStat += EqnTest( _T("f1of4(1)"), 0, false);
      iStat += EqnTest( _T("(1,2,3)"), 0, false);
      iStat += EqnTest( _T("1,2,3"), 0, false);
      iStat += EqnTest( _T("(1*a,2,3)"), 0, false);
      iStat += EqnTest( _T("1,2*a,3"), 0, false);
     
      // correct calculation of arguments
      iStat += EqnTest( _T("min(a, 1)"),  1, true);
      iStat += EqnTest( _T("min(3*2, 1)"),  1, true);
      iStat += EqnTest( _T("min(3*2, 1)"),  6, false);
      iStat += EqnTest( _T("firstArg(2,3,4)"), 2, true);
      iStat += EqnTest( _T("lastArg(2,3,4)"), 4, true);
      iStat += EqnTest( _T("min(3*a+1, 1)"),  1, true);
      iStat += EqnTest( _T("max(3*a+1, 1)"),  4, true);
      iStat += EqnTest( _T("max(3*a+1, 1)*2"),  8, true);
      iStat += EqnTest( _T("2*max(3*a+1, 1)+2"),  10, true);

      // functions with Variable argument count
      iStat += EqnTest( _T("sum(a)"), 1, true);
      iStat += EqnTest( _T("sum(1,2,3)"),  6, true);
      iStat += EqnTest( _T("sum(a,b,c)"),  6, true);
      iStat += EqnTest( _T("sum(1,-max(1,2),3)*2"),  4, true);
      iStat += EqnTest( _T("2*sum(1,2,3)"),  12, true);
      iStat += EqnTest( _T("2*sum(1,2,3)+2"),  14, true);
      iStat += EqnTest( _T("2*sum(-1,2,3)+2"),  10, true);
      iStat += EqnTest( _T("2*sum(-1,2,-(-a))+2"),  6, true);
      iStat += EqnTest( _T("2*sum(-1,10,-a)+2"),  18, true);
      iStat += EqnTest( _T("2*sum(1,2,3)*2"),  24, true);
      iStat += EqnTest( _T("sum(1,-max(1,2),3)*2"),  4, true);
      iStat += EqnTest( _T("sum(1*3, 4, a+2)"),  10, true);
      iStat += EqnTest( _T("sum(1*3, 2*sum(1,2,2), a+2)"),  16, true);
      iStat += EqnTest( _T("sum(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2)"), 24, true);

      // some failures
      iStat += EqnTest( _T("sum()"),  0, false);
      iStat += EqnTest( _T("sum(,)"),  0, false);
      iStat += EqnTest( _T("sum(1,2,)"),  0, false);
      iStat += EqnTest( _T("sum(,1,2)"),  0, false);

      if (iStat==0) 
        mu::console() << _T("passed") << endl;
      else
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;
  
      return iStat;
    }


    //---------------------------------------------------------------------------
    int ParserTester::TestInfixOprt()
    {
      int iStat(0);
      mu::console() << "testing infix operators...";

      iStat += EqnTest( _T("+1"),    +1, true);
      iStat += EqnTest( _T("-(+1)"), -1, true);
      iStat += EqnTest( _T("-(+1)*2"),  -2, true);
      iStat += EqnTest( _T("-(+2)*sqrt(4)"),  -4, true);
      iStat += EqnTest( _T("3-+a"), 2, true);
      iStat += EqnTest( _T("+1*3"),  3, true);

      iStat += EqnTest( _T("-1"),    -1, true);
      iStat += EqnTest( _T("-(-1)"),  1, true);
      iStat += EqnTest( _T("-(-1)*2"),  2, true);
      iStat += EqnTest( _T("-(-2)*sqrt(4)"),  4, true);
      iStat += EqnTest( _T("-_pi"), -PARSER_CONST_PI, true);
      iStat += EqnTest( _T("-a"),  -1, true);
      iStat += EqnTest( _T("-(a)"),  -1, true);
      iStat += EqnTest( _T("-(-a)"),  1, true);
      iStat += EqnTest( _T("-(-a)*2"),  2, true);
      iStat += EqnTest( _T("-(8)"), -8, true);
      iStat += EqnTest( _T("-8"), -8, true);
      iStat += EqnTest( _T("-(2+1)"), -3, true);
      iStat += EqnTest( _T("-(f1of1(1+2*3)+1*2)"), -9, true);
      iStat += EqnTest( _T("-(-f1of1(1+2*3)+1*2)"), 5, true);
      iStat += EqnTest( _T("-sin(8)"), -0.989358, true);
      iStat += EqnTest( _T("3-(-a)"), 4, true);
      iStat += EqnTest( _T("3--a"), 4, true);
      iStat += EqnTest( _T("-1*3"),  -3, true);

      // Postfix / infix priorities
      iStat += EqnTest( _T("~2#"), 8, true);
      iStat += EqnTest( _T("~f1of1(2)#"), 8, true);
      iStat += EqnTest( _T("~(b)#"), 8, true);
      iStat += EqnTest( _T("(~b)#"), 12, true);
      iStat += EqnTest( _T("~(2#)"), 8, true);
      iStat += EqnTest( _T("~(f1of1(2)#)"), 8, true);
      //
      iStat += EqnTest( _T("-2^2"),-4, true);
      iStat += EqnTest( _T("-(a+b)^2"),-9, true);
      iStat += EqnTest( _T("(-3)^2"),9, true);
      iStat += EqnTest( _T("-(-2^2)"),4, true);
      iStat += EqnTest( _T("3+-3^2"),-6, true);
      // The following assumes use of sqr as postfix operator together
      // with a sign operator of low priority:
      iStat += EqnTest( _T("-2'"), -4, true);
      iStat += EqnTest( _T("-(1+1)'"),-4, true);
      iStat += EqnTest( _T("2+-(1+1)'"),-2, true);
      iStat += EqnTest( _T("2+-2'"), -2, true);
      // This is the classic behaviour of the infix sign operator (here: "$") which is
      // now deprecated:
      iStat += EqnTest( _T("$2^2"),4, true);
      iStat += EqnTest( _T("$(a+b)^2"),9, true);
      iStat += EqnTest( _T("($3)^2"),9, true);
      iStat += EqnTest( _T("$($2^2)"),-4, true);
      iStat += EqnTest( _T("3+$3^2"),12, true);

      // infix operators sharing the first few characters
      iStat += EqnTest( _T("~ 123"),  123+2, true);
      iStat += EqnTest( _T("~~ 123"),  123+2, true);

      if (iStat==0)
        mu::console() << _T("passed") << endl;
      else
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }


    //---------------------------------------------------------------------------
    int ParserTester::TestPostFix()
    {
      int iStat = 0;
      mu::console() << _T("testing postfix operators...");

      // application
      iStat += EqnTest( _T("3{m}+5"), 5.003, true);
      iStat += EqnTest( _T("1000{m}"), 1, true);
      iStat += EqnTest( _T("1000 {m}"), 1, true);
      iStat += EqnTest( _T("(a){m}"), 1e-3, true);
      iStat += EqnTest( _T("a{m}"), 1e-3, true);
      iStat += EqnTest( _T("a {m}"), 1e-3, true);
      iStat += EqnTest( _T("-(a){m}"), -1e-3, true);
      iStat += EqnTest( _T("-2{m}"), -2e-3, true);
      iStat += EqnTest( _T("-2 {m}"), -2e-3, true);
      iStat += EqnTest( _T("f1of1(1000){m}"), 1, true);
      iStat += EqnTest( _T("-f1of1(1000){m}"), -1, true);
      iStat += EqnTest( _T("-f1of1(-1000){m}"), 1, true);
      iStat += EqnTest( _T("f4of4(0,0,0,1000){m}"), 1, true);
      iStat += EqnTest( _T("2+(a*1000){m}"), 3, true);

      // can postfix operators "m" und "meg" be told apart properly?
      iStat += EqnTest( _T("2*3000meg+2"), 2*3e9+2, true);   

      // some incorrect results
      iStat += EqnTest( _T("1000{m}"), 0.1, false);
      iStat += EqnTest( _T("(a){m}"), 2, false);
      // failure due to syntax checking
      iStat += ThrowTest(_T("0x"), ecUNASSIGNABLE_TOKEN);  // incomplete hex definition
      iStat += ThrowTest(_T("3+"), ecUNEXPECTED_EOF);
      iStat += ThrowTest( _T("4 + {m}"), ecUNASSIGNABLE_TOKEN);
      iStat += ThrowTest( _T("{m}4"), ecUNASSIGNABLE_TOKEN);
      iStat += ThrowTest( _T("sin({m})"), ecUNASSIGNABLE_TOKEN);
      iStat += ThrowTest( _T("{m} {m}"), ecUNASSIGNABLE_TOKEN);
      iStat += ThrowTest( _T("{m}(8)"), ecUNASSIGNABLE_TOKEN);
      iStat += ThrowTest( _T("4,{m}"), ecUNASSIGNABLE_TOKEN);
      iStat += ThrowTest( _T("-{m}"), ecUNASSIGNABLE_TOKEN);
      iStat += ThrowTest( _T("2(-{m})"), ecUNEXPECTED_PARENS);
      iStat += ThrowTest( _T("2({m})"), ecUNEXPECTED_PARENS);
 
      iStat += ThrowTest( _T("multi*1.0"), ecUNASSIGNABLE_TOKEN);

      if (iStat==0)
        mu::console() << _T("passed") << endl;
      else
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestExpression()
    {
      int iStat = 0;
      mu::console() << _T("testing expression samples...");

      value_type b = 2;

      // Optimization
      iStat += EqnTest( _T("2*b*5"), 20, true);
      iStat += EqnTest( _T("2*b*5 + 4*b"), 28, true);
      iStat += EqnTest( _T("2*a/3"), 2.0/3.0, true);

      // Addition auf cmVARMUL 
      iStat += EqnTest( _T("3+b"), b+3, true);
      iStat += EqnTest( _T("b+3"), b+3, true);
      iStat += EqnTest( _T("b*3+2"), b*3+2, true);
      iStat += EqnTest( _T("3*b+2"), b*3+2, true);
      iStat += EqnTest( _T("2+b*3"), b*3+2, true);
      iStat += EqnTest( _T("2+3*b"), b*3+2, true);
      iStat += EqnTest( _T("b+3*b"), b+3*b, true);
      iStat += EqnTest( _T("3*b+b"), b+3*b, true);

      iStat += EqnTest( _T("2+b*3+b"), 2+b*3+b, true);
      iStat += EqnTest( _T("b+2+b*3"), b+2+b*3, true);

      iStat += EqnTest( _T("(2*b+1)*4"), (2*b+1)*4, true);
      iStat += EqnTest( _T("4*(2*b+1)"), (2*b+1)*4, true);

      // operator precedences
      iStat += EqnTest( _T("1+2-3*4/5^6"), 2.99923, true);
      iStat += EqnTest( _T("1^2/3*4-5+6"), 2.33333333, true);
      iStat += EqnTest( _T("1+2*3"), 7, true);
      iStat += EqnTest( _T("1+2*3"), 7, true);
      iStat += EqnTest( _T("(1+2)*3"), 9, true);
      iStat += EqnTest( _T("(1+2)*(-3)"), -9, true);
      iStat += EqnTest( _T("2/4"), 0.5, true);

      iStat += EqnTest( _T("exp(ln(7))"), 7, true);
      iStat += EqnTest( _T("e^ln(7)"), 7, true);
      iStat += EqnTest( _T("e^(ln(7))"), 7, true);
      iStat += EqnTest( _T("(e^(ln(7)))"), 7, true);
      iStat += EqnTest( _T("1-(e^(ln(7)))"), -6, true);
      iStat += EqnTest( _T("2*(e^(ln(7)))"), 14, true);
      iStat += EqnTest( _T("10^log(5)"), pow(10.0, log(5.0)), true);
      iStat += EqnTest( _T("10^log10(5)"), 5, true);
      iStat += EqnTest( _T("2^log2(4)"), 4, true);
      iStat += EqnTest( _T("-(sin(0)+1)"), -1, true);
      iStat += EqnTest( _T("-(2^1.1)"), -2.14354692, true);

      iStat += EqnTest( _T("(cos(2.41)/b)"), -0.372056, true);
      iStat += EqnTest( _T("(1*(2*(3*(4*(5*(6*(a+b)))))))"), 2160, true);
      iStat += EqnTest( _T("(1*(2*(3*(4*(5*(6*(7*(a+b))))))))"), 15120, true);
      iStat += EqnTest( _T("(a/((((b+(((e*(((((pi*((((3.45*((pi+a)+pi))+b)+b)*a))+0.68)+e)+a)/a))+a)+b))+b)*a)-pi))"), 0.00377999, true);

      // long formula (Reference: Matlab)
      iStat += EqnTest(
        _T("(((-9))-e/(((((((pi-(((-7)+(-3)/4/e))))/(((-5))-2)-((pi+(-0))*(sqrt((e+e))*(-8))*(((-pi)+(-pi)-(-9)*(6*5))")
        _T("/(-e)-e))/2)/((((sqrt(2/(-e)+6)-(4-2))+((5/(-2))/(1*(-pi)+3))/8)*pi*((pi/((-2)/(-6)*1*(-1))*(-6)+(-e)))))/")
        _T("((e+(-2)+(-e)*((((-3)*9+(-e)))+(-9)))))))-((((e-7+(((5/pi-(3/1+pi)))))/e)/(-5))/(sqrt((((((1+(-7))))+((((-")
        _T("e)*(-e)))-8))*(-5)/((-e)))*(-6)-((((((-2)-(-9)-(-e)-1)/3))))/(sqrt((8+(e-((-6))+(9*(-9))))*(((3+2-8))*(7+6")
        _T("+(-5))+((0/(-e)*(-pi))+7)))+(((((-e)/e/e)+((-6)*5)*e+(3+(-5)/pi))))+pi))/sqrt((((9))+((((pi))-8+2))+pi))/e")
        _T("*4)*((-5)/(((-pi))*(sqrt(e)))))-(((((((-e)*(e)-pi))/4+(pi)*(-9)))))))+(-pi)"), -12.23016549, true);

      // long formula (Reference: Matlab)
      iStat += EqnTest(
          _T("(atan(sin((((((((((((((((pi/cos((a/((((0.53-b)-pi)*e)/b))))+2.51)+a)-0.54)/0.98)+b)*b)+e)/a)+b)+a)+b)+pi)/e")
          _T(")+a)))*2.77)"), -2.16995656, true);

      // long formula (Reference: Matlab)
      iStat += EqnTest( _T("1+2-3*4/5^6*(2*(1-5+(3*7^9)*(4+6*7-3)))+12"), -7995810.09926, true);
	  
      if (iStat==0) 
        mu::console() << _T("passed") << endl;  
      else 
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }



    //---------------------------------------------------------------------------
    int ParserTester::TestIfThenElse()
    {
      int iStat = 0;
      mu::console() << _T("testing if-then-else operator...");

      // Test error detection
      iStat += ThrowTest(_T(":3"), ecUNEXPECTED_CONDITIONAL); 
      iStat += ThrowTest(_T("? 1 : 2"), ecUNEXPECTED_CONDITIONAL); 
      iStat += ThrowTest(_T("(a<b) ? (b<c) ? 1 : 2"), ecMISSING_ELSE_CLAUSE); 
      iStat += ThrowTest(_T("(a<b) ? 1"), ecMISSING_ELSE_CLAUSE); 
      iStat += ThrowTest(_T("(a<b) ? a"), ecMISSING_ELSE_CLAUSE); 
      iStat += ThrowTest(_T("(a<b) ? a+b"), ecMISSING_ELSE_CLAUSE); 
      iStat += ThrowTest(_T("a : b"), ecMISPLACED_COLON); 
      iStat += ThrowTest(_T("1 : 2"), ecMISPLACED_COLON); 
      iStat += ThrowTest(_T("(1) ? 1 : 2 : 3"), ecMISPLACED_COLON); 
      iStat += ThrowTest(_T("(true) ? 1 : 2 : 3"), ecUNASSIGNABLE_TOKEN); 

      iStat += EqnTest(_T("1 ? 128 : 255"), 128, true);
      iStat += EqnTest(_T("1<2 ? 128 : 255"), 128, true);
      iStat += EqnTest(_T("a<b ? 128 : 255"), 128, true);
      iStat += EqnTest(_T("(a<b) ? 128 : 255"), 128, true);
      iStat += EqnTest(_T("(1) ? 10 : 11"), 10, true);
      iStat += EqnTest(_T("(0) ? 10 : 11"), 11, true);
      iStat += EqnTest(_T("(1) ? a+b : c+d"), 3, true);
      iStat += EqnTest(_T("(0) ? a+b : c+d"), 1, true);
      iStat += EqnTest(_T("(1) ? 0 : 1"), 0, true);
      iStat += EqnTest(_T("(0) ? 0 : 1"), 1, true);
      iStat += EqnTest(_T("(a<b) ? 10 : 11"), 10, true);
      iStat += EqnTest(_T("(a>b) ? 10 : 11"), 11, true);
      iStat += EqnTest(_T("(a<b) ? c : d"), 3, true);
      iStat += EqnTest(_T("(a>b) ? c : d"), -2, true);

      iStat += EqnTest(_T("(a>b) ? 1 : 0"), 0, true);
      iStat += EqnTest(_T("((a>b) ? 1 : 0) ? 1 : 2"), 2, true);
      iStat += EqnTest(_T("((a>b) ? 1 : 0) ? 1 : sum((a>b) ? 1 : 2)"), 2, true);
      iStat += EqnTest(_T("((a>b) ? 0 : 1) ? 1 : sum((a>b) ? 1 : 2)"), 1, true);

      iStat += EqnTest(_T("sum((a>b) ? 1 : 2)"), 2, true);
      iStat += EqnTest(_T("sum((1) ? 1 : 2)"), 1, true);
      iStat += EqnTest(_T("sum((a>b) ? 1 : 2, 100)"), 102, true);
      iStat += EqnTest(_T("sum((1) ? 1 : 2, 100)"), 101, true);
      iStat += EqnTest(_T("sum(3, (a>b) ? 3 : 10)"), 13, true);
      iStat += EqnTest(_T("sum(3, (a<b) ? 3 : 10)"), 6, true);
      iStat += EqnTest(_T("10*sum(3, (a>b) ? 3 : 10)"), 130, true);
      iStat += EqnTest(_T("10*sum(3, (a<b) ? 3 : 10)"), 60, true);
      iStat += EqnTest(_T("sum(3, (a>b) ? 3 : 10)*10"), 130, true);
      iStat += EqnTest(_T("sum(3, (a<b) ? 3 : 10)*10"), 60, true);
      iStat += EqnTest(_T("(a<b) ? sum(3, (a<b) ? 3 : 10)*10 : 99"), 60, true);
      iStat += EqnTest(_T("(a>b) ? sum(3, (a<b) ? 3 : 10)*10 : 99"), 99, true);
      iStat += EqnTest(_T("(a<b) ? sum(3, (a<b) ? 3 : 10,10,20)*10 : 99"), 360, true);
      iStat += EqnTest(_T("(a>b) ? sum(3, (a<b) ? 3 : 10,10,20)*10 : 99"), 99, true);
      iStat += EqnTest(_T("(a>b) ? sum(3, (a<b) ? 3 : 10,10,20)*10 : sum(3, (a<b) ? 3 : 10)*10"), 60, true);

      // todo: also add for muParserX!
      iStat += EqnTest(_T("(a<b)&&(a<b) ? 128 : 255"), 128, true);
      iStat += EqnTest(_T("(a>b)&&(a<b) ? 128 : 255"), 255, true);
      iStat += EqnTest(_T("(1<2)&&(1<2) ? 128 : 255"), 128, true);
      iStat += EqnTest(_T("(1>2)&&(1<2) ? 128 : 255"), 255, true);
      iStat += EqnTest(_T("((1<2)&&(1<2)) ? 128 : 255"), 128, true);
      iStat += EqnTest(_T("((1>2)&&(1<2)) ? 128 : 255"), 255, true);
      iStat += EqnTest(_T("((a<b)&&(a<b)) ? 128 : 255"), 128, true);
      iStat += EqnTest(_T("((a>b)&&(a<b)) ? 128 : 255"), 255, true);

      iStat += EqnTest(_T("1>0 ? 1>2 ? 128 : 255 : 1>0 ? 32 : 64"), 255, true);
      iStat += EqnTest(_T("1>0 ? 1>2 ? 128 : 255 :(1>0 ? 32 : 64)"), 255, true);
      iStat += EqnTest(_T("1>0 ? 1>0 ? 128 : 255 : 1>2 ? 32 : 64"), 128, true);
      iStat += EqnTest(_T("1>0 ? 1>0 ? 128 : 255 :(1>2 ? 32 : 64)"), 128, true);
      iStat += EqnTest(_T("1>2 ? 1>2 ? 128 : 255 : 1>0 ? 32 : 64"), 32, true);
      iStat += EqnTest(_T("1>2 ? 1>0 ? 128 : 255 : 1>2 ? 32 : 64"), 64, true);
      iStat += EqnTest(_T("1>0 ? 50 :  1>0 ? 128 : 255"), 50, true);
      iStat += EqnTest(_T("1>0 ? 50 : (1>0 ? 128 : 255)"), 50, true);
      iStat += EqnTest(_T("1>0 ? 1>0 ? 128 : 255 : 50"), 128, true);
      iStat += EqnTest(_T("1>2 ? 1>2 ? 128 : 255 : 1>0 ? 32 : 1>2 ? 64 : 16"), 32, true);
      iStat += EqnTest(_T("1>2 ? 1>2 ? 128 : 255 : 1>0 ? 32 :(1>2 ? 64 : 16)"), 32, true);
      iStat += EqnTest(_T("1>0 ? 1>2 ? 128 : 255 :  1>0 ? 32 :1>2 ? 64 : 16"), 255, true);
      iStat += EqnTest(_T("1>0 ? 1>2 ? 128 : 255 : (1>0 ? 32 :1>2 ? 64 : 16)"), 255, true);
      iStat += EqnTest(_T("1 ? 0 ? 128 : 255 : 1 ? 32 : 64"), 255, true);

      // assignment operators
      iStat += EqnTest(_T("a= 0 ? 128 : 255, a"), 255, true);
      iStat += EqnTest(_T("a=((a>b)&&(a<b)) ? 128 : 255, a"), 255, true);
      iStat += EqnTest(_T("c=(a<b)&&(a<b) ? 128 : 255, c"), 128, true);
      iStat += EqnTest(_T("0 ? a=a+1 : 666, a"), 1, true);
      iStat += EqnTest(_T("1?a=10:a=20, a"), 10, true);
      iStat += EqnTest(_T("0?a=10:a=20, a"), 20, true);
      iStat += EqnTest(_T("0?a=sum(3,4):10, a"), 1, true);  // a should not change its value due to lazy calculation
      
      iStat += EqnTest(_T("a=1?b=1?3:4:5, a"), 3, true);
      iStat += EqnTest(_T("a=1?b=1?3:4:5, b"), 3, true);
      iStat += EqnTest(_T("a=0?b=1?3:4:5, a"), 5, true);
      iStat += EqnTest(_T("a=0?b=1?3:4:5, b"), 2, true);

      iStat += EqnTest(_T("a=1?5:b=1?3:4, a"), 5, true);
      iStat += EqnTest(_T("a=1?5:b=1?3:4, b"), 2, true);
      iStat += EqnTest(_T("a=0?5:b=1?3:4, a"), 3, true);
      iStat += EqnTest(_T("a=0?5:b=1?3:4, b"), 3, true);

      if (iStat==0) 
        mu::console() << _T("passed") << endl;  
      else 
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestException()
    {
      int  iStat = 0;
      mu::console() << _T("testing error codes...");

      iStat += ThrowTest(_T("3+"),           ecUNEXPECTED_EOF);
      iStat += ThrowTest(_T("3+)"),          ecUNEXPECTED_PARENS);
      iStat += ThrowTest(_T("()"),           ecUNEXPECTED_PARENS);
      iStat += ThrowTest(_T("3+()"),         ecUNEXPECTED_PARENS);
      iStat += ThrowTest(_T("sin(3,4)"),     ecTOO_MANY_PARAMS);
      iStat += ThrowTest(_T("sin()"),        ecTOO_FEW_PARAMS);
      iStat += ThrowTest(_T("(1+2"),         ecMISSING_PARENS);
      iStat += ThrowTest(_T("sin(3)3"),      ecUNEXPECTED_VAL);
      iStat += ThrowTest(_T("sin(3)xyz"),    ecUNASSIGNABLE_TOKEN);
      iStat += ThrowTest(_T("sin(3)cos(3)"), ecUNEXPECTED_FUN);
      iStat += ThrowTest(_T("a+b+c=10"),     ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest(_T("a=b=3"),        ecUNEXPECTED_OPERATOR);
      
#if defined(MUP_MATH_EXCEPTIONS)
      // divide by zero whilst constant folding
      iStat += ThrowTest(_T("1/0"),          ecDIV_BY_ZERO);
      // square root of a negative number
      iStat += ThrowTest(_T("sqrt(-1)"),     ecDOMAIN_ERROR);
      // logarithms of zero
      iStat += ThrowTest(_T("ln(0)"),        ecDOMAIN_ERROR);
      iStat += ThrowTest(_T("log2(0)"),      ecDOMAIN_ERROR);
      iStat += ThrowTest(_T("log10(0)"),     ecDOMAIN_ERROR);
      iStat += ThrowTest(_T("log(0)"),       ecDOMAIN_ERROR);
      // logarithms of negative values
      iStat += ThrowTest(_T("ln(-1)"),       ecDOMAIN_ERROR);
      iStat += ThrowTest(_T("log2(-1)"),     ecDOMAIN_ERROR);
      iStat += ThrowTest(_T("log10(-1)"),    ecDOMAIN_ERROR);
      iStat += ThrowTest(_T("log(-1)"),      ecDOMAIN_ERROR);
#endif

      // functions without parameter
      iStat += ThrowTest( _T("3+ping(2)"),    ecTOO_MANY_PARAMS);
      iStat += ThrowTest( _T("3+ping(a+2)"),  ecTOO_MANY_PARAMS);
      iStat += ThrowTest( _T("3+ping(sin(a)+2)"),  ecTOO_MANY_PARAMS);
      iStat += ThrowTest( _T("3+ping(1+sin(a))"),  ecTOO_MANY_PARAMS);

      // String function related
      iStat += ThrowTest( _T("valueof(\"xxx\")"),  999, false);
      iStat += ThrowTest( _T("valueof()"),          ecUNEXPECTED_PARENS);
      iStat += ThrowTest( _T("1+valueof(\"abc\""),  ecMISSING_PARENS);
      iStat += ThrowTest( _T("valueof(\"abc\""),    ecMISSING_PARENS);
      iStat += ThrowTest( _T("valueof(\"abc"),      ecUNTERMINATED_STRING);
      iStat += ThrowTest( _T("valueof(\"abc\",3)"), ecTOO_MANY_PARAMS);
      iStat += ThrowTest( _T("valueof(3)"),         ecSTRING_EXPECTED);
      iStat += ThrowTest( _T("sin(\"abc\")"),       ecVAL_EXPECTED);
      iStat += ThrowTest( _T("valueof(\"\\\"abc\\\"\")"),  999, false);
      iStat += ThrowTest( _T("\"hello world\""),    ecSTR_RESULT);
      iStat += ThrowTest( _T("(\"hello world\")"),  ecSTR_RESULT);
      iStat += ThrowTest( _T("\"abcd\"+100"),       ecOPRT_TYPE_CONFLICT);
      iStat += ThrowTest( _T("\"a\"+\"b\""),        ecOPRT_TYPE_CONFLICT);
      iStat += ThrowTest( _T("strfun1(\"100\",3)"),     ecTOO_MANY_PARAMS);
      iStat += ThrowTest( _T("strfun2(\"100\",3,5)"),   ecTOO_MANY_PARAMS);
      iStat += ThrowTest( _T("strfun3(\"100\",3,5,6)"), ecTOO_MANY_PARAMS);
      iStat += ThrowTest( _T("strfun2(\"100\")"),       ecTOO_FEW_PARAMS);
      iStat += ThrowTest( _T("strfun3(\"100\",6)"),   ecTOO_FEW_PARAMS);
      iStat += ThrowTest( _T("strfun2(1,1)"),         ecSTRING_EXPECTED);
      iStat += ThrowTest( _T("strfun2(a,1)"),         ecSTRING_EXPECTED);
      iStat += ThrowTest( _T("strfun2(1,1,1)"),       ecTOO_MANY_PARAMS);
      iStat += ThrowTest( _T("strfun2(a,1,1)"),       ecTOO_MANY_PARAMS);
      iStat += ThrowTest( _T("strfun3(1,2,3)"),         ecSTRING_EXPECTED);
      iStat += ThrowTest( _T("strfun3(1, \"100\",3)"),  ecSTRING_EXPECTED);
      iStat += ThrowTest( _T("strfun3(\"1\", \"100\",3)"),  ecVAL_EXPECTED);
      iStat += ThrowTest( _T("strfun3(\"1\", 3, \"100\")"),  ecVAL_EXPECTED);
      iStat += ThrowTest( _T("strfun3(\"1\", \"100\", \"100\", \"100\")"),  ecTOO_MANY_PARAMS);

      // assignment operator
      iStat += ThrowTest( _T("3=4"), ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest( _T("sin(8)=4"), ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest( _T("\"test\"=a"), ecUNEXPECTED_OPERATOR);

      // <ibg 20090529>
      // this is now legal, for reference see:
      // https://sourceforge.net/forum/message.php?msg_id=7411373
      //      iStat += ThrowTest( _T("sin=9"), ecUNEXPECTED_OPERATOR);    
      // </ibg>

      iStat += ThrowTest( _T("(8)=5"), ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest( _T("(a)=5"), ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest( _T("a=\"tttt\""), ecOPRT_TYPE_CONFLICT);

      if (iStat==0) 
        mu::console() << _T("passed") << endl;
      else 
        mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

      return iStat;
    }


    //---------------------------------------------------------------------------
    void ParserTester::AddTest(testfun_type a_pFun)
    {
      m_vTestFun.push_back(a_pFun);
    }

    //---------------------------------------------------------------------------
    void ParserTester::Run()
    {
      int iStat = 0;
      try
      {
        for (int i=0; i<(int)m_vTestFun.size(); ++i)
          iStat += (this->*m_vTestFun[i])();
      }
      catch(Parser::exception_type &e)
      {
        mu::console() << "\n" << e.GetMsg() << endl;
        mu::console() << e.GetToken() << endl;
        Abort();
      }
      catch(std::exception &e)
      {
        mu::console() << e.what() << endl;
        Abort();
      }
      catch(...)
      {
        mu::console() << "Internal error";
        Abort();
      }

      if (iStat==0) 
      {
        mu::console() << "Test passed (" <<  ParserTester::c_iCount << " expressions)" << endl;
      }
      else 
      {
        mu::console() << "Test failed with " << iStat 
                  << " errors (" <<  ParserTester::c_iCount 
                  << " expressions)" << endl;
      }
      ParserTester::c_iCount = 0;
    }


    //---------------------------------------------------------------------------
    int ParserTester::ThrowTest(const string_type &a_str, int a_iErrc, bool a_bFail)
    {
      ParserTester::c_iCount++;

      try
      {
        value_type fVal[] = {1,1,1};
        Parser p;

        p.DefineVar( _T("a"), &fVal[0]);
        p.DefineVar( _T("b"), &fVal[1]);
        p.DefineVar( _T("c"), &fVal[2]);
        p.DefinePostfixOprt( _T("{m}"), Milli);
        p.DefinePostfixOprt( _T("m"), Milli);
        p.DefineFun( _T("ping"), Ping);
        p.DefineFun( _T("valueof"), ValueOf);
        p.DefineFun( _T("strfun1"), StrFun1);
        p.DefineFun( _T("strfun2"), StrFun2);
        p.DefineFun( _T("strfun3"), StrFun3);
        p.SetExpr(a_str);
        p.Eval();
      }
      catch(ParserError &e)
      {
        // output the formula in case of an failed test
        if (a_bFail==false || (a_bFail==true && a_iErrc!=e.GetCode()) )
        {
          mu::console() << _T("\n  ") 
                        << _T("Expression: ") << a_str 
                        << _T("  Code:") << e.GetCode() << _T("(") << e.GetMsg() << _T(")")
                        << _T("  Expected:") << a_iErrc;
        }

        return (a_iErrc==e.GetCode()) ? 0 : 1;
      }

      // if a_bFail==false no exception is expected
      bool bRet((a_bFail==false) ? 0 : 1);
      if (bRet==1)
      {
        mu::console() << _T("\n  ") 
                      << _T("Expression: ") << a_str 
                      << _T("  did evaluate; Expected error:") << a_iErrc;
      }

      return bRet; 
    }

    //---------------------------------------------------------------------------
    /** \brief Evaluate a tet expression. 

        \return 1 in case of a failure, 0 otherwise.
    */
    int ParserTester::EqnTestWithVarChange(const string_type &a_str, 
                                           double a_fVar1, 
                                           double a_fRes1, 
                                           double a_fVar2, 
                                           double a_fRes2)
    {
      ParserTester::c_iCount++;

      try
      {
        value_type fVal[2] = {-999, -999 }; // should be equal
	  
        Parser  p;
        value_type var = 0;

        // variable
        p.DefineVar( _T("a"), &var);
        p.SetExpr(a_str);

        var = a_fVar1;
        fVal[0] = p.Eval();

        var = a_fVar2;
        fVal[1] = p.Eval();
        
        if ( fabs(a_fRes1-fVal[0]) > 0.0000000001)
          throw std::runtime_error("incorrect result (first pass)");

        if ( fabs(a_fRes2-fVal[1]) > 0.0000000001)
          throw std::runtime_error("incorrect result (second pass)");
      }
      catch(Parser::exception_type &e)
      {
        mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.GetMsg() << _T(")");
        return 1;
      }
      catch(std::exception &e)
      {
        mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.what() << _T(")");
        return 1;  // always return a failure since this exception is not expected
      }
      catch(...)
      {
        mu::console() << _T("\n  fail: ") << a_str.c_str() <<  _T(" (unexpected exception)");
        return 1;  // exceptions other than ParserException are not allowed
      }

      return 0;
    }

    //---------------------------------------------------------------------------
    /** \brief Evaluate a tet expression. 

        \return 1 in case of a failure, 0 otherwise.
    */
    int ParserTester::EqnTest(const string_type &a_str, double a_fRes, bool a_fPass)
    {
      ParserTester::c_iCount++;
      int iRet(0);
      value_type fVal[5] = {-999, -998, -997, -996, -995}; // initially should be different

      try
      {
        std::unique_ptr<Parser> p1;
        Parser  p2, p3;   // three parser objects
                          // they will be used for testing copy and assignment operators
        // p1 is a pointer since i'm going to delete it in order to test if
        // parsers after copy construction still refer to members of it.
        // !! If this is the case this function will crash !!
      
        p1.reset(new mu::Parser()); 
        // Add constants
        p1->DefineConst( _T("pi"), (value_type)PARSER_CONST_PI);
        p1->DefineConst( _T("e"), (value_type)PARSER_CONST_E);
        p1->DefineConst( _T("const"), 1);
        p1->DefineConst( _T("const1"), 2);
        p1->DefineConst( _T("const2"), 3);
        // string constants
        p1->DefineStrConst( _T("str1"), _T("1.11"));
        p1->DefineStrConst( _T("str2"), _T("2.22"));
        // variables
        value_type vVarVal[] = { 1, 2, 3, -2};
        p1->DefineVar( _T("a"), &vVarVal[0]);
        p1->DefineVar( _T("aa"), &vVarVal[1]);
        p1->DefineVar( _T("b"), &vVarVal[1]);
        p1->DefineVar( _T("c"), &vVarVal[2]);
        p1->DefineVar( _T("d"), &vVarVal[3]);
        
        // custom value ident functions
        p1->AddValIdent(&ParserTester::IsHexVal);        

        // functions
        p1->DefineFun( _T("ping"), Ping);
        p1->DefineFun( _T("f1of1"), f1of1);  // one parameter
        p1->DefineFun( _T("f1of2"), f1of2);  // two parameter
        p1->DefineFun( _T("f2of2"), f2of2);
        p1->DefineFun( _T("f1of3"), f1of3);  // three parameter
        p1->DefineFun( _T("f2of3"), f2of3);
        p1->DefineFun( _T("f3of3"), f3of3);
        p1->DefineFun( _T("f1of4"), f1of4);  // four parameter
        p1->DefineFun( _T("f2of4"), f2of4);
        p1->DefineFun( _T("f3of4"), f3of4);
        p1->DefineFun( _T("f4of4"), f4of4);
        p1->DefineFun( _T("f1of5"), f1of5);  // five parameter
        p1->DefineFun( _T("f2of5"), f2of5);
        p1->DefineFun( _T("f3of5"), f3of5);
        p1->DefineFun( _T("f4of5"), f4of5);
        p1->DefineFun( _T("f5of5"), f5of5);

        // binary operators
        p1->DefineOprt( _T("add"), add, 0);
        p1->DefineOprt( _T("++"), add, 0);
        p1->DefineOprt( _T("&"), land, prLAND);

        // sample functions
        p1->DefineFun( _T("min"), Min);
        p1->DefineFun( _T("max"), Max);
        p1->DefineFun( _T("sum"), Sum);
        p1->DefineFun( _T("valueof"), ValueOf);
        p1->DefineFun( _T("atof"), StrToFloat);
        p1->DefineFun( _T("strfun1"), StrFun1);
        p1->DefineFun( _T("strfun2"), StrFun2);
        p1->DefineFun( _T("strfun3"), StrFun3);
        p1->DefineFun( _T("lastArg"), LastArg);
        p1->DefineFun( _T("firstArg"), FirstArg);
        p1->DefineFun( _T("order"), FirstArg);

        // infix / postfix operator
        // Note: Identifiers used here do not have any meaning 
        //       they are mere placeholders to test certain features.
        p1->DefineInfixOprt( _T("$"), sign, prPOW+1);  // sign with high priority
        p1->DefineInfixOprt( _T("~"), plus2);          // high priority
        p1->DefineInfixOprt( _T("~~"), plus2);
        p1->DefinePostfixOprt( _T("{m}"), Milli);
        p1->DefinePostfixOprt( _T("{M}"), Mega);
        p1->DefinePostfixOprt( _T("m"), Milli);
        p1->DefinePostfixOprt( _T("meg"), Mega);
        p1->DefinePostfixOprt( _T("#"), times3);
        p1->DefinePostfixOprt( _T("'"), sqr); 
        p1->SetExpr(a_str);

        // Test bytecode integrity
        // String parsing and bytecode parsing must yield the same result
        fVal[0] = p1->Eval(); // result from stringparsing
        fVal[1] = p1->Eval(); // result from bytecode
        if (fVal[0]!=fVal[1])
          throw Parser::exception_type( _T("Bytecode / string parsing mismatch.") );

        // Test copy and assignment operators
        try
        {
          // Test copy constructor
          std::vector<mu::Parser> vParser;
          vParser.push_back(*(p1.get()));
          mu::Parser p4 = vParser[0];   // take parser from vector
        
          // destroy the originals from p2
          vParser.clear();              // delete the vector
          p1.reset(0);

          fVal[2] = p4.Eval();

          // Test assignment operator
          // additionally  disable Optimizer this time
          mu::Parser p5;
          p5 = p4;
          p5.EnableOptimizer(false);
          fVal[3] = p5.Eval();

          // Test Eval function for multiple return values
          // use p2 since it has the optimizer enabled!
          int nNum;
          value_type *v = p4.Eval(nNum);
          fVal[4] = v[nNum-1];
        }
        catch(std::exception &e)
        {
          mu::console() << _T("\n  ") << e.what() << _T("\n");
        }

        // limited floating point accuracy requires the following test
        bool bCloseEnough(true);
        for (unsigned i=0; i<sizeof(fVal)/sizeof(value_type); ++i)
        {
          bCloseEnough &= (fabs(a_fRes-fVal[i]) <= fabs(fVal[i]*0.00001));

          // The tests equations never result in infinity, if they do thats a bug.
          // reference:
          // http://sourceforge.net/projects/muparser/forums/forum/462843/topic/5037825
          #ifdef _MSC_VER
          #pragma warning(push)
          #pragma warning(disable:4127)
          #endif
		  if (std::numeric_limits<value_type>::has_infinity)
          #ifdef _MSC_VER
          #pragma warning(pop)
          #endif
		  {
            bCloseEnough &= (fabs(fVal[i]) != numeric_limits<value_type>::infinity());
		  }
		}

        iRet = ((bCloseEnough && a_fPass) || (!bCloseEnough && !a_fPass)) ? 0 : 1;
        
        
        if (iRet==1)
        {
          mu::console() << _T("\n  fail: ") << a_str.c_str() 
                        << _T(" (incorrect result; expected: ") << a_fRes
                        << _T(" ;calculated: ") << fVal[0] << _T(",") 
                                                << fVal[1] << _T(",")
                                                << fVal[2] << _T(",")
                                                << fVal[3] << _T(",")
                                                << fVal[4] << _T(").");
        }
      }
      catch(Parser::exception_type &e)
      {
        if (a_fPass)
        {
          if (fVal[0]!=fVal[2] && fVal[0]!=-999 && fVal[1]!=-998)
            mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (copy construction)");
          else
            mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.GetMsg() << _T(")");
          return 1;
        }
      }
      catch(std::exception &e)
      {
        mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.what() << _T(")");
        return 1;  // always return a failure since this exception is not expected
      }
      catch(...)
      {
        mu::console() << _T("\n  fail: ") << a_str.c_str() <<  _T(" (unexpected exception)");
        return 1;  // exceptions other than ParserException are not allowed
      }

      return iRet;
    }

    //---------------------------------------------------------------------------
    int ParserTester::EqnTestInt(const string_type &a_str, double a_fRes, bool a_fPass)
    {
      ParserTester::c_iCount++;

      value_type vVarVal[] = {1, 2, 3};   // variable values
      int iRet(0);

      try
      {
        value_type fVal[2] = {-99, -999};   // results: initially should be different
        ParserInt p;
        p.DefineConst( _T("const1"), 1);
        p.DefineConst( _T("const2"), 2);
        p.DefineVar( _T("a"), &vVarVal[0]);
        p.DefineVar( _T("b"), &vVarVal[1]);
        p.DefineVar( _T("c"), &vVarVal[2]);

        p.SetExpr(a_str);
        fVal[0] = p.Eval(); // result from stringparsing
        fVal[1] = p.Eval(); // result from bytecode

        if (fVal[0]!=fVal[1])
          throw Parser::exception_type( _T("Bytecode corrupt.") );

        iRet =  ( (a_fRes==fVal[0] &&  a_fPass) || 
                  (a_fRes!=fVal[0] && !a_fPass) ) ? 0 : 1;
        if (iRet==1)
        {
          mu::console() << _T("\n  fail: ") << a_str.c_str() 
                        << _T(" (incorrect result; expected: ") << a_fRes 
                        << _T(" ;calculated: ") << fVal[0]<< _T(").");
        }
      }
      catch(Parser::exception_type &e)
      {
        if (a_fPass)
        {
          mu::console() << _T("\n  fail: ") << e.GetExpr() << _T(" : ") << e.GetMsg();
          iRet = 1;
        }
      }
      catch(...)
      {
        mu::console() << _T("\n  fail: ") << a_str.c_str() <<  _T(" (unexpected exception)");
        iRet = 1;  // exceptions other than ParserException are not allowed
      }

      return iRet;
    }

    //---------------------------------------------------------------------------
    /** \brief Test an expression in Bulk Mode. */
    int ParserTester::EqnTestBulk(const string_type &a_str, double a_fRes[4], bool a_fPass)
    {
        ParserTester::c_iCount++;

        // Define Bulk Variables
        int nBulkSize = 4;
        value_type vVariableA[] = { 1, 2, 3, 4 };   // variable values
        value_type vVariableB[] = { 2, 2, 2, 2 };   // variable values
        value_type vVariableC[] = { 3, 3, 3, 3 };   // variable values
        value_type vResults[] = { 0, 0, 0, 0 };   // variable values
        int iRet(0);

        try
        {
            Parser p;
            p.DefineConst(_T("const1"), 1);
            p.DefineConst(_T("const2"), 2);
            p.DefineVar(_T("a"), vVariableA);
            p.DefineVar(_T("b"), vVariableB);
            p.DefineVar(_T("c"), vVariableC);

            p.SetExpr(a_str);
            p.Eval(vResults, nBulkSize);

            bool bCloseEnough(true);
            for (int i = 0; i < nBulkSize; ++i)
            {
                bCloseEnough &= (fabs(a_fRes[i] - vResults[i]) <= fabs(a_fRes[i] * 0.00001));
            }

            iRet = ((bCloseEnough && a_fPass) || (!bCloseEnough && !a_fPass)) ? 0 : 1;
            if (iRet == 1)
            {
                mu::console() << _T("\n  fail: ") << a_str.c_str()
                    << _T(" (incorrect result; expected: {") << a_fRes[0] << _T(",") << a_fRes[1] << _T(",") << a_fRes[2] << _T(",") << a_fRes[3] << _T("}")
                    << _T(" ;calculated: ") << vResults[0] << _T(",") << vResults[1] << _T(",") << vResults[2] << _T(",") << vResults[3] << _T("}");
            }
        }
        catch (Parser::exception_type &e)
        {
            if (a_fPass)
            {
                mu::console() << _T("\n  fail: ") << e.GetExpr() << _T(" : ") << e.GetMsg();
                iRet = 1;
            }
        }
        catch (...)
        {
            mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (unexpected exception)");
            iRet = 1;  // exceptions other than ParserException are not allowed
        }

        return iRet;
    }

    //---------------------------------------------------------------------------
    /** \brief Internal error in test class Test is going to be aborted. */
    void ParserTester::Abort() const
    {
      mu::console() << _T("Test failed (internal error in test class)") << endl;
      while (!getchar());
      exit(-1);
    }
  } // namespace test
} // namespace mu
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
#include <cassert>
#include <cstdio>
#include <cstring>
#include <map>
#include <stack>
#include <string>

//amalgamated header include by Piotrs script #include "muParserTokenReader.h"
//amalgamated header include by Piotrs script #include "muParserBase.h"

/** \file
    \brief This file contains the parser token reader implementation.
*/


namespace mu
{

  // Forward declaration
  class ParserBase;

  //---------------------------------------------------------------------------
  /** \brief Copy constructor.

      \sa Assign
      \throw nothrow
  */
  ParserTokenReader::ParserTokenReader(const ParserTokenReader &a_Reader) 
  { 
    Assign(a_Reader);
  }
    
  //---------------------------------------------------------------------------
  /** \brief Assignment operator.

      Self assignment will be suppressed otherwise #Assign is called.

      \param a_Reader Object to copy to this token reader.
      \throw nothrow
  */
  ParserTokenReader& ParserTokenReader::operator=(const ParserTokenReader &a_Reader) 
  {
    if (&a_Reader!=this)
      Assign(a_Reader);

    return *this;
  }

  //---------------------------------------------------------------------------
  /** \brief Assign state of a token reader to this token reader. 
      
      \param a_Reader Object from which the state should be copied.
      \throw nothrow
  */
  void ParserTokenReader::Assign(const ParserTokenReader &a_Reader)
  {
    m_pParser = a_Reader.m_pParser;
    m_strFormula = a_Reader.m_strFormula;
    m_iPos = a_Reader.m_iPos;
    m_iSynFlags = a_Reader.m_iSynFlags;
    
    m_UsedVar         = a_Reader.m_UsedVar;
    m_pFunDef         = a_Reader.m_pFunDef;
    m_pConstDef       = a_Reader.m_pConstDef;
    m_pVarDef         = a_Reader.m_pVarDef;
    m_pStrVarDef      = a_Reader.m_pStrVarDef;
    m_pPostOprtDef    = a_Reader.m_pPostOprtDef;
    m_pInfixOprtDef   = a_Reader.m_pInfixOprtDef;
    m_pOprtDef        = a_Reader.m_pOprtDef;
    m_bIgnoreUndefVar = a_Reader.m_bIgnoreUndefVar;
    m_vIdentFun       = a_Reader.m_vIdentFun;
    m_pFactory        = a_Reader.m_pFactory;
    m_pFactoryData    = a_Reader.m_pFactoryData;
    m_iBrackets       = a_Reader.m_iBrackets;
    m_cArgSep         = a_Reader.m_cArgSep;
	m_fZero           = a_Reader.m_fZero;
	m_lastTok         = a_Reader.m_lastTok;
  }

  //---------------------------------------------------------------------------
  /** \brief Constructor. 
      
      Create a Token reader and bind it to a parser object. 

      \pre [assert] a_pParser may not be NULL
      \post #m_pParser==a_pParser
      \param a_pParent Parent parser object of the token reader.
  */
  ParserTokenReader::ParserTokenReader(ParserBase *a_pParent)
    :m_pParser(a_pParent)
    ,m_strFormula()
    ,m_iPos(0)
    ,m_iSynFlags(0)
    ,m_bIgnoreUndefVar(false)
    ,m_pFunDef(NULL)
    ,m_pPostOprtDef(NULL)
    ,m_pInfixOprtDef(NULL)
    ,m_pOprtDef(NULL)
    ,m_pConstDef(NULL)
    ,m_pStrVarDef(NULL)
    ,m_pVarDef(NULL)
    ,m_pFactory(NULL)
    ,m_pFactoryData(NULL)
    ,m_vIdentFun()
    ,m_UsedVar()
    ,m_fZero(0)
    ,m_iBrackets(0)
    ,m_lastTok()
    ,m_cArgSep(',')
  {
    assert(m_pParser);
    SetParent(m_pParser);
  }
    
  //---------------------------------------------------------------------------
  /** \brief Create instance of a ParserTokenReader identical with this 
              and return its pointer. 

      This is a factory method the calling function must take care of the object destruction.

      \return A new ParserTokenReader object.
      \throw nothrow
  */
  ParserTokenReader* ParserTokenReader::Clone(ParserBase *a_pParent) const
  {
    std::unique_ptr<ParserTokenReader> ptr(new ParserTokenReader(*this));
    ptr->SetParent(a_pParent);
    return ptr.release();
  }

  //---------------------------------------------------------------------------
  ParserTokenReader::token_type& ParserTokenReader::SaveBeforeReturn(const token_type &tok)
  {
    m_lastTok = tok;
    return m_lastTok;
  }

  //---------------------------------------------------------------------------
  void ParserTokenReader::AddValIdent(identfun_type a_pCallback)
  {
    // Use push_front is used to give user defined callbacks a higher priority than
    // the built in ones. Otherwise reading hex numbers would not work
    // since the "0" in "0xff" would always be read first making parsing of 
    // the rest impossible.
    // reference:
    // http://sourceforge.net/projects/muparser/forums/forum/462843/topic/4824956
    m_vIdentFun.push_front(a_pCallback);
  }

  //---------------------------------------------------------------------------
  void ParserTokenReader::SetVarCreator(facfun_type a_pFactory, void *pUserData)
  {
    m_pFactory = a_pFactory;
    m_pFactoryData = pUserData;
  }

  //---------------------------------------------------------------------------
  /** \brief Return the current position of the token reader in the formula string. 

      \return #m_iPos
      \throw nothrow
  */
  int ParserTokenReader::GetPos() const
  {
    return m_iPos;
  }

  //---------------------------------------------------------------------------
  /** \brief Return a reference to the formula. 

      \return #m_strFormula
      \throw nothrow
  */
  const string_type& ParserTokenReader::GetExpr() const
  {
    return m_strFormula;
  }

  //---------------------------------------------------------------------------
  /** \brief Return a map containing the used variables only. */
  varmap_type& ParserTokenReader::GetUsedVar() 
  {
    return m_UsedVar;
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize the token Reader. 
  
      Sets the formula position index to zero and set Syntax flags to default for initial formula parsing.
      \pre [assert] triggered if a_szFormula==0
  */
  void ParserTokenReader::SetFormula(const string_type &a_strFormula)
  {
    m_strFormula = a_strFormula;
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Set Flag that controls behaviour in case of undefined variables being found. 
  
    If true, the parser does not throw an exception if an undefined variable is found. 
    otherwise it does. This variable is used internally only!
    It suppresses a "undefined variable" exception in GetUsedVar().  
    Those function should return a complete list of variables including 
    those the are not defined by the time of it's call.
  */
  void ParserTokenReader::IgnoreUndefVar(bool bIgnore)
  {
    m_bIgnoreUndefVar = bIgnore;
  }

  //---------------------------------------------------------------------------
  /** \brief Reset the token reader to the start of the formula. 

      The syntax flags will be reset to a value appropriate for the 
      start of a formula.
      \post #m_iPos==0, #m_iSynFlags = noOPT | noBC | noPOSTOP | noSTR
      \throw nothrow
      \sa ESynCodes
  */
  void ParserTokenReader::ReInit()
  {
    m_iPos = 0;
    m_iSynFlags = sfSTART_OF_LINE;
    m_iBrackets = 0;
    m_UsedVar.clear();
    m_lastTok = token_type();
  }

  //---------------------------------------------------------------------------
  /** \brief Read the next token from the string. */ 
  ParserTokenReader::token_type ParserTokenReader::ReadNextToken()
  {
    assert(m_pParser);

    const char_type *szFormula = m_strFormula.c_str();
    token_type tok;

    // Ignore all non printable characters when reading the expression
    while (szFormula[m_iPos]>0 && szFormula[m_iPos]<=0x20) 
      ++m_iPos;

    if ( IsEOF(tok) )        return SaveBeforeReturn(tok); // Check for end of formula
    if ( IsOprt(tok) )       return SaveBeforeReturn(tok); // Check for user defined binary operator
    if ( IsFunTok(tok) )     return SaveBeforeReturn(tok); // Check for function token
    if ( IsBuiltIn(tok) )    return SaveBeforeReturn(tok); // Check built in operators / tokens
    if ( IsArgSep(tok) )     return SaveBeforeReturn(tok); // Check for function argument separators
    if ( IsValTok(tok) )     return SaveBeforeReturn(tok); // Check for values / constant tokens
    if ( IsVarTok(tok) )     return SaveBeforeReturn(tok); // Check for variable tokens
    if ( IsStrVarTok(tok) )  return SaveBeforeReturn(tok); // Check for string variables
    if ( IsString(tok) )     return SaveBeforeReturn(tok); // Check for String tokens
    if ( IsInfixOpTok(tok) ) return SaveBeforeReturn(tok); // Check for unary operators
    if ( IsPostOpTok(tok) )  return SaveBeforeReturn(tok); // Check for unary operators

    // Check String for undefined variable token. Done only if a 
    // flag is set indicating to ignore undefined variables.
    // This is a way to conditionally avoid an error if 
    // undefined variables occur. 
    // (The GetUsedVar function must suppress the error for
    // undefined variables in order to collect all variable 
    // names including the undefined ones.)
    if ( (m_bIgnoreUndefVar || m_pFactory) && IsUndefVarTok(tok) )  
      return SaveBeforeReturn(tok);

    // Check for unknown token
    // 
    // !!! From this point on there is no exit without an exception possible...
    // 
    string_type strTok;
    int iEnd = ExtractToken(m_pParser->ValidNameChars(), strTok, m_iPos);
    if (iEnd!=m_iPos)
      Error(ecUNASSIGNABLE_TOKEN, m_iPos, strTok);

    Error(ecUNASSIGNABLE_TOKEN, m_iPos, m_strFormula.substr(m_iPos));
    return token_type(); // never reached
  }

  //---------------------------------------------------------------------------
  void ParserTokenReader::SetParent(ParserBase *a_pParent)
  {
    m_pParser       = a_pParent; 
    m_pFunDef       = &a_pParent->m_FunDef;
    m_pOprtDef      = &a_pParent->m_OprtDef;
    m_pInfixOprtDef = &a_pParent->m_InfixOprtDef;
    m_pPostOprtDef  = &a_pParent->m_PostOprtDef;
    m_pVarDef       = &a_pParent->m_VarDef;
    m_pStrVarDef    = &a_pParent->m_StrVarDef;
    m_pConstDef     = &a_pParent->m_ConstDef;
  }

  //---------------------------------------------------------------------------
  /** \brief Extract all characters that belong to a certain charset.

    \param a_szCharSet [in] Const char array of the characters allowed in the token. 
    \param a_strTok [out]  The string that consists entirely of characters listed in a_szCharSet.
    \param a_iPos [in] Position in the string from where to start reading.
    \return The Position of the first character not listed in a_szCharSet.
    \throw nothrow
  */
  int ParserTokenReader::ExtractToken(const char_type *a_szCharSet, 
                                      string_type &a_sTok, 
                                      int a_iPos) const
  {
    int iEnd = (int)m_strFormula.find_first_not_of(a_szCharSet, a_iPos);

    if (iEnd==(int)string_type::npos)
        iEnd = (int)m_strFormula.length();
    
    // Assign token string if there was something found
    if (a_iPos!=iEnd)
      a_sTok = string_type( m_strFormula.begin()+a_iPos, m_strFormula.begin()+iEnd);

    return iEnd;
  }

  //---------------------------------------------------------------------------
  /** \brief Check Expression for the presence of a binary operator token.
  
    Userdefined binary operator "++" gives inconsistent parsing result for
    the equations "a++b" and "a ++ b" if alphabetic characters are allowed
    in operator tokens. To avoid this this function checks specifically
    for operator tokens.
  */
  int ParserTokenReader::ExtractOperatorToken(string_type &a_sTok, 
                                              int a_iPos) const
  {
    // Changed as per Issue 6: https://code.google.com/p/muparser/issues/detail?id=6
    int iEnd = (int)m_strFormula.find_first_not_of(m_pParser->ValidOprtChars(), a_iPos);
    if (iEnd==(int)string_type::npos)
      iEnd = (int)m_strFormula.length();

    // Assign token string if there was something found
    if (a_iPos!=iEnd)
    {
      a_sTok = string_type( m_strFormula.begin() + a_iPos, m_strFormula.begin() + iEnd);
      return iEnd;
    }
    else
    {
      // There is still the chance of having to deal with an operator consisting exclusively
      // of alphabetic characters.
      return ExtractToken(MUP_CHARS, a_sTok, a_iPos);
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Check if a built in operator or other token can be found
      \param a_Tok  [out] Operator token if one is found. This can either be a binary operator or an infix operator token.
      \return true if an operator token has been found.
  */
  bool ParserTokenReader::IsBuiltIn(token_type &a_Tok)
  {
    const char_type **const pOprtDef = m_pParser->GetOprtDef(),
                     *const szFormula = m_strFormula.c_str();

    // Compare token with function and operator strings
    // check string for operator/function
    for (int i=0; pOprtDef[i]; i++)
    {
      std::size_t len( std::char_traits<char_type>::length(pOprtDef[i]) );
      if ( string_type(pOprtDef[i]) == string_type(szFormula + m_iPos, szFormula + m_iPos + len) )
      {
        switch(i)
        {
        //case cmAND:
        //case cmOR:
        //case cmXOR:
        case cmLAND:
        case cmLOR:
        case cmLT:
        case cmGT:
        case cmLE:
        case cmGE:
        case cmNEQ:  
        case cmEQ:
        case cmADD:
        case cmSUB:
        case cmMUL:
        case cmDIV:
        case cmPOW:
        case cmASSIGN:
              //if (len!=sTok.length())
              //  continue;

              // The assignment operator need special treatment
              if (i==cmASSIGN && m_iSynFlags & noASSIGN)
                Error(ecUNEXPECTED_OPERATOR, m_iPos, pOprtDef[i]);

              if (!m_pParser->HasBuiltInOprt()) continue;
              if (m_iSynFlags & noOPT) 
              {
                // Maybe its an infix operator not an operator
                // Both operator types can share characters in 
                // their identifiers
                if ( IsInfixOpTok(a_Tok) ) 
                  return true;

                Error(ecUNEXPECTED_OPERATOR, m_iPos, pOprtDef[i]);
              }

              m_iSynFlags  = noBC | noOPT | noARG_SEP | noPOSTOP | noASSIGN | noIF | noELSE | noEND;
              break;

		    case cmBO:
              if (m_iSynFlags & noBO)
	              Error(ecUNEXPECTED_PARENS, m_iPos, pOprtDef[i]);
              
              if (m_lastTok.GetCode()==cmFUNC)
                m_iSynFlags = noOPT | noEND | noARG_SEP | noPOSTOP | noASSIGN | noIF | noELSE;
              else
                m_iSynFlags = noBC | noOPT | noEND | noARG_SEP | noPOSTOP | noASSIGN| noIF | noELSE;

              ++m_iBrackets;
              break;

		    case cmBC:
              if (m_iSynFlags & noBC)
                Error(ecUNEXPECTED_PARENS, m_iPos, pOprtDef[i]);

              m_iSynFlags  = noBO | noVAR | noVAL | noFUN | noINFIXOP | noSTR | noASSIGN;

              if (--m_iBrackets<0)
                Error(ecUNEXPECTED_PARENS, m_iPos, pOprtDef[i]);
              break;

        case cmELSE:
              if (m_iSynFlags & noELSE)
                Error(ecUNEXPECTED_CONDITIONAL, m_iPos, pOprtDef[i]);

              m_iSynFlags = noBC | noPOSTOP | noEND | noOPT | noIF | noELSE;
              break;

        case cmIF:
              if (m_iSynFlags & noIF)
                Error(ecUNEXPECTED_CONDITIONAL, m_iPos, pOprtDef[i]);

              m_iSynFlags = noBC | noPOSTOP | noEND | noOPT | noIF | noELSE;
              break;

		    default:      // The operator is listed in c_DefaultOprt, but not here. This is a bad thing...
              Error(ecINTERNAL_ERROR);
        } // switch operator id

        m_iPos += (int)len;
        a_Tok.Set( (ECmdCode)i, pOprtDef[i] );
        return true;
	    } // if operator string found
    } // end of for all operator strings
  
    return false;
  }

  //---------------------------------------------------------------------------
  bool ParserTokenReader::IsArgSep(token_type &a_Tok)
  {
    const char_type* szFormula = m_strFormula.c_str();

    if (szFormula[m_iPos]==m_cArgSep)
    {
      // copy the separator into null terminated string
      char_type szSep[2];
      szSep[0] = m_cArgSep;
      szSep[1] = 0;

      if (m_iSynFlags & noARG_SEP)
        Error(ecUNEXPECTED_ARG_SEP, m_iPos, szSep);

      m_iSynFlags  = noBC | noOPT | noEND | noARG_SEP | noPOSTOP | noASSIGN;
      m_iPos++;
      a_Tok.Set(cmARG_SEP, szSep);
      return true;
    }

    return false;
  }

  //---------------------------------------------------------------------------
  /** \brief Check for End of Formula.

      \return true if an end of formula is found false otherwise.
      \param a_Tok [out] If an eof is found the corresponding token will be stored there.
      \throw nothrow
      \sa IsOprt, IsFunTok, IsStrFunTok, IsValTok, IsVarTok, IsString, IsInfixOpTok, IsPostOpTok
  */
  bool ParserTokenReader::IsEOF(token_type &a_Tok)
  {
    const char_type* szFormula = m_strFormula.c_str();

    // check for EOF
    if ( !szFormula[m_iPos] /*|| szFormula[m_iPos] == '\n'*/)
    {
      if ( m_iSynFlags & noEND )
        Error(ecUNEXPECTED_EOF, m_iPos);

      if (m_iBrackets>0)
        Error(ecMISSING_PARENS, m_iPos, _T(")"));

      m_iSynFlags = 0;
      a_Tok.Set(cmEND);
      return true;
    }

    return false;
  }

  //---------------------------------------------------------------------------
  /** \brief Check if a string position contains a unary infix operator. 
      \return true if a function token has been found false otherwise.
  */
  bool ParserTokenReader::IsInfixOpTok(token_type &a_Tok)
  {
    string_type sTok;
    int iEnd = ExtractToken(m_pParser->ValidInfixOprtChars(), sTok, m_iPos);
    if (iEnd==m_iPos)
      return false;

    // iterate over all postfix operator strings
    funmap_type::const_reverse_iterator it = m_pInfixOprtDef->rbegin();
    for ( ; it!=m_pInfixOprtDef->rend(); ++it)
    {
      if (sTok.find(it->first)!=0)
        continue;

      a_Tok.Set(it->second, it->first);
      m_iPos += (int)it->first.length();

      if (m_iSynFlags & noINFIXOP) 
        Error(ecUNEXPECTED_OPERATOR, m_iPos, a_Tok.GetAsString());

      m_iSynFlags = noPOSTOP | noINFIXOP | noOPT | noBC | noSTR | noASSIGN;
      return true;
    }

    return false;

/*
    a_Tok.Set(item->second, sTok);
    m_iPos = (int)iEnd;

    if (m_iSynFlags & noINFIXOP) 
      Error(ecUNEXPECTED_OPERATOR, m_iPos, a_Tok.GetAsString());

    m_iSynFlags = noPOSTOP | noINFIXOP | noOPT | noBC | noSTR | noASSIGN; 
    return true;
*/
  }

  //---------------------------------------------------------------------------
  /** \brief Check whether the token at a given position is a function token.
      \param a_Tok [out] If a value token is found it will be placed here.
      \throw ParserException if Syntaxflags do not allow a function at a_iPos
      \return true if a function token has been found false otherwise.
      \pre [assert] m_pParser!=0
  */
  bool ParserTokenReader::IsFunTok(token_type &a_Tok)
  {
    string_type strTok;
    int iEnd = ExtractToken(m_pParser->ValidNameChars(), strTok, m_iPos);
    if (iEnd==m_iPos)
      return false;

    funmap_type::const_iterator item = m_pFunDef->find(strTok);
    if (item==m_pFunDef->end())
      return false;

    // Check if the next sign is an opening bracket
    const char_type *szFormula = m_strFormula.c_str();
    if (szFormula[iEnd]!='(')
      return false;

    a_Tok.Set(item->second, strTok);

    m_iPos = (int)iEnd;
    if (m_iSynFlags & noFUN)
      Error(ecUNEXPECTED_FUN, m_iPos-(int)a_Tok.GetAsString().length(), a_Tok.GetAsString());

    m_iSynFlags = noANY ^ noBO;
    return true;
  }

  //---------------------------------------------------------------------------
  /** \brief Check if a string position contains a binary operator.
      \param a_Tok  [out] Operator token if one is found. This can either be a binary operator or an infix operator token.
      \return true if an operator token has been found.
  */
  bool ParserTokenReader::IsOprt(token_type &a_Tok)
  {
    const char_type *const szExpr = m_strFormula.c_str();
    string_type strTok;

    int iEnd = ExtractOperatorToken(strTok, m_iPos);
    if (iEnd==m_iPos)
      return false;

    // Check if the operator is a built in operator, if so ignore it here
    const char_type **const pOprtDef = m_pParser->GetOprtDef();
    for (int i=0; m_pParser->HasBuiltInOprt() && pOprtDef[i]; ++i)
    {
      if (string_type(pOprtDef[i])==strTok)
        return false;
    }

    // Note:
    // All tokens in oprt_bin_maptype are have been sorted by their length
    // Long operators must come first! Otherwise short names (like: "add") that
    // are part of long token names (like: "add123") will be found instead 
    // of the long ones.
    // Length sorting is done with ascending length so we use a reverse iterator here.
    funmap_type::const_reverse_iterator it = m_pOprtDef->rbegin();
    for ( ; it!=m_pOprtDef->rend(); ++it)
    {
      const string_type &sID = it->first;
      if ( sID == string_type(szExpr + m_iPos, szExpr + m_iPos + sID.length()) )
      {
        a_Tok.Set(it->second, strTok);

        // operator was found
        if (m_iSynFlags & noOPT) 
        {
          // An operator was found but is not expected to occur at
          // this position of the formula, maybe it is an infix 
          // operator, not a binary operator. Both operator types
          // can share characters in their identifiers.
          if ( IsInfixOpTok(a_Tok) ) 
            return true;
          else
          {
            // nope, no infix operator
            return false;
            //Error(ecUNEXPECTED_OPERATOR, m_iPos, a_Tok.GetAsString()); 
          }

        }

        m_iPos += (int)sID.length();
        m_iSynFlags  = noBC | noOPT | noARG_SEP | noPOSTOP | noEND | noASSIGN;
        return true;
      }
    }

    return false;
  }

  //---------------------------------------------------------------------------
  /** \brief Check if a string position contains a unary post value operator. */
  bool ParserTokenReader::IsPostOpTok(token_type &a_Tok)
  {
    // <ibg 20110629> Do not check for postfix operators if they are not allowed at
    //                the current expression index.
    //
    //  This will fix the bug reported here:  
    //
    //  http://sourceforge.net/tracker/index.php?func=detail&aid=3343891&group_id=137191&atid=737979
    //
    if (m_iSynFlags & noPOSTOP)
      return false;
    // </ibg>

    // Tricky problem with equations like "3m+5":
    //     m is a postfix operator, + is a valid sign for postfix operators and 
    //     for binary operators parser detects "m+" as operator string and 
    //     finds no matching postfix operator.
    // 
    // This is a special case so this routine slightly differs from the other
    // token readers.
    
    // Test if there could be a postfix operator
    string_type sTok;
    int iEnd = ExtractToken(m_pParser->ValidOprtChars(), sTok, m_iPos);
    if (iEnd==m_iPos)
      return false;

    // iterate over all postfix operator strings
    funmap_type::const_reverse_iterator it = m_pPostOprtDef->rbegin();
    for ( ; it!=m_pPostOprtDef->rend(); ++it)
    {
      if (sTok.find(it->first)!=0)
        continue;

      a_Tok.Set(it->second, sTok);
  	  m_iPos += (int)it->first.length();

      m_iSynFlags = noVAL | noVAR | noFUN | noBO | noPOSTOP | noSTR | noASSIGN;
      return true;
    }

    return false;
  }

  //---------------------------------------------------------------------------
  /** \brief Check whether the token at a given position is a value token.

    Value tokens are either values or constants.

    \param a_Tok [out] If a value token is found it will be placed here.
    \return true if a value token has been found.
  */
  bool ParserTokenReader::IsValTok(token_type &a_Tok)
  {
    assert(m_pConstDef);
    assert(m_pParser);

    string_type strTok;
    value_type fVal(0);
    int iEnd(0);
    
    // 2.) Check for user defined constant
    // Read everything that could be a constant name
    iEnd = ExtractToken(m_pParser->ValidNameChars(), strTok, m_iPos);
    if (iEnd!=m_iPos)
    {
      valmap_type::const_iterator item = m_pConstDef->find(strTok);
      if (item!=m_pConstDef->end())
      {
        m_iPos = iEnd;
        a_Tok.SetVal(item->second, strTok);

        if (m_iSynFlags & noVAL)
          Error(ecUNEXPECTED_VAL, m_iPos - (int)strTok.length(), strTok);

        m_iSynFlags = noVAL | noVAR | noFUN | noBO | noINFIXOP | noSTR | noASSIGN; 
        return true;
      }
    }

    // 3.call the value recognition functions provided by the user
    // Call user defined value recognition functions
    std::list<identfun_type>::const_iterator item = m_vIdentFun.begin();
    for (item = m_vIdentFun.begin(); item!=m_vIdentFun.end(); ++item)
    {
      int iStart = m_iPos;
      if ( (*item)(m_strFormula.c_str() + m_iPos, &m_iPos, &fVal)==1 )
      {
        // 2013-11-27 Issue 2:  https://code.google.com/p/muparser/issues/detail?id=2
        strTok.assign(m_strFormula.c_str(), iStart, m_iPos-iStart);

        if (m_iSynFlags & noVAL)
          Error(ecUNEXPECTED_VAL, m_iPos - (int)strTok.length(), strTok);

        a_Tok.SetVal(fVal, strTok);
        m_iSynFlags = noVAL | noVAR | noFUN | noBO | noINFIXOP | noSTR | noASSIGN;
        return true;
      }
    }

    return false;
  }

  //---------------------------------------------------------------------------
  /** \brief Check wheter a token at a given position is a variable token. 
      \param a_Tok [out] If a variable token has been found it will be placed here.
	    \return true if a variable token has been found.
  */
  bool ParserTokenReader::IsVarTok(token_type &a_Tok)
  {
    if (m_pVarDef->empty())
      return false;

    string_type strTok;
    int iEnd = ExtractToken(m_pParser->ValidNameChars(), strTok, m_iPos);
    if (iEnd==m_iPos)
      return false;

    varmap_type::const_iterator item =  m_pVarDef->find(strTok);
    if (item==m_pVarDef->end())
      return false;

    if (m_iSynFlags & noVAR)
      Error(ecUNEXPECTED_VAR, m_iPos, strTok);

    m_pParser->OnDetectVar(&m_strFormula, m_iPos, iEnd);

    m_iPos = iEnd;
    a_Tok.SetVar(item->second, strTok);
    m_UsedVar[item->first] = item->second;  // Add variable to used-var-list

    m_iSynFlags = noVAL | noVAR | noFUN | noBO | noINFIXOP | noSTR;

//  Zur Info hier die SynFlags von IsVal():
//    m_iSynFlags = noVAL | noVAR | noFUN | noBO | noINFIXOP | noSTR | noASSIGN; 
    return true;
  }

  //---------------------------------------------------------------------------
  bool ParserTokenReader::IsStrVarTok(token_type &a_Tok)
  {
    if (!m_pStrVarDef || m_pStrVarDef->empty())
      return false;

    string_type strTok;
    int iEnd = ExtractToken(m_pParser->ValidNameChars(), strTok, m_iPos);
    if (iEnd==m_iPos)
      return false;

    strmap_type::const_iterator item =  m_pStrVarDef->find(strTok);
    if (item==m_pStrVarDef->end())
      return false;

    if (m_iSynFlags & noSTR)
      Error(ecUNEXPECTED_VAR, m_iPos, strTok);

    m_iPos = iEnd;
    if (!m_pParser->m_vStringVarBuf.size())
      Error(ecINTERNAL_ERROR);

    a_Tok.SetString(m_pParser->m_vStringVarBuf[item->second], m_pParser->m_vStringVarBuf.size() );

    m_iSynFlags = noANY ^ ( noBC | noOPT | noEND | noARG_SEP);
    return true;
  }


  //---------------------------------------------------------------------------
  /** \brief Check wheter a token at a given position is an undefined variable. 

      \param a_Tok [out] If a variable tom_pParser->m_vStringBufken has been found it will be placed here.
	    \return true if a variable token has been found.
      \throw nothrow
  */
  bool ParserTokenReader::IsUndefVarTok(token_type &a_Tok)
  {
    string_type strTok;
    int iEnd( ExtractToken(m_pParser->ValidNameChars(), strTok, m_iPos) );
    if ( iEnd==m_iPos )
      return false;

    if (m_iSynFlags & noVAR)
    {
      // <ibg/> 20061021 added token string strTok instead of a_Tok.GetAsString() as the 
      //                 token identifier. 
      // related bug report:
      // http://sourceforge.net/tracker/index.php?func=detail&aid=1578779&group_id=137191&atid=737979
      Error(ecUNEXPECTED_VAR, m_iPos - (int)a_Tok.GetAsString().length(), strTok);
    }

    // If a factory is available implicitely create new variables
    if (m_pFactory)
    {
      value_type *fVar = m_pFactory(strTok.c_str(), m_pFactoryData);
      a_Tok.SetVar(fVar, strTok );

      // Do not use m_pParser->DefineVar( strTok, fVar );
      // in order to define the new variable, it will clear the
      // m_UsedVar array which will kill previously defined variables
      // from the list
      // This is safe because the new variable can never override an existing one
      // because they are checked first!
      (*m_pVarDef)[strTok] = fVar;
      m_UsedVar[strTok] = fVar;  // Add variable to used-var-list
    }
    else
    {
      a_Tok.SetVar((value_type*)&m_fZero, strTok);
      m_UsedVar[strTok] = 0;  // Add variable to used-var-list
    }

    m_iPos = iEnd;

    // Call the variable factory in order to let it define a new parser variable
    m_iSynFlags = noVAL | noVAR | noFUN | noBO | noPOSTOP | noINFIXOP | noSTR;
    return true;
  }


  //---------------------------------------------------------------------------
  /** \brief Check wheter a token at a given position is a string.
      \param a_Tok [out] If a variable token has been found it will be placed here.
  	  \return true if a string token has been found.
      \sa IsOprt, IsFunTok, IsStrFunTok, IsValTok, IsVarTok, IsEOF, IsInfixOpTok, IsPostOpTok
      \throw nothrow
  */
  bool ParserTokenReader::IsString(token_type &a_Tok)
  {
    if (m_strFormula[m_iPos]!='"') 
      return false;

    string_type strBuf(&m_strFormula[m_iPos+1]);
    std::size_t iEnd(0), iSkip(0);

    // parser over escaped '\"' end replace them with '"'
    for(iEnd=(int)strBuf.find( _T('\"') ); iEnd!=0 && iEnd!=string_type::npos; iEnd=(int)strBuf.find( _T('\"'), iEnd))
    {
      if (strBuf[iEnd-1]!='\\') break;
      strBuf.replace(iEnd-1, 2, _T("\"") );
      iSkip++;
    }

    if (iEnd==string_type::npos)
      Error(ecUNTERMINATED_STRING, m_iPos, _T("\"") );

    string_type strTok(strBuf.begin(), strBuf.begin()+iEnd);

    if (m_iSynFlags & noSTR)
      Error(ecUNEXPECTED_STR, m_iPos, strTok);

		m_pParser->m_vStringBuf.push_back(strTok); // Store string in internal buffer
    a_Tok.SetString(strTok, m_pParser->m_vStringBuf.size());

    m_iPos += (int)strTok.length() + 2 + (int)iSkip;  // +2 for quotes; +iSkip for escape characters 
    m_iSynFlags = noANY ^ ( noARG_SEP | noBC | noOPT | noEND );

    return true;
  }

  //---------------------------------------------------------------------------
  /** \brief Create an error containing the parse error position.

    This function will create an Parser Exception object containing the error text and its position.

    \param a_iErrc [in] The error code of type #EErrorCodes.
    \param a_iPos [in] The position where the error was detected.
    \param a_strTok [in] The token string representation associated with the error.
    \throw ParserException always throws thats the only purpose of this function.
  */
  void  ParserTokenReader::Error( EErrorCodes a_iErrc, 
                                  int a_iPos, 
                                  const string_type &a_sTok) const
  {
    m_pParser->Error(a_iErrc, a_iPos, a_sTok);
  }

  //---------------------------------------------------------------------------
  void ParserTokenReader::SetArgSep(char_type cArgSep)
  {
    m_cArgSep = cArgSep;
  }

  //---------------------------------------------------------------------------
  char_type ParserTokenReader::GetArgSep() const
  {
    return m_cArgSep;
  }
} // namespace mu

