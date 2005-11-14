# -*- coding: utf-8 -*-

import copy, os, os.path

################################################################################
#
# 定义编译选项
#
################################################################################

# 定义文件签名
#SourceSignatures( "timestamp" )
#TargetSignatures( "content" )

# 记录当前的OS
conf = {
    'platform' : Environment()['PLATFORM'],
}

# 读取环境变量
def getenv( name, defval ):
    if name in os.environ: return os.environ[name]
    else: return defval

# 定义缺省的选项
if 'win32' == conf['platform']: default_compiler = 'vc71'
else: default_compiler = 'gcc'
default_configs = {
    'genconf'           : getenv('GN_BUILD_GENCONF', 0), # force (re)generation of build configuration
    'enable_cache'      : getenv('GN_BUILD_ENABLE_CACHE', 1), # enable build cache
    'variant'           : getenv('GN_BUILD_VARIANT', 'debug'),
    'compiler'          : getenv('GN_BUILD_COMPILER', default_compiler), # default compiler
    'enable_cg'         : getenv('GN_BUILD_ENABLE_CG', 1), # use Cg by default.
    'enable_profile'    : getenv('GN_BUILD_ENABLE_PROFILE', 0), # disabled by default
    'run_unit_tests'    : getenv('GN_BUILD_RUN_UNIT_TESTS', 0), # Do not run unit tests after build by default.
    }

# 是否强制生成配置信息
conf['genconf']  = ARGUMENTS.get('conf', default_configs['genconf'] )

# 是否使用build cache
conf['enable_cache']  = ARGUMENTS.get('cache', default_configs['enable_cache'] )

# 定义编译器类型
conf['compiler'] = ARGUMENTS.get('compiler', default_configs['compiler'])
if not conf['compiler'] in Split('%s vc80 vc80-x64 icl'%default_compiler):
    print 'Invalid compiler type! Must be one of (%s vc80 vc80-x64 icl)'%default_compiler;
    Exit(-1)

# 定义编译类型
# can be 'debug', 'release', 'stdbg', 'strel', 'static', 'all'
conf['variant'] = ARGUMENTS.get('variant', default_configs['variant'] )
if not conf['variant'] in Split('debug release stdbg strel all'):
    print 'Invalid variant type! Must be one of (debug releae stdbg strel all)';
    Exit(-1)

# 是否支持Cg语言.
conf['enable_cg']  = ARGUMENTS.get('cg', default_configs['enable_cg'] )

# 是否启用profiler.
conf['enable_profile'] = ARGUMENTS.get('prof', default_configs['enable_profile'] )

# 是否自动运行UnitTest.
conf['run_unit_tests'] = ARGUMENTS.get('ut', default_configs['run_unit_tests'] )

# 定义target dict
targets = [{}, {}, {}, {}]
alias   = []

################################################################################
#
# 创建缺省编译环境
#
################################################################################

opts = Options()
opts.Add(
    'conf',
    'Force (re)generation of build configuration. (GN_BUILD_GENCONF)',
    default_configs['genconf'] )
opts.Add(
    'cache',
    'Use scons build cache. (GN_BUILD_ENABLE_CACHE)',
    default_configs['enable_cache'] )
opts.Add(
    'compiler',
    'Specify compiler. Could be : %s vc80 vc80-x64 icl. (GN_BUILD_COMPILER)'%default_compiler,
    default_configs['compiler'] )
opts.Add(
    'variant',
    'Specify variant. Could be : debug, release, stdbg, strel or all. (GN_BUILD_VARIANT)',
    default_configs['variant'] )
opts.Add(
    'cg',
    'Support Cg language or not. (GN_BUILD_ENABLE_CG)',
    default_configs['enable_cg'] )
opts.Add(
    'prof',
    'Enable performance profiler. (GN_BUILD_ENABLE_PROFILE)',
    default_configs['enable_profile'] )
opts.Add(
    'ut',
    'Run unit tests after build. (GN_BUILD_RUN_UNIT_TESTS)',
    default_configs['run_unit_tests'] )

env = Environment( options = opts )

################################################################################
#
# Build variants
#
################################################################################

if 'all' in COMMAND_LINE_TARGETS: conf['variant'] = 'all'

if 'all' == conf['variant'] or 'debug' == conf['variant'] or 'debug' in COMMAND_LINE_TARGETS:
    c = copy.copy(conf);
    c['variant'] = 'debug'
    SConscript(
        'SConscript',
        exports={
            'GN_conf'    : c,
            'GN_targets' : targets[0],
            'GN_alias'   : alias,
            },
        )

if 'all' == conf['variant'] or 'release' == conf['variant'] or 'release' in COMMAND_LINE_TARGETS:
    c = copy.copy(conf);
    c['variant'] = 'release'
    SConscript(
        'SConscript',
        exports={
            'GN_conf'    : c,
            'GN_targets' : targets[1],
            'GN_alias'   : alias,
            },
        )

if 'all' == conf['variant'] or 'stdbg' == conf['variant'] or 'stdbg' in COMMAND_LINE_TARGETS:
    c = copy.copy(conf);
    c['variant'] = 'stdbg'
    SConscript(
        'SConscript',
        exports={
            'GN_conf'    : c,
            'GN_targets' : targets[2],
            'GN_alias'   : alias,
            },
        )

if 'all' == conf['variant'] or 'strel' == conf['variant'] or 'strel' in COMMAND_LINE_TARGETS:
    c = copy.copy(conf);
    c['variant'] = 'strel'
    SConscript(
        'SConscript',
        exports={
            'GN_conf'    : c,
            'GN_targets' : targets[3],
            'GN_alias'   : alias,
            },
        )

################################################################################
#
# Build and install manual headers and manual
#
################################################################################

manual = SConscript( 'src/priv/manual/SConscript' )
if manual:
    manual = { 'GNman' : env.Install( os.path.join( 'bin', 'sdk', 'manual' ), manual ) }
else:
    manual = {}

################################################################################
#
# Install public headers
#
################################################################################

# install SDK headers
target = os.path.join( 'bin','sdk', 'inc' )
source = os.path.join( '#src','priv','inc' )
for src in env.GN_glob(source, True):
    dst = os.path.join( target, env.GN_relpath(src,source) )
    env.Command( dst, src, Copy('$TARGET', '$SOURCE') )
    env.Alias( 'sdk', dst )

################################################################################
#
# 定义 phony targets
#
################################################################################

env.Alias( 'msvc', '#msvc' )

all_targets = targets[0].items() + targets[1].items() + targets[2].items() + targets[3].items() + manual.items()
all_targets.sort()
for x in all_targets:
    env.Alias( x[0], x[1] )
    env.Alias( 'all', x[1] )
    env.Default( x[1] )
env.Default( Split('samples sdk') )

################################################################################
#
# 定义 help screen
#
################################################################################

targets_text = ''
targets_text += '%16s : %s\n'%( 'all', 'Build all targets of all variants' )
targets_text += '%16s : %s\n'%( 'samples', 'Build samples' )
targets_text += '%16s : %s\n'%( 'sdk', 'Build garnet SDK' )
targets_text += '%16s : %s\n'%( 'msvc', 'Build MSVC projects' )
for x in all_targets:
    targets_text += '%16s : %s\n'%( x[0], env.File(x[1]) )

help_text = """
Usage:
    scons [options] [target1 target2 ...]

Options:%s

Targets:
%s
""" % (
    opts.GenerateHelpText(env),
    targets_text,
    )
Help( help_text )
