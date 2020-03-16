; installer.nsi
; Generated by Excelsior Installer v2.2
; Mon Jun 25 20:24:47 CEST 2012
; -----------------------------------------------------------------------------
; The installation script based on NSIS modern user interface
; -----------------------------------------------------------------------------
;
!define PROJECT_ROOT_PATH "C:\Documents and Settings\gida\Desktop\fred-0.1.0beta4"
!define PROJECT_NSIS_PATH "${PROJECT_ROOT_PATH}\NSIS"
!define PROJECT_NSIS_RESOURCES_PATH "${PROJECT_NSIS_PATH}\resources"
!define PROJECT_LONG_NAME "Forensic Registry EDitor"
!define PROJECT_SHORT_NAME "fred"
!define PROJECT_VERSION "0.1.0beta4"
!define PROJECT_DEF_INSTDIR "$PROGRAMFILES\\${PROJECT_SHORT_NAME}"

!define PACKAGENAME "${PROJECT_SHORT_NAME}-${PROJECT_VERSION}"
!define EIT_LOG_FILENAME "nsis_install.log"
!define DEF_SHORTCUTS_EXISTS
;
; -----------------------------------------------------------------------------
; Include Modern UI, logic statements, file functions and EIT support
!include "MUI2.nsh"
!include "x64.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"
!include "${PROJECT_SHORT_NAME}.nsh"
!include "${PROJECT_SHORT_NAME}_PageInstallType.nsh"
!include "${PROJECT_SHORT_NAME}_PageShortcuts.nsh"
!include "${PROJECT_SHORT_NAME}_PageInstallInfo.nsh"
;
; -----------------------------------------------------------------------------
; Configuration of the installation script
!define COMPANY_NAME ""
!define DEFAULT_PROGRAM_FOLDER "\${PROJECT_SHORT_NAME}"
; Valid values for installation type:  "auto" | "askuser" | "common" | "private"
!define DESIRED_INSTALL_TYPE "auto"
!define REGISTRY_KEY_NAME "Software\\${PROJECT_SHORT_NAME}"
!define LICENSE_TEXT_FILE "\\?\${PROJECT_NSIS_RESOURCES_PATH}\license.txt"
!define UNINSTALLER_RELPATH ""
!define UNINSTALLER_FILE_NAME  "Uninstall.exe"
!define UNINSTALLER_PATH       "$INSTDIR\${UNINSTALLER_RELPATH}"
; -----------------------------------------------------------------------------
; Declaration of variables
Var IsCommonInstallation
Var KeepOnUninstall
Var InstlDirWasNotEdited
Var LastDefaultInstalllDir
Var StartMenuFolder
; -----------------------------------------------------------------------------
; General Attributes
Name    "${PROJECT_LONG_NAME}" "${PROJECT_LONG_NAME}"
OutFile "\\?\${PROJECT_NSIS_PATH}\${PROJECT_SHORT_NAME}-${PROJECT_VERSION}-win32.exe"

; Default installation folder
InstallDir "${PROJECT_DEF_INSTDIR}"

; Version Information
VIAddVersionKey  "CompanyName"     ""
VIAddVersionKey  "LegalCopyright"  "Copyright (c) 2011-2012 by Gillen Daniel"
VIAddVersionKey  "ProductName"     "${PROJECT_LONG_NAME}"
VIAddVersionKey  "ProductVersion"  "${PROJECT_VERSION}"
VIAddVersionKey  "FileDescription" "${PROJECT_SHORT_NAME} ${PROJECT_VERSION}"
VIAddVersionKey  "FileVersion"     "${PROJECT_VERSION}"
VIProductVersion "0.1.0.0"

; The installer will perform a CRC on itself before allowing an install
CRCCheck on
; Request application privileges for Windows Vista
RequestExecutionLevel admin
BrandingText ""
; -----------------------------------------------------------------------------
; Interface Attributes and Settings
; The interface settings provided by the NSIS compiler itself (such as 
; LicenseText, Icon, CheckBitmap, InstallColors) should not be used in 
; Modern UI scripts. The Modern UI provides equalivent or extended 
; versions of these settings.
!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_UNICON "${PROJECT_NSIS_RESOURCES_PATH}\xuninst.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT

!define MUI_HEADERIMAGE_BITMAP   "${PROJECT_NSIS_RESOURCES_PATH}\xinst_header.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${PROJECT_NSIS_RESOURCES_PATH}\xinst_header.bmp"

!define MUI_WELCOMEFINISHPAGE_BITMAP "${PROJECT_NSIS_RESOURCES_PATH}\xinst_page.bmp"

!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${PROJECT_NSIS_RESOURCES_PATH}\xuninst_page.bmp"

!define MUI_WELCOMEPAGE_TITLE "$(MSG_WELCOMEPAGE_TITLE)"
!define MUI_WELCOMEPAGE_TEXT "$(MSG_WELCOMEPAGE_TEXT)"

; -----------------------------------------------------------------------------
; Installer Page Settings
; Do not display the checkbox to disable the creation of Start Menu shortcuts.
!define MUI_STARTMENUPAGE_NODISABLE
!ifdef MUI_FINISHPAGE_RUN
  !define MUI_FINISHPAGE_RUN_TEXT "$(MSG_FINISHPAGE_RUN_TEXT)"
!endif
!ifdef MUI_FINISHPAGE_SHOWREADME
  !define MUI_FINISHPAGE_SHOWREADME_TEXT "$(MSG_FINISHPAGE_SHOWREADME_TEXT)"
!endif
!ifdef FEATURE_REBOOT_NEEDED
  !define MUI_FINISHPAGE_TEXT_REBOOT      "$(MSG_FINISHPAGE_REBOOT_TEXT)"
  !define MUI_FINISHPAGE_TEXT_REBOOTNOW   "$(MSG_FINISHPAGE_REBOOT_NOW)"
  !define MUI_FINISHPAGE_TEXT_REBOOTLATER "$(MSG_FINISHPAGE_REBOOT_LATER)"
!endif
; Declaration of folder for shortcuts
${EIT_MUI_PAGE_SHORTCUTS_AddFolder}  EIT_MUI_SHORTCUT_FOLDER_DESKTOP
; -----------------------------------------------------------------------------
!ifdef CLEANUP_DIR
  !insertmacro EIT_FUNC_DECL_MkCleanupLogString
!endif
; -----------------------------------------------------------------------------
; If $INSTDIR was never edited by user - update it to default value to accept
; installation type changes
Function UpdateInstallDir
  ${if} $InstlDirWasNotEdited != 0
    StrCmp "$INSTDIR" "$LastDefaultInstalllDir" instdir_not_changed
    StrCpy $InstlDirWasNotEdited 0 ; oops. it was changed. turn this auto-update off
    Return
instdir_not_changed:
    StrCpy $INSTDIR "${PROJECT_DEF_INSTDIR}"
    StrCpy $LastDefaultInstalllDir "$INSTDIR"
  ${endif}
FunctionEnd ; UpdateInstallDir
; -----------------------------------------------------------------------------
; Call at exit from MUI_PAGE_WELCOME:
Function CheckAdminRights
  Push $R1
  StrCpy $R1 ${DESIRED_INSTALL_TYPE}
  ; EIT_CheckAdminRights():
  ;   In:  R1 = ${DESIRED_INSTALL_TYPE}
  ;   Out: R1 = 1/0 - Admin/NoAdmin
  ;        (OR exit with 'Admin rigths required' message)
  Call   EIT_CheckAdminRights
  ${if} $R1 != 0
    ${if} ${DESIRED_INSTALL_TYPE} != "personal"
      ${EIT_SetCommonInstallType} 1 ; by default it was 0
      Call UpdateInstallDir
    ${endif}
  ${endif}
  Pop $R1
FunctionEnd ; CheckAdminRights
; -----------------------------------------------------------------------------
; Call at exit from EIT_MUI_PAGE_INSTALLTYPE:
Function AcceptInstallType
  ${EIT_SetCommonInstallType} $IsCommonInstallation
  Call UpdateInstallDir
FunctionEnd ; AcceptInstallType
; -----------------------------------------------------------------------------
; Installer Pages
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE  CheckAdminRights
!insertmacro MUI_PAGE_WELCOME
!ifdef LICENSE_TEXT_FILE
  !insertmacro MUI_PAGE_LICENSE "${LICENSE_TEXT_FILE}"
!endif
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE  AcceptInstallType
!insertmacro EIT_MUI_PAGE_INSTALLTYPE  $IsCommonInstallation
!insertmacro MUI_PAGE_DIRECTORY
!define MUI_STARTMENUPAGE_DEFAULTFOLDER  "${DEFAULT_PROGRAM_FOLDER}"
  !insertmacro MUI_PAGE_STARTMENU  Application $StartMenuFolder
!insertmacro EIT_MUI_PAGE_SHORTCUTS  EIT_CreateShortCut
!insertmacro EIT_MUI_PAGE_INSTALLINFO  $StartMenuFolder  \
                                       EIT_MUI_PAGE_SHORTCUTS_GetInstallInfo
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
; -----------------------------------------------------------------------------
; Uninstaller Page Settings
!ifdef UNINSTALL_FEEDBACK_URL
  !define MUI_FINISHPAGE_TEXT            "$(MSG_FEEDBACK_TEXT)"
  !define MUI_FINISHPAGE_SHOWREADME      ${UNINSTALL_FEEDBACK_URL}
  !define MUI_FINISHPAGE_SHOWREADME_TEXT "$(MSG_FEEDBACK_CHK_TEXT)"
!endif
; -----------------------------------------------------------------------------
; Uninstaller Pages
!insertmacro MUI_UNPAGE_WELCOME
;  !insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
; -----------------------------------------------------------------------------
; Languages
##SUPPORTED_LANGUAGES
!insertmacro MUI_LANGUAGE_EIT "English"
;!insertmacro MUI_LANGUAGE_EIT "Czech"
;!insertmacro MUI_LANGUAGE_EIT "French"
;!insertmacro MUI_LANGUAGE_EIT "German"
;!insertmacro MUI_LANGUAGE_EIT "Italian"
;!insertmacro MUI_LANGUAGE_EIT "Japanese"
;!insertmacro MUI_LANGUAGE_EIT "Polish"
;!insertmacro MUI_LANGUAGE_EIT "Brazilian"
;!insertmacro MUI_LANGUAGE_EIT "Russian"
;!insertmacro MUI_LANGUAGE_EIT "Spanish"
; -----------------------------------------------------------------------------
; Reserve Files
  ;If you are using solid compression, files that are required before
;the actual installation should be stored first in the data block,
;because this will make your installer start faster.
  !insertmacro MUI_RESERVEFILE_LANGDLL
; -----------------------------------------------------------------------------
; Installer Sections
Section "Installer Section"
  Push   $0
  StrCpy $0 ""  ; collect cleanup directopy log strings in $0
  StrCpy $KeepOnUninstall 0
  ; Call pre-install action of install customization DLL if it was 
  ; specified in the project
  !ifdef INSTALL_CALLBACK_DLL
    !insertmacro EIT_CALL_PREINSTALL "${INSTALL_CALLBACK_DLL}"
  !endif
  ; Set $OUTPATH, open install log and create uninstaller
  ${EIT_StartInstallation} "${UNINSTALLER_PATH}" "${UNINSTALLER_FILE_NAME}" "$0"
  ; Add package files to be extracted to the output path
  ${EIT_CreateDirectory} "$INSTDIR\report_templates"
  ${EIT_AddFile} "$INSTDIR\report_templates\NTUSER_RecentDocs.qs" "${PROJECT_ROOT_PATH}\report_templates\NTUSER_RecentDocs.qs"
  ${EIT_AddFile} "$INSTDIR\report_templates\NTUSER_TypedUrls.qs" "${PROJECT_ROOT_PATH}\report_templates\NTUSER_TypedUrls.qs"
  ${EIT_AddFile} "$INSTDIR\report_templates\NTUSER_WindowsLiveAccounts.qs" "${PROJECT_ROOT_PATH}\report_templates\NTUSER_WindowsLiveAccounts.qs"
  ${EIT_AddFile} "$INSTDIR\report_templates\SAM_UserAccounts.qs" "${PROJECT_ROOT_PATH}\report_templates\SAM_UserAccounts.qs"
  ${EIT_AddFile} "$INSTDIR\report_templates\SOFTWARE_WindowsVersion.qs" "${PROJECT_ROOT_PATH}\report_templates\SOFTWARE_WindowsVersion.qs"
  ${EIT_AddFile} "$INSTDIR\report_templates\SYSTEM_CurrentNetworkSettings.qs" "${PROJECT_ROOT_PATH}\report_templates\SYSTEM_CurrentNetworkSettings.qs"
  ${EIT_AddFile} "$INSTDIR\report_templates\SYSTEM_SystemTimeInfo.qs" "${PROJECT_ROOT_PATH}\report_templates\SYSTEM_SystemTimeInfo.qs"
  ${EIT_AddFile} "$INSTDIR\report_templates\SYSTEM_UsbStorageDevices.qs" "${PROJECT_ROOT_PATH}\report_templates\SYSTEM_UsbStorageDevices.qs"
  ${EIT_AddFile} "$INSTDIR\fred.exe" "${PROJECT_ROOT_PATH}\fred.exe"
  ${EIT_AddFile} "$INSTDIR\libgcc_s_dw2-1.dll" "${PROJECT_ROOT_PATH}\libgcc_s_dw2-1.dll"
  ${EIT_AddFile} "$INSTDIR\libiconv-2.dll" "${PROJECT_ROOT_PATH}\libiconv-2.dll"
  ${EIT_AddFile} "$INSTDIR\mingwm10.dll" "${PROJECT_ROOT_PATH}\mingwm10.dll"
  ${EIT_AddFile} "$INSTDIR\phonon4.dll" "${PROJECT_ROOT_PATH}\phonon4.dll"
  ${EIT_AddFile} "$INSTDIR\QtCore4.dll" "${PROJECT_ROOT_PATH}\QtCore4.dll"
  ${EIT_AddFile} "$INSTDIR\QtGui4.dll" "${PROJECT_ROOT_PATH}\QtGui4.dll"
  ${EIT_AddFile} "$INSTDIR\QtNetwork4.dll" "${PROJECT_ROOT_PATH}\QtNetwork4.dll"
  ${EIT_AddFile} "$INSTDIR\QtScript4.dll" "${PROJECT_ROOT_PATH}\QtScript4.dll"
  ${EIT_AddFile} "$INSTDIR\QtWebKit4.dll" "${PROJECT_ROOT_PATH}\QtWebKit4.dll"
  StrCpy $KeepOnUninstall 0
  ; Create registry key
  !ifdef REGISTRY_KEY_NAME
    ${EIT_WriteRegStr} SHCTX "${REGISTRY_KEY_NAME}" "InstallPath" "$INSTDIR"
  !endif
  ; Create Start Menu shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    ${EIT_CreateDirRecursively}   "$SMPROGRAMS\$StartMenuFolder"
    ${EIT_CreateShortCut}  \
        "$SMPROGRAMS\$StartMenuFolder\fred.lnk"  "$INSTDIR\fred.exe"  \
        '""'  "$INSTDIR"

    ${EIT_CreateShortCut}  \
        "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"  "$INSTDIR\Uninstall.exe"  \
        '""'  "$INSTDIR"

  !insertmacro MUI_STARTMENU_WRITE_END

  ${EIT_MUI_PAGE_SHORTCUTS_CreateShortcut}  EIT_MUI_SHORTCUT_FOLDER_DESKTOP  \
        "\fred.lnk"  "$INSTDIR\fred.exe"  \
        '""'  "$INSTDIR"
  ; Call post-install action of install customization DLL if it was 
  ; specified in the project
  !ifdef INSTALL_CALLBACK_DLL
    !insertmacro EIT_CALL_POSTINSTALL
  !endif
  !ifdef FEATURE_REBOOT_NEEDED
    SetRebootFlag true
  !endif
  Pop $0
  System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0) v'
SectionEnd ; Installer Sections
; -----------------------------------------------------------------------------
; Customization of install NSIS callbacks
Function .onInit
  StrCpy $InstlDirWasNotEdited 1
  !ifdef SPLASH_FILE
    InitPluginsDir     
    File /oname=$PLUGINSDIR\splash.bmp "${SPLASH_FILE}"
    splash::show 1000 $PLUGINSDIR\splash
    Pop $0
  !endif
  !ifdef REGISTRY_KEY_NAME
    ReadRegStr $R0 HKLM "${REGISTRY_KEY_NAME}" "InstallPath"
    ${if} $R0 == ''
      ClearErrors
      ReadRegStr $R0 HKCU "${REGISTRY_KEY_NAME}" "InstallPath"
    ${endif}
    ${if} $R0 != ''
      ClearErrors
      FileOpen $R0 "${UNINSTALLER_PATH}${EIT_LOG_FILENAME}" "r"
      ${if} ${Errors}
        ClearErrors
      ${else}
        FileClose $R0
        MessageBox MB_YESNO "$(MSG_PREV_INST_FOUND)" IDYES L_DoInstall
        Quit
L_DoInstall:
      ${endif}
    ${endif}
  !endif
  StrCpy $LastDefaultInstalllDir $INSTDIR
FunctionEnd
; -----------------------------------------------------------------------------
Function .onInstFailed
  ${EIT_AbortInstallation} "${UNINSTALLER_PATH}" "${UNINSTALLER_FILE_NAME}"
FunctionEnd
; -----------------------------------------------------------------------------
Function .onInstSuccess
  ${EIT_EndInstallation} "${COMPANY_NAME}" "${UNINSTALLER_PATH}" "${UNINSTALLER_FILE_NAME}"
FunctionEnd
; -----------------------------------------------------------------------------
; Uninstaller Section
Section "un.Uninstaller Section"
  ; Call pre-uninstall action of uninstall customization DLL if it was 
  ; specified in the project
  !ifdef UNINSTALL_CALLBACK_DLL
  !endif
  ; at this point the $INSTDIR contais the path to Uninstall.exe
  ${un.EIT_ExecuteUninstall} "$INSTDIR\"
  RMDir "$INSTDIR"
SectionEnd ; Uninstaller Section
; -----------------------------------------------------------------------------
; Customization of uninstall NSIS callbacks
Function un.onInit
FunctionEnd