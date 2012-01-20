Decider('MD5-timestamp')

env = Environment()

env32 = env.Clone(CCFLAGS="-m32", LINKFLAGS="-m32")
env64 = env.Clone(CCFLAGS="-m64", LINKFLAGS="-m64")

SConscript('plugin/SConscript',
           exports={'env': env32},
           variant_dir="plugin/build-i386",
           duplicate=0)
SConscript('plugin/SConscript',
           exports={'env': env64},
           variant_dir="plugin/build-x86_64",
           duplicate=0)

env.Command('chrome/libembedded-emacs-launcher-64.so',
            'plugin/build-x86_64/libembedded-emacs-launcher.so',
            Copy("$TARGET", "$SOURCE"));
env.Command('chrome/libembedded-emacs-launcher-32.so',
            'plugin/build-i386/libembedded-emacs-launcher.so',
            Copy("$TARGET", "$SOURCE"));

env.Command('chrome/libembedded-emacs-container-64.so',
            'plugin/build-x86_64/libembedded-emacs-container.so',
            Copy("$TARGET", "$SOURCE"));
env.Command('chrome/libembedded-emacs-container-32.so',
            'plugin/build-i386/libembedded-emacs-container.so',
            Copy("$TARGET", "$SOURCE"));

env.Command('chrome/LICENSE', 'LICENSE',
            Copy("$TARGET", "$SOURCE"));
