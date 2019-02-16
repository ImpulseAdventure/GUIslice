#!/usr/bin/env sh

##############################################################################
##
##  ${applicationName} start up script for LINUX
##
##############################################################################

# APP_HOME is set by the install.sh
APP_HOME=<APPHOME>
cd "\$APPHOME"

APP_NAME="${applicationName}"
APP_BASE_NAME=`basename "\$0"`

# Add default JVM options here. You can also use JAVA_OPTS and ${optsEnvironmentVar} to pass JVM options to this script.
DEFAULT_JVM_OPTS=${defaultJvmOpts}

# Use the maximum available, or set MAX_FD != -1 to use that value.
MAX_FD="maximum"

warn ( ) {
    echo "\$*"
}

die ( ) {
    echo
    echo "\$*"
    echo
    exit 1
}

CLASSPATH=$classpath

# Determine the Java command to use to start the JVM.
# Test to see if string has non-zero length
if [ -n "\$JAVA_HOME" ] ; then
    # JAVA_HOME not set so use whatever install.sh came up with
    JAVA_HOME=<JAVA_LOCATION>
    JAVACMD="\$JAVA_HOME/bin/java"
    # Test to see if file exists and is executable
    if [ ! -x "\$JAVACMD" ] ; then
        die "ERROR: JAVA_HOME is set to an invalid directory: \$JAVA_HOME

Please edit the JAVA_HOME variable in this script at line 50 
to match the location of your Java installation."
    fi
else
    JAVACMD="java"
    which java >/dev/null 2>&1 || die "ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.

Please edit the JAVA_HOME variable in this script at line 50 
to match the location of your Java installation."
fi

# Increase the maximum file descriptors if we can.
MAX_FD_LIMIT=`ulimit -H -n`
if [ \$? -eq 0 ] ; then
    if [ "\$MAX_FD" = "maximum" -o "\$MAX_FD" = "max" ] ; then
        MAX_FD="\$MAX_FD_LIMIT"
    fi
    ulimit -n \$MAX_FD
    if [ \$? -ne 0 ] ; then
        warn "Could not set maximum file descriptor limit: \$MAX_FD"
    fi
else
    warn "Could not query maximum file descriptor limit: \$MAX_FD_LIMIT"
fi

# Escape application args
save ( ) {
    for i do printf %s\\\\n "\$i" | sed "s/'/'\\\\\\\\''/g;1s/^/'/;\\\$s/\\\$/' \\\\\\\\/" ; done
    echo " "
}
APP_ARGS=\$(save "\$@")

# Collect all arguments for the java command, following the shell quoting and substitution rules
eval set -- \$DEFAULT_JVM_OPTS \$JAVA_OPTS \$${optsEnvironmentVar} <% if ( appNameSystemProperty ) { %>"\"-D${appNameSystemProperty}=\$APP_BASE_NAME\"" <% } %>-classpath "\"\$CLASSPATH\"" ${mainClassName} "\$APP_ARGS"

exec "\$JAVACMD" "\$@"
