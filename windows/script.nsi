;NSIS Modern User Interface
;Basic Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  !define /file Version ..\version.txt

;--------------------------------
;General

  OutFile "ardoise_win_mingw_x86_offline_installer.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES32\Ardoise"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKU "Software\Ardoise" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user
  
;--------------------------------
;Interface Settings


  !define MUI_ABORTWARNING
  !define MUI_LANGDLL_ALLLANGUAGES



;--------------------------------
;Pages
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "..\COPYING.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
   Page Custom shortcuts check ""
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
  
;--------------------------------
;Languages
 

  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "English"
  
LangString name ${LANG_FRENCH} "Ardoise"
LangString name ${LANG_ENGLISH} "Ardoise (Slate)"

Name $(name)
  
;--------------------------------
;Custom page
  
   Var Checkbox1
   Var Checkbox2
   Var Label
   Var Mypage

LangString SCText ${LANG_FRENCH} "Vous pouvez ajouter des raccourcis pour trouver et lancer plus rapidemant et facilement l'Ardoise :"
LangString SCText ${LANG_ENGLISH} "You can add some shortcuts to find and run the Ardoise (Slate) in an easier and faster way :"
LangString DesktopSCText ${LANG_FRENCH} "Créer un raccourcis sur le bureau"
LangString DesktopSCText ${LANG_ENGLISH} "Create desktop shortcut"
LangString QLSCText ${LANG_FRENCH} "Créer un raccourcis dans la barre de lancement rapide"
LangString QLSCText ${LANG_ENGLISH} "Create quick launch bar shortcut"
  
Function shortcuts 
   nsDialogs::Create 1018
	Pop $Mypage

	${If} $Mypage == error
		Abort
	${EndIf}

	${NSD_CreateLabel} 0 0 100% 40u $(SCText)
	Pop $Label

	${NSD_CreateCheckbox} 0 50u 100% 10u $(DesktopSCText)
   Pop $Checkbox1
   
	${NSD_CreateCheckbox} 0 63u 100% 10u $(QLSCText)
   Pop $Checkbox2

	nsDialogs::Show
;!insertmacro MUI_HEADER_TEXT $(PAGE_TITLE) $(PAGE_SUBTITLE)
FunctionEnd

Function check

  SetOutPath "$INSTDIR"
${NSD_GetState} $Checkbox1 $0
${If} $0 <> 0
  CreateShortCut "$DESKTOP\Ardoise.lnk" "$INSTDIR\ardoise.exe" ..\icon.ico
${EndIf}


${NSD_GetState} $Checkbox2 $0
${If} $0 <> 0
  CreateShortCut "$QUICKLAUNCH\Ardoise.lnk" "$INSTDIR\ardoise.exe" ..\icon.ico
${EndIf}
FunctionEnd
  

;--------------------------------
;Finish page
  
LangString DesktopShortcutText ${LANG_FRENCH} "Raccourci sur le bureau"
LangString DesktopShortcutText ${LANG_ENGLISH} "Desktop shortcut"
  /*
Function finishPage
   ${NSD_CreateCheckBox} 0 100u 100% 10u "Test"
FunctionEnd
  */
;--------------------------------
;Installer Sections

LangString Ardoise ${LANG_FRENCH} "Ardoise"
LangString Ardoise ${LANG_ENGLISH} "Ardoise (Slate)"

LangString Conf ${LANG_FRENCH} "Configurations de base"
LangString Conf ${LANG_ENGLISH} "Default configurations"

Section !$(Ardoise) Ardoise
  SetOutPath "$INSTDIR"
  File "ardoise.exe"
  File "icudt51.dll"
  File "icuin51.dll"
  File "icuuc51.dll"
  File "libgcc_s_sjlj-1.dll"
  File "libstdc++-6.dll"
  File "libwinpthread-1.dll"
  File "Qt5Core.dll"
  File "Qt5Gui.dll"
  File "Qt5Widgets.dll"
  File "Qt5Xml.dll"
  File "Qt5Network.dll"
  SetOutPath "$INSTDIR\platforms"
  File "qwindows.dll"
  CreateShortCut "$STARTMENU\Programs\Ardoise.lnk" "$INSTDIR\ardoise.exe"
SectionEnd


LangString ConfJson ${LANG_FRENCH} "fr"
LangString ConfJson ${LANG_ENGLISH} "en"

Section $(Conf) Conf
  SetOutPath "$INSTDIR\conf"
  File "conf\*"
  StrCmp $(ConfJson) "fr" +1 +3
     File "fr\*"
     Goto done
  StrCmp $(ConfJson) "en" +1 +3
     File "en\*"
     Goto done
  done:
SectionEnd

Section
  ;Store installation folder
  WriteRegStr HKU "Software\Ardoise" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_Ardoise ${LANG_FRENCH} "Programme Ardoise avec tous les composants nécessaires à son bon fonctionnemnet."
  LangString DESC_Ardoise ${LANG_ENGLISH} "Ardoise (Slate) programm with all needed componats."
  
  LangString DESC_Conf ${LANG_FRENCH} "Fichier de configuration de base incluant une palette prédéfinie"
  LangString DESC_Conf ${LANG_ENGLISH} "Configuration files including a default palette"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${Ardoise} $(DESC_Ardoise)
    !insertmacro MUI_DESCRIPTION_TEXT ${Conf} $(DESC_Conf)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END




;--------------------------------
;Uninstaller Section

Section "Uninstall"


  Delete "$INSTDIR\Uninstall.exe"
  Delete "$STARTMENU\Programs\Ardoise.lnk"
  Delete "$DESKTOP\Ardoise.lnk"
  Delete "$QUICKLAUNCH\Ardoise.lnk"

  RMDir /r "$INSTDIR"

  DeleteRegKey /ifempty HKU "Software\Ardoise"

SectionEnd




Function .onInit

	;Language selection dialog

	Push ""
	Push ${LANG_FRENCH}
	Push French
	Push ${LANG_ENGLISH}
	Push English
	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

	Pop $LANGUAGE
	StrCmp $LANGUAGE "cancel" 0 +2
		Abort
FunctionEnd
