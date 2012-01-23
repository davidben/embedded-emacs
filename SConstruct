Decider('MD5-timestamp')

env = Environment()

variants = {
    'i386':   env.Clone(CCFLAGS="-m32", LINKFLAGS="-m32"),
    'x86_64': env.Clone(CCFLAGS="-m64", LINKFLAGS="-m64"),
}

for name, variant_env in variants.items():
    SConscript('plugin/SConscript',
               exports={'env': variant_env},
               variant_dir='plugin/build-%s' % name,
               duplicate=0)

    env.Command('chrome/libembedded-emacs-launcher-%s.so' % name,
                'plugin/build-%s/libembedded-emacs-launcher.so' % name,
                Copy("$TARGET", "$SOURCE"));
    env.Command('chrome/libembedded-emacs-container-%s.so' % name,
                'plugin/build-%s/libembedded-emacs-container.so' % name,
                Copy("$TARGET", "$SOURCE"));

env.Command('chrome/LICENSE', 'LICENSE',
            Copy("$TARGET", "$SOURCE"));
