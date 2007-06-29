/*! @file SimpleOpt.h

    @version 2.8

    @brief A cross-platform command line library which can parse almost any
    of the standard command line formats in use today. It is designed explicitly
    to be portable to any platform and has been tested on Windows and Linux.
    See CSimpleOptTempl for the class definition.

    @section features FEATURES

    -   MIT Licence allows free use in all software (including GPL and commercial)
    -   multi-platform (Windows 95/98/ME/NT/2K/XP, Linux, Unix)
    -   supports all lengths of option names:
        <table width="60%">
            <tr><td width="30%"> -           <td>switch character only (e.g. use stdin for input)
            <tr><td> -o          <td>short (single character)
            <tr><td> -long       <td>long (multiple character, single switch character)
            <tr><td> --longer    <td>long (multiple character, multiple switch characters)
        </table>
    -   supports all types of arguments for options:
        <table width="60%">
            <tr><td width="30%"> --option        <td>short/long option flag (no argument)
            <tr><td> --option ARG    <td>short/long option with separate required argument
            <tr><td> --option=ARG    <td>short/long option with combined required argument
            <tr><td> --option[=ARG]  <td>short/long option with combined optional argument
            <tr><td> -oARG           <td>short option with combined required argument
            <tr><td> -o[ARG]         <td>short option with combined optional argument
        </table>
    -   supports options with multiple or variable numbers of arguments:
        <table width="60%">
            <tr><td width="30%"> --multi ARG1 ARG2      <td>Multiple arguments
            <tr><td> --multi N ARG-1 ARG-2 ... ARG-N    <td>Variable number of arguments
        </table>
    -   supports options which do not use a switch character. i.e. a special word
        which is construed as an option. e.g. "foo.exe open /directory/file.txt"
        supports clumping of multiple short options (no arguments) in a string, e.g.
        "foo.exe -abcdef file1" <==> "foo.exe -a -b -c -d -e -f file1"
    -   automatic recognition of a single slash as equivalent to a single hyphen
        on Windows, e.g. "/f FILE" is equivalent to "-f FILE".
    -   file arguments can appear anywhere in the argument list:
        "foo.exe file1.txt -a ARG file2.txt --flag file3.txt file4.txt"
        files will be returned to the application in the same order they were supplied
        on the command line
    -   short-circuit option matching: "--man" will match "--mandate"
        invalid options can be handled while continuing to parse the command line
        valid options list can be changed dynamically during command line processing,
        i.e. accept different options depending on an option supplied earlier in the
        command line.
    -   implemented with only a single C++ header file
    -   uses no C runtime or OS functions
    -   char, wchar_t and Windows TCHAR in the same program
    -   complete working examples included
    -   compiles cleanly at warning level 4 (Windows/VC.NET 2003), warning level
        3 (Windows/VC6) and -Wall (Linux/gcc)

    @section usage USAGE

    The SimpleOpt class is used by following these steps:

    <ol>
    <li> Include the SimpleOpt.h header file

        <pre>
        \#include "SimpleOpt.h"
        </pre>

    <li> Define an array of valid options for your program.

        <pre>
        @link CSimpleOptTempl::SOption CSimpleOpt::SOption @endlink g_rgOptions[] = {
            { OPT_FLAG, _T("-a"),     SO_NONE    }, // "-a"
            { OPT_FLAG, _T("-b"),     SO_NONE    }, // "-b"
            { OPT_ARG,  _T("-f"),     SO_REQ_SEP }, // "-f ARG"
            { OPT_HELP, _T("-?"),     SO_NONE    }, // "-?"
            { OPT_HELP, _T("--help"), SO_NONE    }, // "--help"
            SO_END_OF_OPTIONS                       // END
        };
        </pre>

        Note that all options must start with a hyphen even if the slash will
        be accepted. This is because the slash character is automatically
        converted into a hyphen to test against the list of options. For example,
        the following line matches both "-?" and "/?" (on Windows).

        <pre>
        { OPT_HELP, _T("-?"),     SO_NONE    }, // "-?"
        </pre>

   <li> Instantiate a CSimpleOpt object supplying argc, argv and the option table

        <pre>
        @link CSimpleOptTempl CSimpleOpt @endlink args(argc, argv, g_rgOptions);
        </pre>

   <li> Process the arguments by calling Next() until it returns false. On each call,
        first check for an error by calling LastError(), then either handle the error
        or process the argument.

        <pre>
        while (args.Next()) {
            if (args.LastError() == SO_SUCCESS) {
                handle option: use OptionId(), OptionText() and OptionArg()
            }
            else {
                handle error: see ESOError enums
            }
        }
        </pre>

   <li> Process all non-option arguments with File(), Files() and FileCount()

        <pre>
        ShowFiles(args.FileCount(), args.Files());
        </pre>

    </ol>

    @section notes NOTES

    -   In MBCS mode, this library is guaranteed to work correctly only when all
        option names use only ASCII characters.

    @section licence MIT LICENCE

    The licence text below is the boilerplate "MIT Licence" used from:
    http://www.opensource.org/licenses/mit-license.php

    Copyright (c) 2006-2007, Brodie Thiesfield

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is furnished
    to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*! @mainpage

    <table>
        <tr><th>Library     <td>SimpleOpt
        <tr><th>Author      <td>Brodie Thiesfield [code at jellycan dot com]
        <tr><th>Source      <td>http://code.jellycan.com/simpleopt/
    </table>

    @section SimpleOpt SimpleOpt

    A cross-platform library providing a simple method to parse almost any of the
    standard command-line formats in use today.

    See the @link SimpleOpt.h SimpleOpt @endlink documentation for full details.

    @section SimpleGlob SimpleGlob

    A cross-platform file globbing library providing the ability to
    expand wildcards in command-line arguments to a list of all matching files.

    See the @link SimpleGlob.h SimpleGlob @endlink documentation for full details.
*/

#ifndef INCLUDED_SimpleOpt
#define INCLUDED_SimpleOpt


//! Error values
typedef enum _ESOError
{
    SO_SUCCESS          =  0,   //!< No error
    SO_OPT_INVALID      = -1,   /*!< It looks like an option (it starts with a switch character),
                                     but it isn't registered in the option table. */
    SO_OPT_MULTIPLE     = -2,   /*!< Multiple options matched the supplied option text.
                                     Only returned when NOT using SO_O_EXACT. */
    SO_ARG_INVALID      = -3,   /*!< Option doesn't take an argument, but a combined argument
                                     was supplied. */
    SO_ARG_INVALID_TYPE = -4,   /*!< SO_REQ_CMB style-argument was supplied to a SO_REQ_SEP option
                                     Only returned when using SO_O_PEDANTIC. */
    SO_ARG_MISSING      = -5,   //!< Required argument was not supplied
    SO_ARG_INVALID_DATA = -6,   /*!< Option argument looks like another option.
                                     Only returned when NOT using SO_O_NOERR. */
} ESOError;

//! Option flags
enum _ESOFlags
{
    SO_O_EXACT    = 0x0001, /*!< Disallow partial matching of option names */
    SO_O_NOSLASH  = 0x0002, /*!< Disallow use of slash as an option marker on Windows.
                                 Un*x only ever recognizes a hyphen. */
    SO_O_SHORTARG = 0x0004, /*!< Permit arguments on single letter options with
                                 no equals sign. e.g. -oARG or -o[ARG] */
    SO_O_CLUMP    = 0x0008, /*!< Permit single character options to be clumped into
                                 a single option string. e.g. "-a -b -c" <==> "-abc" */
    SO_O_USEALL   = 0x0010, /*!< Process the entire argv array for options, including
                                 the argv[0] entry. */
    SO_O_NOERR    = 0x0020, /*!< Do not generate an error for invalid options. errors
                                 for missing arguments will still be generated. invalid
                                 options will be treated as files. invalid options in
                                 clumps will be silently ignored. */
    SO_O_PEDANTIC = 0x0040  /*!< Validate argument type pedantically. Return an error when
                                 a separated argument "-opt arg" is supplied by the user
                                 as a combined argument "-opt=arg". By default this is
                                 not considered an error. */
};

/*! Types of arguments that options may have. Note that some of the _ESOFlags are
    not compatible with all argument types. SO_O_SHORTARG requires that relevant
    options use either SO_REQ_CMB or SO_OPT. SO_O_CLUMP requires that relevant options
    use only SO_NONE.
 */
typedef enum _ESOArgType {
    SO_NONE,    //!< No argument.                 -o                 --opt
    SO_REQ_SEP, //!< Required separate argument.  -o ARG             --opt ARG
    SO_REQ_CMB, //!< Required combined argument.  -oARG    -o=ARG    --opt=ARG
    SO_OPT,     //!< Optional combined argument.  -o[ARG]  -o[=ARG]  --opt[=ARG]
    SO_MULTI    //!< Multiple separate arguments. -o ARG1 ARG2 ... --opt ARG1 ARG2 ...
} ESOArgType;

//! this option definition must be the last entry in the table
#define SO_END_OF_OPTIONS   { -1, NULL, SO_NONE }

#ifndef SO_ASSERT
#ifdef _DEBUG
# ifdef _WIN32
#  include <crtdbg.h>
#  define SO_ASSERT(b)  _ASSERTE(b)
# else
#  include <assert.h>
#  define SO_ASSERT(b)  assert(b)
# endif
#else
# define SO_ASSERT(b)   //!< assertion used to test input data
#endif
#endif

// ---------------------------------------------------------------------------
//                              MAIN TEMPLATE CLASS
// ---------------------------------------------------------------------------

/*! @brief Implementation of the SimpleOpt class */
template<class SOCHAR>
class CSimpleOptTempl
{
public:
    /*! @brief Structure used to define all known options. */
    struct SOption {
        int         nId;        //!< ID to return for this flag. Optional but must be >= 0
        SOCHAR *    pszArg;     //!< arg string to search for, e.g.  "open", "-", "-f", "--file"
                                // Note that on Windows the slash option marker will be converted
                                // to a hyphen so that "-f" will also match "/f".
        ESOArgType  nArgType;   //!< type of argument accepted by this option
    };

    /*! @brief Initialize the class. Init() must be called later. */
    CSimpleOptTempl() { Init(0, NULL, NULL, 0); }

    /*! @brief Initialize the class in preparation for use. */
    CSimpleOptTempl(int argc, SOCHAR * argv[], const SOption * a_rgOptions, int a_nFlags = 0) {
        Init(argc, argv, a_rgOptions, a_nFlags);
    }

    /*! @brief Initialize the class in preparation for calling Next.

        The table of options pointed to by a_rgOptions does not need to be valid
        at the time that Init() is called. However on every call to Next() the
        table pointed to must be a valid options table with the last valid
        entry set to SO_END_OF_OPTIONS.

        NOTE: the array pointed to by a_argv will be modified by this
        class and must not be used or modified outside of member calls to
        this class.

        @param a_argc       Argument array size
        @param a_argv       Argument array
        @param a_rgOptions  Valid option array
        @param a_nFlags     Optional flags to modify the processing of the arguments
    */
    void Init(int a_argc, SOCHAR * a_argv[], const SOption * a_rgOptions, int a_nFlags = 0);

    /*! @brief Change the current options table during option parsing.

        @param a_rgOptions  Valid option array
     */
    inline void SetOptions(const SOption * a_rgOptions) { m_rgOptions = a_rgOptions; }

    /*! @brief Change the current flags during option parsing.

        Note that changing the SO_O_USEALL flag here will have no affect.
        It must be set using Init() or the constructor.

        @param a_nFlags     Flags to modify the processing of the arguments
     */
    inline void SetFlags(int a_nFlags) { m_nFlags = a_nFlags; }

    /*! @brief Query if a particular flag is set */
    inline bool HasFlag(int a_nFlag) const { return (m_nFlags & a_nFlag) == a_nFlag; }

    /*! @brief Advance to the next option if available.

        When all options have been processed it will return false. When true has been
        returned, you must check for an invalid or unrecognized option using the
        LastError() method. This will be return an error value other than SO_SUCCESS
        on an error. All standard data (e.g. OptionText(), OptionArg(), OptionId(), etc)
        will be available depending on the error.

        After all options have been processed, the remaining files from the
        command line can be processed in same order as they were passed to
        the program.

        @return true    option or error available for processing
        @return false   all options have been processed
    */
    bool Next();

    /*! @brief Return the last error that occurred.

        This function must always be called before processing the current option.
        This function is available only when Next() has returned true.
     */
    inline ESOError LastError() const  { return m_nLastError; }

    /*! @brief Return the nId value from the options array for the current option.

        This function is available only when Next() has returned true.
     */
    inline int OptionId() const { return m_nOptionId; }

    /*! @brief Return the pszArg from the options array for the current option.

        This function is available only when Next() has returned true.
     */
    inline SOCHAR * OptionText() const { return m_pszOptionText; }

    /*! @brief Return the argument for the current option where one exists.

        If there is no argument for the option, this will return NULL.
        This function is available only when Next() has returned true.
     */
    inline SOCHAR * OptionArg() const { return m_pszOptionArg; }

    /*! @brief Validate and return the desired number of arguments.

        This is only valid when OptionId() has return the ID of an option
        that is registered as SO_MULTI. It may be called multiple times
        each time returning the desired number of arguments. Previously
        returned argument pointers are remain valid.

        If an error occurs during processing, NULL will be returned and
        the error will be available via LastError().

        @param n    Number of arguments to return.
     */
    SOCHAR ** MultiArg(int n);

    /*! @brief Returned the number of entries in the Files() array.

        After Next() has returned false, this will be the list of files (or
        otherwise unprocessed arguments).
     */
    inline int FileCount() const { return m_argc - m_nLastArg; }

    /*! @brief Return the specified file argument.

        @param n    Index of the file to return. This must be between 0
                    and FileCount() - 1;
     */
    inline SOCHAR * File(int n) const {
        SO_ASSERT(n >= 0 && n < FileCount());
        return m_argv[m_nLastArg + n];
    }

    /*! @brief Return the array of files. */
    inline SOCHAR ** Files() const { return &m_argv[m_nLastArg]; }

private:
    SOCHAR PrepareArg(SOCHAR * a_pszString) const;
    bool NextClumped();
    void ShuffleArg(int a_nStartIdx, int a_nCount);
    int LookupOption(const SOCHAR * a_pszOption) const;
    int CalcMatch(const SOCHAR *a_pszSource, const SOCHAR *a_pszTest) const;

    // Find the '=' character within a string.
    inline SOCHAR * FindEquals(SOCHAR *s) const {
        while (*s && *s != (SOCHAR)'=') ++s;
        return *s ? s : NULL;
    }

private:
    const SOption * m_rgOptions;     // pointer to options table as passed in to soInit()
    int             m_nFlags;       // flags for parsing the command line
    int             m_nOptionIdx;    // index of the current option in argv
    int             m_nOptionId;     // id of the current option (or -1 if invalid option)
    int             m_nNextOption;   // index of the next option to be processed
    int             m_nLastArg;      // last unprocessed argument, after this are files
    int             m_argc;          // argc to process
    SOCHAR **       m_argv;          // argv (rearranged during processing)
    SOCHAR *        m_pszOptionText; // text of the current option, e.g. "-f" or "--file"
    SOCHAR *        m_pszOptionArg;  // argument for the current option, e.g. "c:\blah.txt" (or NULL if no argument)
    SOCHAR *        m_pszClump;      // processing of clumped single character options
    SOCHAR          m_szShort[3];    // extract short option text from clumps and combined arguments
    ESOError        m_nLastError;    // error status from the last call
};

// ---------------------------------------------------------------------------
//                                  IMPLEMENTATION
// ---------------------------------------------------------------------------

template<class SOCHAR>
void
CSimpleOptTempl<SOCHAR>::Init(
    int             a_argc,
    SOCHAR *        a_argv[],
    const SOption * a_rgOptions,
    int             a_nFlags
    )
{
    m_argc           = a_argc;
    m_nLastArg       = a_argc;
    m_argv           = a_argv;
    m_rgOptions      = a_rgOptions;
    m_nLastError     = SO_SUCCESS;
    m_nOptionIdx     = 0;
    m_nOptionId      = -1;
    m_pszOptionText  = NULL;
    m_pszOptionArg   = NULL;
    m_nNextOption    = (a_nFlags & SO_O_USEALL) ? 0 : 1;
    m_szShort[0]     = (SOCHAR)'-';
    m_szShort[2]     = (SOCHAR)'\0';
    m_nFlags         = a_nFlags;
    m_pszClump       = NULL;
}

template<class SOCHAR>
bool
CSimpleOptTempl<SOCHAR>::Next()
{
    // process a clumped option string if appropriate
    if (m_pszClump && *m_pszClump) {
        // silently discard invalid clumped option
        bool bIsValid = NextClumped();
        while (*m_pszClump && !bIsValid && HasFlag(SO_O_NOERR)) {
            bIsValid = NextClumped();
        }

        // return this option if valid or we are returning errors
        if (bIsValid || !HasFlag(SO_O_NOERR)) {
            return true;
        }
    }
    SO_ASSERT(!m_pszClump || !*m_pszClump);
    m_pszClump = NULL;

    // init for the next option
    m_nOptionIdx    = m_nNextOption;
    m_nOptionId     = -1;
    m_pszOptionText = NULL;
    m_pszOptionArg  = NULL;
    m_nLastError    = SO_SUCCESS;

    // find the next option
    SOCHAR cFirst;
    int nTableIdx = -1;
    int nOptIdx = m_nOptionIdx;
    while (nTableIdx < 0 && nOptIdx < m_nLastArg) {
        // assumed argument
        m_pszOptionText = m_argv[nOptIdx];
        m_pszOptionArg  = NULL;

        // find this option in the options table
        cFirst = PrepareArg(m_pszOptionText);
        if (m_pszOptionText[0] == (SOCHAR)'-') {
            // find any combined argument string and remove equals sign
            m_pszOptionArg = FindEquals(m_pszOptionText);
            if (m_pszOptionArg) {
                *m_pszOptionArg++ = (SOCHAR)'\0';
            }
        }
        nTableIdx = LookupOption(m_pszOptionText);

        // if we didn't find this option but if it is a short form
        // option then we try the alternative forms
        if (nTableIdx < 0
            && !m_pszOptionArg
            && m_pszOptionText[0] == (SOCHAR)'-'
            && m_pszOptionText[1]
            && m_pszOptionText[1] != (SOCHAR)'-'
            && m_pszOptionText[2])
        {
            // test for a short-form with argument if appropriate
            if (HasFlag(SO_O_SHORTARG)) {
                m_szShort[1] = m_pszOptionText[1];
                int nIdx = LookupOption(m_szShort);
                if (nIdx >= 0
                    && (m_rgOptions[nIdx].nArgType == SO_REQ_CMB
                        || m_rgOptions[nIdx].nArgType == SO_OPT))
                {
                    m_pszOptionArg  = &m_pszOptionText[2];
                    m_pszOptionText = m_szShort;
                    nTableIdx       = nIdx;
                }
            }

            // test for a clumped short-form option string and we didn't
            // match on the short-form argument above
            if (nTableIdx < 0 && HasFlag(SO_O_CLUMP))  {
                m_pszClump = &m_pszOptionText[1];
                ++m_nNextOption;
                if (nOptIdx > m_nOptionIdx) {
                    ShuffleArg(m_nOptionIdx, nOptIdx - m_nOptionIdx);
                }
                return Next();
            }
        }

        // The option wasn't found. If it starts with a switch character
        // and we are not suppressing errors for invalid options then it
        // is reported as an error, otherwise it is data.
        if (nTableIdx < 0) {
            if (!HasFlag(SO_O_NOERR) && m_pszOptionText[0] == (SOCHAR)'-') {
                break;
            }
            m_pszOptionText[0] = cFirst;
            ++nOptIdx;
            if (m_pszOptionArg) {
                *(--m_pszOptionArg) = (SOCHAR)'=';
            }
        }
    }

    // end of options
    if (nOptIdx >= m_nLastArg) {
        if (nOptIdx > m_nOptionIdx) {
            ShuffleArg(m_nOptionIdx, nOptIdx - m_nOptionIdx);
        }
        return false;
    }
    ++m_nNextOption;

    // get the option id
    ESOArgType nArgType = SO_NONE;
    if (nTableIdx < 0) {
        m_nLastError = (ESOError) nTableIdx; // error code
    }
    else {
        m_nOptionId     = m_rgOptions[nTableIdx].nId;
        m_pszOptionText = m_rgOptions[nTableIdx].pszArg;

        // ensure that the arg type is valid
        nArgType = m_rgOptions[nTableIdx].nArgType;
        switch (nArgType) {
        case SO_NONE:
            if (m_pszOptionArg) {
                m_nLastError = SO_ARG_INVALID;
            }
            break;

        case SO_REQ_SEP:
            if (m_pszOptionArg) {
                // they wanted separate args, but we got a combined one, unless
                // we are being pedantic about things, just accept it.
                if (HasFlag(SO_O_PEDANTIC)) {
                    m_nLastError = SO_ARG_INVALID_TYPE;
                }
            }
            // more processing after we shuffle
            break;

        case SO_REQ_CMB:
            if (!m_pszOptionArg) {
                m_nLastError = SO_ARG_MISSING;
            }
            break;

        case SO_OPT:
            // nothing to do
            break;

        case SO_MULTI:
            // nothing to do. Caller must now check for valid arguments
            // using GetMultiArg()
            break;
        }
    }

    // shuffle the files out of the way
    if (nOptIdx > m_nOptionIdx) {
        ShuffleArg(m_nOptionIdx, nOptIdx - m_nOptionIdx);
    }

    // we need to return the separate arg if required, just re-use the
    // multi-arg code because it all does the same thing
    if (nArgType == SO_REQ_SEP && !m_pszOptionArg && m_nLastError == SO_SUCCESS) {
        SOCHAR ** ppArgs = MultiArg(1);
        if (ppArgs) {
            m_pszOptionArg = *ppArgs;
        }
    }

    return true;
}

template<class SOCHAR>
SOCHAR
CSimpleOptTempl<SOCHAR>::PrepareArg(
    SOCHAR * a_pszString
    ) const
{
#ifdef _WIN32
    // On Windows we can accept the forward slash as a single character
    // option delimiter, but it cannot replace the '-' option used to
    // denote stdin. On Un*x paths may start with slash so it may not
    // be used to start an option.
    if (!HasFlag(SO_O_NOSLASH)
        && a_pszString[0] == (SOCHAR)'/'
        && a_pszString[1]
        && a_pszString[1] != (SOCHAR)'-')
    {
        a_pszString[0] = (SOCHAR)'-';
        return (SOCHAR)'/';
    }
#endif
    return a_pszString[0];
}

template<class SOCHAR>
bool
CSimpleOptTempl<SOCHAR>::NextClumped()
{
    // prepare for the next clumped option
    m_szShort[1]    = *m_pszClump++;
    m_nOptionId     = -1;
    m_pszOptionText = m_szShort;
    m_pszOptionArg  = NULL;
    m_nLastError    = SO_SUCCESS;

    // lookup this option, ensure that we are using exact matching
    int nSavedFlags = m_nFlags;
    m_nFlags = SO_O_EXACT;
    int nTableIdx = LookupOption(m_pszOptionText);
    m_nFlags = nSavedFlags;

    // unknown option
    if (nTableIdx < 0) {
        m_nLastError = (ESOError) nTableIdx; // error code
        return false;
    }

    // valid option
    ESOArgType nArgType = m_rgOptions[nTableIdx].nArgType;
    if (nArgType == SO_NONE) {
        m_nOptionId = m_rgOptions[nTableIdx].nId;
        return true;
    }

    if (nArgType == SO_REQ_CMB && *m_pszClump) {
        m_nOptionId = m_rgOptions[nTableIdx].nId;
        m_pszOptionArg = m_pszClump;
        while (*m_pszClump) ++m_pszClump; // must point to an empty string
        return true;
    }

    // invalid option as it requires an argument
    m_nLastError = SO_ARG_MISSING;
    return true;
}

// Shuffle arguments to the end of the argv array.
//
// For example:
//      argv[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8" };
//
//  ShuffleArg(1, 1) = { "0", "2", "3", "4", "5", "6", "7", "8", "1" };
//  ShuffleArg(5, 2) = { "0", "1", "2", "3", "4", "7", "8", "5", "6" };
//  ShuffleArg(2, 4) = { "0", "1", "6", "7", "8", "2", "3", "4", "5" };
template<class SOCHAR>
void
CSimpleOptTempl<SOCHAR>::ShuffleArg(
    int a_nStartIdx,
    int a_nCount
    )
{
    SOCHAR * buf[200];
    int n, nSrc, nDst;
    int nTail = m_argc - a_nStartIdx - a_nCount;

    // make a copy of the elements to be moved
    nSrc = a_nStartIdx;
    nDst = 0;
    while (nDst < a_nCount) {
        buf[nDst++] = m_argv[nSrc++];
    }

    // move the tail down
    nSrc = a_nStartIdx + a_nCount;
    nDst = a_nStartIdx;
    for (n = 0; n < nTail; ++n) {
        m_argv[nDst++] = m_argv[nSrc++];
    }

    // append the moved elements to the tail
    nSrc = 0;
    nDst = a_nStartIdx + nTail;
    for (n = 0; n < a_nCount; ++n) {
        m_argv[nDst++] = buf[nSrc++];
    }

    // update the index of the last unshuffled arg
    m_nLastArg -= a_nCount;
}

// match on the long format strings. partial matches will be
// accepted only if that feature is enabled.
template<class SOCHAR>
int
CSimpleOptTempl<SOCHAR>::LookupOption(
    const SOCHAR * a_pszOption
    ) const
{
    int nBestMatch = -1;    // index of best match so far
    int nBestMatchLen = 0;  // matching characters of best match
    int nLastMatchLen = 0;  // matching characters of last best match

    for (int n = 0; m_rgOptions[n].nId >= 0; ++n) {
        // the option table must use hyphens as the option character,
        // the slash character is converted to a hyphen for testing.
        SO_ASSERT(m_rgOptions[n].pszArg[0] != (SOCHAR)'/');

        int nMatchLen = CalcMatch(m_rgOptions[n].pszArg, a_pszOption);
        if (nMatchLen == -1) {
            return n;
        }
        if (nMatchLen > 0 && nMatchLen >= nBestMatchLen) {
            nLastMatchLen = nBestMatchLen;
            nBestMatchLen = nMatchLen;
            nBestMatch = n;
        }
    }

    // only partial matches or no match gets to here, ensure that we
    // don't return a partial match unless it is a clear winner
    if (HasFlag(SO_O_EXACT) || nBestMatch == -1) {
        return SO_OPT_INVALID;
    }
    return (nBestMatchLen > nLastMatchLen) ? nBestMatch : SO_OPT_MULTIPLE;
}

// calculate the number of characters that match (case-sensitive)
// 0 = no match, > 0 == number of characters, -1 == perfect match
template<class SOCHAR>
int
CSimpleOptTempl<SOCHAR>::CalcMatch(
    const SOCHAR *  a_pszSource,
    const SOCHAR *  a_pszTest
    ) const
{
    if (!a_pszSource || !a_pszTest) {
        return 0;
    }

    // match and skip leading hyphens
    while (*a_pszSource == (SOCHAR)'-' && *a_pszSource == *a_pszTest) {
        ++a_pszSource; ++a_pszTest;
    }
    if (*a_pszSource == (SOCHAR)'-' || *a_pszTest == (SOCHAR)'-') {
        return 0;
    }

    // find matching number of characters in the strings
    int nLen = 0;
    while (*a_pszSource && *a_pszSource == *a_pszTest) {
        ++a_pszSource; ++a_pszTest; ++nLen;
    }

    // if we have exhausted the source...
    if (!*a_pszSource) {
        // and the test strings, then it's a perfect match
        if (!*a_pszTest) {
            return -1;
        }

        // otherwise the match failed as the test is longer than
        // the source. i.e. "--mant" will not match the option "--man".
        return 0;
    }

    // if we haven't exhausted the test string then it is not a match
    // i.e. "--mantle" will not best-fit match to "--mandate" at all.
    if (*a_pszTest) {
        return 0;
    }

    // partial match to the current length of the test string
    return nLen;
}

// calculate the number of characters that match (case-sensitive)
// 0 = no match, > 0 == number of characters, -1 == perfect match
template<class SOCHAR>
SOCHAR **
CSimpleOptTempl<SOCHAR>::MultiArg(
    int a_nCount
    )
{
    // ensure we have enough arguments
    if (m_nNextOption + a_nCount > m_nLastArg) {
        m_nLastError = SO_ARG_MISSING;
        return NULL;
    }

    // our argument array
    SOCHAR ** rgpszArg = &m_argv[m_nNextOption];

    // Ensure that each of the following don't start with an switch character.
    // Only make this check if we are returning errors for unknown arguments.
    if (!HasFlag(SO_O_NOERR)) {
        for (int n = 0; n < a_nCount; ++n) {
            SOCHAR ch = PrepareArg(rgpszArg[n]);
            if (rgpszArg[n][0] == (SOCHAR)'-') {
                rgpszArg[n][0] = ch;
                m_nLastError = SO_ARG_INVALID_DATA;
                return NULL;
            }
            rgpszArg[n][0] = ch;
        }
    }

    // all good
    m_nNextOption += a_nCount;
    return rgpszArg;
}


// ---------------------------------------------------------------------------
//                                  TYPE DEFINITIONS
// ---------------------------------------------------------------------------

typedef CSimpleOptTempl<char>    CSimpleOptA; /*!< @brief ASCII/MBCS version of CSimpleOpt */
typedef CSimpleOptTempl<wchar_t> CSimpleOptW; /*!< @brief wchar_t version of CSimpleOpt */
#if defined(_UNICODE)
# define CSimpleOpt CSimpleOptW   /*!< @brief TCHAR version dependent on if _UNICODE is defined */
#else
# define CSimpleOpt CSimpleOptA   /*!< @brief TCHAR version dependent on if _UNICODE is defined */
#endif

#endif // INCLUDED_SimpleOpt
