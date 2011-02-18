Decider('MD5-timestamp')

env = Environment()
env.SConscript('plugin/SConscript')
env.Command('chrome/libembedded-emacs-launcher.so',
            'plugin/libembedded-emacs-launcher.so',
            Copy("$TARGET", "$SOURCE"));
env.Command('chrome/libembedded-emacs-container.so',
            'plugin/libembedded-emacs-container.so',
            Copy("$TARGET", "$SOURCE"));
