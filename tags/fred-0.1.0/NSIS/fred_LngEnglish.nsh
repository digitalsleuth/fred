; installer_LngEnglish.nsh
; Generated by Excelsior Installer v2.2
; Mon Jun 25 20:24:47 CEST 2012
; -----------------------------------------------------------------------------
; English localization for additional messages

  !insertmacro LANGFILE "English" "English"

  ${LangFileString} EIT_MUI_TEXT_INSTALLTYPE_HEADER      "Installation type"  
  ${LangFileString} EIT_MUI_TEXT_INSTALLTYPE_SUBHEADER   "Choose the type of installation" 
  ${LangFileString} EIT_MUI_TEXT_INSTALLTYPE_DESCRIPTION "Install this application for:"
  ${LangFileString} EIT_MUI_TEXT_INSTALLTYPE_COMMON      "Anyone who uses this computer"
  ${LangFileString} EIT_MUI_TEXT_INSTALLTYPE_PERSONAL    "Current user only"

  ${LangFileString} EIT_MUI_TEXT_SHORTCUTS_HEADER      "Shortcut icons"
  ${LangFileString} EIT_MUI_TEXT_SHORTCUTS_SUBHEADER   "Select shortcuts you want to create"
  ${LangFileString} EIT_MUI_TEXT_SHORTCUTS_DESCRIPTION "Create shortcut icons in the following locations:"
  ${LangFileString} EIT_MUI_TEXT_SHORTCUTS_TODESKTOP   "Desktop"
  ${LangFileString} EIT_MUI_TEXT_SHORTCUTS_TOQUICK     "Quick Launch"
  ${LangFileString} EIT_MUI_TEXT_SHORTCUTS_TOSTARTMENU "Start Menu Root"
  ${LangFileString} EIT_MUI_TEXT_SHORTCUTS_TOSTARTUP   "Startup Folder"

  ${LangFileString} EIT_MUI_TEXT_FILEASSOCIATION_HEADER      "File extension associations"
  ${LangFileString} EIT_MUI_TEXT_FILEASSOCIATION_SUBHEADER   "Select file extensions you want to register"
  ${LangFileString} EIT_MUI_TEXT_FILEASSOCIATION_DESCRIPTION "Register the following file extensions:"

  ${LangFileString} EIT_MUI_TEXT_INFO_HEADER            "Start installation"
  ${LangFileString} EIT_MUI_TEXT_INFO_SUBHEADER         "View current settings"
  ${LangFileString} EIT_MUI_TEXT_INFO_DESCRIPTION       "The installer is ready to install $0 on your computer. Click Next to begin the installation or Back to change the current settings listed below."
  ${LangFileString} EIT_MUI_TEXT_INFO_DESCLABEL         "Current settings:"


  ${LangFileString} MSG_FILEEXTS_TEMPLATE          "Associate *.$0 files with $1"
  ${LangFileString} MSG_PREV_INST_FOUND            "Previous installation of $(^Name) was found.$\r$\nIt is recommended to remove it before installing this version.$\r$\n$\r$\nDo you wish to continue without removing the previous installation?"

  ${LangFileString} MSG_INFO_DESTINATION_DIRECTORY "Destination folder$\r$\n    $0$\r$\n$\r$\n"
  ${LangFileString} MSG_INFO_PROGRAM_FOLDER        "Program folder    $\r$\n    $0$\r$\n$\r$\n"
  ${LangFileString} MSG_INFO_SHORTCUTS_DESKTOP     "Create shortcuts on the Desktop$\r$\n$\r$\n"
  ${LangFileString} MSG_INFO_SHORTCUTS_STARTMENU   "Create shortcuts in the Start Menu$\r$\n$\r$\n"
  ${LangFileString} MSG_INFO_SHORTCUTS_QUICKLAUNCH "Create shortcuts on the Quick Launch$\r$\n$\r$\n"
  ${LangFileString} MSG_INFO_SHORTCUTS_STARTUP     "Create shortcuts in the Startup Folder$\r$\n$\r$\n"

  ${LangFileString} MSG_FINISHPAGE_REBOOT_TEXT     "Please, remove disks from drives and click Finish to complete setup."
  ${LangFileString} MSG_FINISHPAGE_REBOOT_NOW      "Yes, I want to restart my computer now."
  ${LangFileString} MSG_FINISHPAGE_REBOOT_LATER    "No, I will restart my computer later."

  ${LangFileString} MSG_FEEDBACK_CHK_TEXT          "Yes, take me to the feedback form when I click OK." 

  ${LangFileString} MSG_NO_RIGHTS                  "Administrator privileges are required to install this application"
  ${LangFileString} MSG_UNABLE_TO_READ_FILE        "Unable to read file $1"
  ${LangFileString} MSG_WIN_64_REQUIRED            "This installation package can be run on the 64-bit Windows only."


  ${LangFileString} MSG_WELCOMEPAGE_TITLE          "Welcome to ${PROJECT_LONG_NAME} v${PROJECT_VERSION} Installer"
  ${LangFileString} MSG_WELCOMEPAGE_TEXT           "This Installer will guide you through the installation of the software on your computer. To proceed, click Next."
