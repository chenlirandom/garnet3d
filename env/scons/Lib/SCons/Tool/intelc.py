"""SCons.Tool.icl

Tool-specific initialization for the Intel C/C++ compiler.
Supports Linux and Windows compilers, v7 and up.

There normally shouldn't be any need to import this module directly.
It will usually be imported through the generic SCons.Tool.Tool()
selection method.

"""

#
# Copyright (c) 2001, 2002, 2003, 2004 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

__revision__ = "src\engine\SCons\Tool\intelc.py 0.96 2005/11/07 20:52:44 chenli"

import math, sys, os.path, glob, string, re

is_win32 = sys.platform == 'win32'
is_linux = sys.platform == 'linux2'

if is_win32:
    import SCons.Tool.msvc
elif is_linux:
    import SCons.Tool.gcc
import SCons.Util
import SCons.Warnings

# Exceptions for this tool
class IntelCError(SCons.Errors.InternalError):
    pass
class MissingRegistryError(IntelCError): # missing registry entry
    pass
class MissingDirError(IntelCError):     # dir not found
    pass
class NoRegistryModuleError(IntelCError): # can't read registry at all
    pass

def uniquify(s):
    """Return a sequence containing only one copy of each unique element from input sequence s.
    Does not preserve order.
    Input sequence must be hashable (i.e. must be usable as a dictionary key)."""
    u = {}
    for x in s:
        u[x] = 1
    return u.keys()

def linux_ver_normalize(vstr):
    """Normalize a Linux compiler version number.
    Intel changed from "80" to "9.0" in 2005, so we assume if the number
    is greater than 60 it's an old-style number and otherwise new-style.
    Always returns an old-style float like 80 or 90 for compatibility with Windows.
    Shades of Y2K!"""
    f = float(vstr)
    if is_win32:
        return f
    else:
        if f < 60: return f * 10.0
        else: return f

def vercmp(a, b):
    """Compare strings as floats,
    but Intel changed Linux naming convention at 9.0"""
    return cmp(linux_ver_normalize(b), linux_ver_normalize(a))

def get_default_abi():
    """ Return default api on current platform"""
    if is_linux:
        # Check if we are on 64-bit linux, default to 64 then.
        uname_m = os.uname()[4]
        if uname_m == 'x86_64':
            return 'x86_64'
        else:
            return 'ia32'
    else:
        if 'PROCESSOR_ARCHITEW6432' in os.environ:
            if 'AMD64' == os.environ['PROCESSOR_ARCHITEW6432']:
                return 'em64t'
            else:
                return 'ia64'
        else:
            return 'ia32'

def get_intel_registry_value(valuename, version=None, abi=None):
    """
    Return a value from the Intel compiler registry tree. (Win32 only)
    """

    # Open the key:
    K = 'Software\\Intel\\Compilers\\C++\\' + version + '\\'+abi.upper()
    try:
        k = SCons.Util.RegOpenKeyEx(SCons.Util.HKEY_LOCAL_MACHINE, K)
    except SCons.Util.RegError:
        raise MissingRegistryError, \
              "%s was not found in the registry, for Intel compiler version %s, abi='%s'"%(K, version,abi)

    # Get the value:
    try:
        v = SCons.Util.RegQueryValueEx(k, valuename)[0]
        return v  # or v.encode('iso-8859-1', 'replace') to remove unicode?
    except SCons.Util.RegError:
        raise MissingRegistryError, \
              "%s\\%s was not found in the registry."%(K, value)


def get_default_compiler_version( abi ):
    """Returns the default (highest) compiler version of specif ABI. Return None if failed.
    """
    versions=[]
    if is_win32:
        keyname = 'Software\\Intel\\Compilers\\C++'
        try:
            k = SCons.Util.RegOpenKeyEx( SCons.Util.HKEY_LOCAL_MACHINE, keyname )
        except WindowsError:
            return []
        i = 0
        versions = []
        try:
            while i < 100:
                subkey = SCons.Util.RegEnumKey(k, i) # raises EnvironmentError
                # Check that this refers to an existing dir.
                # This is not 100% perfect but should catch common
                # installation issues like when the compiler was installed
                # and then the install directory deleted or moved (rather
                # than uninstalling properly), so the registry values
                # are still there.
                try:
                    d = get_intel_registry_value('ProductDir', subkey, abi.upper() )
                    if os.path.exists(d):
                        versions.append(subkey)
                    else:
                        # Registry points to nonexistent dir.  Ignore this version.
                        print "Ignoring "+str(get_intel_registry_value('ProductDir', subkey, abi.upper()))
                except MissingRegistryError:
                    pass;
                i = i + 1
        except EnvironmentError:
            # no more subkeys
            pass
    elif is_linux:
        for d in glob.glob('/opt/intel_cc_*'):
            # Typical dir here is /opt/intel_cc_80.
            versions.append(re.search(r'cc_(.*)$', d).group(1))
        for d in glob.glob('/opt/intel/cc*/*'):
            # Typical dir here is /opt/intel/cc/9.0 for IA32,
            # /opt/intel/cce/9.0 for EMT64 (AMD64)
            versions.append(re.search(r'([0-9.]+)$', d).group(1))

    if 0 == len(versions): return None;

    # found!
    versions = uniquify(versions)
    versions.sort(vercmp)
    return versions[0]

def get_intel_compiler_top(version, abi):
    """
    Return the main path to the top-level dir of the Intel compiler,
    using the given version.
    The compiler will be in <top>/bin/icl.exe (icc on linux),
    the include dir is <top>/include, etc.
    """
    if is_win32:
        if not SCons.Util.can_read_reg:
            raise NoRegistryModuleError, "No Windows registry module was found"
        top = get_intel_registry_value('ProductDir', version, abi)
        if not os.path.exists(os.path.join(top, "Bin", "icl.exe")):
            raise MissingDirError, \
                  "Can't find Intel compiler in %s"%(top)
    elif is_linux:
        # first dir is new (>=9.0) style, second is old (8.0) style.
        dirs=('/opt/intel/cc/%s', '/opt/intel_cc_%s')
        if abi == 'x86_64':
            dirs=('/opt/intel/cce/%s',)  # 'e' stands for 'em64t', aka x86_64 aka amd64
        top=None
        for d in dirs:
            if os.path.exists(os.path.join(d%version, "bin", "icc")):
                top = d%version
                break
        if not top:
            raise MissingDirError, \
                  "Can't find version %s Intel compiler in %s (abi='%s')"%(version,top, abi)
    return top


def generate(env):
    """Add Builders and construction variables for Intel C/C++ compiler
    to an Environment.
    args:
      version: (string) compiler version to use, like "80"
      abi:     (string) 'win32' or whatever Itanium version wants
      topdir:  (string) compiler top dir, like
                         "c:\Program Files\Intel\Compiler70"
                        If topdir is used, version and abi are ignored.
      verbose: (int)    if >0, prints compiler version used.
    """
    if not (is_linux or is_win32):
        # can't handle this platform
        return

    # determin the abi
    abi = env.get( 'ICL_ABI', None )
    if not abi :
        abi = get_default_abi()
        if not abi :
            raise SCons.Errors.UserError, 'Fail to determine default Intel compiler ABI!';

    # determine version
    version = env.get( 'ICL_VERSION', None )
    if not version:
        version = get_default_compiler_version( abi )
        if not version:
            raise SCons.Errors.UserError, 'Fail to find default Intel C++ compiler for ABI=%s'%(abi)

    # deterimin compiler root
    topdir = get_intel_compiler_top(version, abi)

    if not topdir:

        # Normally this is an error, but it might not be if the compiler is
        # on $PATH and the user is importing their env.
        class ICLTopDirWarning(SCons.Warnings.Warning):
            pass
        if is_linux and not env.Detect('icc') or \
           is_win32 and not env.Detect('icl'):

            SCons.Warnings.enableWarningClass(ICLTopDirWarning)
            SCons.Warnings.warn(ICLTopDirWarning,
                                "Failed to find Intel compiler for version='%s', abi='%s'"%
                                (str(version), str(abi)))
        else:
            # should be cleaned up to say what this other version is
            # since in this case we have some other Intel compiler installed
            SCons.Warnings.enableWarningClass(ICLTopDirWarning)
            SCons.Warnings.warn(ICLTopDirWarning,
                                "Can't find Intel compiler top dir for version='%s', abi='%s'"%
                                    (str(version), str(abi)))

    else:

        ## print compiler version
        #print "Intel C compiler: using version '%s' (%g), abi %s, in '%s'"%\
        #      (version, linux_ver_normalize(version),abi,topdir)
        #if is_linux:
        #    # Show the actual compiler version by running the compiler.
        #    os.system('%s/bin/icc --version'%topdir)

        if is_win32:
            # setup Platform SDK
            if 'ia32' == abi:
                SCons.Tool.msvc.generate(env)
            if 'em64t' == abi:
                if float(env.get('MSVS_VERSION',0)) < 8.0:
                    raise SCons.Errors.UserError, 'Intel compiler (version=%s, abi=%s) requires VS.NET 2005 or above'%(version,abi)
                if env.get('MSVS_PLATFORM','') != 'x64':
                    raise SCons.Errors.UserError, 'Intel compiler (version=%s, abi=%s) requires MSVS_PLATFORM to be set to "x64"'%(version,abi)
                SCons.Tool.msvc.generate(env)
                psroot = env.get( 'MSVS_PLATFORMSDK', None )
                if not psroot:
                    # TODO: try parse platform sdk directroy from icl enviroment batch file.
                    psroot = "C:\\Program Files\\Microsoft Platform SDK"
                    class DefaultPlatformSDKWarning(SCons.Warnings.Warning):
                        pass
                    SCons.Warnings.enableWarningClass( DefaultPlatformSDKWarning )
                    SCons.Warnings.warn( DefaultPlatformSDKWarning, "Fail to detect Microsoft Platform SDK path (set env['MSVS_PLATFORMSDK'] please). Use default one: %s"%psroot )
                paths = (
                    ('INCLUDE', 'include'),
                    ('INCLUDE', 'include\\crt'),
                    ('INCLUDE', 'include\\crt\\sys'),
                    ('INCLUDE', 'include\\mfc'),
                    ('INCLUDE', 'include\\atl'),
                    ('LIB', 'Lib\\AMD64'), # TODO: what about IA64?
                    ('LIB', 'Lib\\AMD64\\atlmfc'), # TODO: IA64
                    ('PATH', 'Bin\\Win64\\x86\\AMD64') ) # TODO: IA64
                for p in paths:
                    dir = os.path.join( psroot, p[1] )
                    if not os.path.exists( dir ):
                        raise SCons.Errors.UserError, \
                            'Microsoft Platform SDK directory "%s" not found,' \
                            'which is required by Intel compiler (version=%s, abi=%s)'%(dir,version,abi)
                    env.PrependENVPath( p[0], dir )
            elif 'ia64' == abi:
                raise SCons.Errors.UserError, "Unsupport icl ABI : %s"%abi
        elif is_linux:
            SCons.Tool.gcc.generate(env)

        # setup ICL paths
        env['INTEL_C_COMPILER_TOP'] = topdir
        if is_linux:
            paths={'INCLUDE'         : 'include',
                   'LIB'             : 'lib',
                   'PATH'            : 'bin',
                   'LD_LIBRARY_PATH' : 'lib'}
            for p in paths:
                env.PrependENVPath(p, os.path.join(topdir, paths[p]))
        if is_win32:
            #       env key    reg valname   default subdir of top
            paths=(('INCLUDE', 'IncludeDir', 'Include'),
                   ('LIB'    , 'LibDir',     'Lib'),
                   ('PATH'   , 'BinDir',     'Bin'))
            # Each path has a registry entry, use that or default to subdir
            for p in paths:
                try:
                    path=get_intel_registry_value(p[1], version, abi)
                    # These paths may have $(ICInstallDir)
                    # which needs to be substituted with the topdir.
                    path=path.replace('$(ICInstallDir)', topdir + os.sep)
                except IntelCError:
                    # Couldn't get it from registry: use default subdir of topdir
                    env.PrependENVPath(p[0], os.path.join(topdir, p[2]))
                else:
                    env.PrependENVPath(p[0], string.split(path, os.pathsep))
                    # print "ICL %s: %s, final=%s"%(p[0], path, str(env['ENV'][p[0]]))

    if is_win32:
        env['CC']        = 'icl'
        env['CXX']       = 'icl'
        env['AR']        = 'xilib'
        env['LINK']      = 'xilink'
        if float(env.get('MSVS_VERSION',0)) >= 8.0:
            env.Append( LINKFLAGS = ['/MANIFEST'] )
        #env.Append( CCFLAGS = ['/QH'] )
        #env.Append( LINKFLAGS = ['/VERBOSE:LIB'] )
    else:
        env['CC']        = 'icc'
        env['CXX']       = 'icpc'
        env['LINK']      = '$CC'
        env['AR']        = 'xiar'
        env['LD']        = 'xild' # not used by default

    # This is not the exact (detailed) compiler version,
    # just the major version as determined above or specified
    # by the user.  It is a float like 80 or 90, in normalized form for Linux
    # (i.e. even for Linux 9.0 compiler, still returns 90 rather than 9.0)
    if version:
        env['INTEL_C_COMPILER_VERSION']=linux_ver_normalize(version)

    if is_win32:
        # Look for license file dir
        # in system environment, registry, and default location.
        envlicdir = os.environ.get("INTEL_LICENSE_FILE", '')
        K = ('SOFTWARE\Intel\Licenses')
        try:
            k = SCons.Util.RegOpenKeyEx(SCons.Util.HKEY_LOCAL_MACHINE, K)
            reglicdir = SCons.Util.RegQueryValueEx(k, "w_cpp")[0]
        except (AttributeError, SCons.Util.RegError):
            reglicdir = ""
        defaultlicdir = r'C:\Program Files\Common Files\Intel\Licenses'

        licdir = None
        for ld in [envlicdir, reglicdir]:
            if ld and os.path.exists(ld):
                licdir = ld
                break
        if not licdir:
            licdir = defaultlicdir
            if not os.path.exists(licdir):
                class ICLLicenseDirWarning(SCons.Warnings.Warning):
                    pass
                SCons.Warnings.enableWarningClass(ICLLicenseDirWarning)
                SCons.Warnings.warn(ICLLicenseDirWarning,
                                    "Intel license dir was not found."
                                    "  Tried using the INTEL_LICENSE_FILE environment variable (%s), the registry (%s) and the default path (%s)."
                                    "  Using the default path as a last resort."
                                        % (envlicdir, reglicdir, defaultlicdir))
        env['ENV']['INTEL_LICENSE_FILE'] = licdir

def exists(env):
    if not (is_linux or is_win32):
        # can't handle this platform
        return 0

    try:
        versions = get_all_compiler_versions()
    except (SCons.Util.RegError, IntelCError):
        versions = None
    detected = versions is not None and len(versions) > 0
    if not detected:
        # try env.Detect, maybe that will work
        if is_win32:
            return env.Detect('icl')
        elif is_linux:
            return env.Detect('icc')
    return detected

# end of file
