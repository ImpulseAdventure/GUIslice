#!/usr/bin/env sh

##############################################################################
##
##  GUIsliceBuilder install script for LINUX
##
##############################################################################

# This script will create and copy a desktop file to the user's dir.
# If called with the "-u" option, it will undo the changes.

# Get absolute path from which this script file was executed
# (Could be changed to "pwd -P" to resolve symlinks to their target)
SCRIPT_PATH=$( cd $(dirname $0) ; pwd )
cd "${SCRIPT_PATH}"
export PATH=.:$PATH

# Default mode is to install.
UNINSTALL=false

# Install files by first creating tmp folder, then creating and copy desktop file.
install_files() {

  # Display start message
  printf "Welcome to the GUIslice Installer\n"
  printf "This script will install GUIslice Builder into your Desktop\n"
  printf "To uninstall this App simply re-run this script with -u argument\n\n"
  
  # Create startup script
  printf "The Builder requires the location of Java 1.8\n"
  printf "If you have more then one Java version installed\n"
  printf "you need to identify what directory has Java 8.\n" 
  printf "You can exit this script with ^c and use this command to find it.\n\n"

  printf "sudo update-alternatives --config java \n\n"
  printf "If Arduino IDE is installed you can use the Arduino built in Java\n"
  printf "Examples:\n"
  printf "/usr/lib/jvm/java-8-openjdk-amd64/jre\n"
  printf "$HOME/arduino-1.8.7/java\n\n"

  # Ask user where we can find Java - loop until a valid answer is given
  JAVA_PATH=""
  JAVACMD=""
  while [ ! -d "$JAVA_PATH" ]
  do
    printf "\nEnter location of Java 1.8: "
    read JAVA_PATH
    JAVACMD="$JAVA_PATH/bin/java"
    # Test to see if file exists and is executable
    if [ ! -x "$JAVACMD" ] ; then
        printf "Error: JAVA Location you entered does not contain executable java:\n"
        echo $JAVACMD
        printf "Please try-again or ^c to exit\n"
        JAVA_PATH=""
    fi
  done
  echo "JAVA Location: $JAVA_PATH"

  sed -e "s,<JAVA_LOCATION>,${JAVA_PATH},g" \
      -e "s,<APPHOME>,${SCRIPT_PATH},g" \
         ${SCRIPT_PATH}/lib/startup.template > "${SCRIPT_PATH}/GUIslice.sh"

  chmod +x "${SCRIPT_PATH}/GUIslice.sh"

  # Create a temp folder for the install
  mkdir tmp

  # Create *.desktop file using the existing template file
  sed -e "s,<BINARY_LOCATION>,${SCRIPT_PATH}/GUIslice.sh,g" \
      -e "s,<WORKING_DIR>,${SCRIPT_PATH},g" \
      -e "s,<ICON_NAME>,${SCRIPT_PATH}/lib/guislicebuilder.png,g" \
      "${SCRIPT_PATH}/lib/desktop.template" > "${SCRIPT_PATH}/tmp/guislice.desktop"

  cp "${SCRIPT_PATH}/tmp/guislice.desktop" "${HOME}/Desktop"

  # Need to set execute permissions on guislice.desktop
  chmod +x "${HOME}/Desktop/guislice.desktop"

  # Clean up temp dir
  rm "${SCRIPT_PATH}/tmp/guislice.desktop"
  rmdir "${SCRIPT_PATH}/tmp"

  printf "Install Completed!\n"
}

# Uninstall by simply removing desktop files
uninstall_files() {
  printf "Removing desktop shortcut for GUIsliceBuilder\n" 
  if [ -f "${SCRIPT_PATH}/GUIslice.sh" ]; then
    rm "${SCRIPT_PATH}/GUIslice.sh"
  fi
 
  if [ -f "${HOME}/Desktop/guislice.desktop" ]; then
    rm "${HOME}/Desktop/guislice.desktop"
  fi
  printf "Uninstall Completed!\n"
}

# Check for provided arguments
while [ $# -gt 0 ] ; do
  ARG="${1}"
  case $ARG in
      -u|--uninstall)
        UNINSTALL=true
        shift
      ;;
      *)
        printf "\nInvalid option -- '${ARG}'\n"
        printf "\nOptional argument:\n"
        printf "\t-u, --uninstall\t\tRemoves shortcut, menu item and icons.\n\n"
        exit 1
      ;;
  esac
done

if [ ${UNINSTALL} = true ]; then
  uninstall_files
else
  uninstall_files
  install_files
fi

exit 0

 
