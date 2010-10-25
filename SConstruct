Decider('MD5-timestamp')

env = Environment()
def CheckPKGConfig(context, version):
    context.Message( 'Checking for pkg-config... ' )
    ret = context.TryAction('pkg-config --atleast-pkgconfig-version=%s' % version)[0]
    context.Result( ret )
    return ret

def CheckPKG(context, name):
    context.Message( 'Checking for %s... ' % name )
    ret = context.TryAction('pkg-config --exists \'%s\'' % name)[0]
    context.Result( ret )
    return ret

conf = Configure(env, custom_tests = { 'CheckPKGConfig' : CheckPKGConfig,
                                       'CheckPKG' : CheckPKG })
if not conf.CheckPKGConfig('0.15.0'):
    print 'pkg-config >= 0.15.0 not found.'
    Exit(1)
if not conf.CheckPKG('nspr'):
    print 'NSPR not found.'
    Exit(1)
if not conf.CheckPKG('glib-2.0'):
    print 'GLib not found.'
    Exit(1)
if not conf.CheckPKG('gthread-2.0'):
    print 'GLib not found.'
    Exit(1)
env = conf.Finish()

# Only pull in the header files; link against NSPR later if needbe.
env.ParseConfig('pkg-config --cflags nspr')
env.ParseConfig('pkg-config --cflags --libs glib-2.0')
env.ParseConfig('pkg-config --cflags --libs gthread-2.0')

env.SharedLibrary('emacs-npapi', ['browser.cc',
                                  'emacsinstance.cc',
                                  'identifiers.cc',
                                  'plugin.cc',
                                  'scriptobject.cc',
                                  'task.cc',
                                  ])
