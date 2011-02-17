Decider('MD5-timestamp')

env = Environment()
env.SConscript('plugin/SConscript')
env.Command('chrome/libemacs-npapi.so', 'plugin/libemacs-npapi.so',
            Copy("$TARGET", "$SOURCE"));
env.Command('chrome/libembedded-emacs-container.so',
            'plugin/libembedded-emacs-container.so',
            Copy("$TARGET", "$SOURCE"));
