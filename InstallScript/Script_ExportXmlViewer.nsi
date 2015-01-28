; Script generated with the Venis Install Wizard

; Define your application name
!define APPNAME "ExportXmlViewer"
!define APPNAMEANDVERSION "ExportXmlViewer"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\ExportXmlViewer"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile ".\Setup_ExportXmlViewer.exe"

; Modern interface settings
!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\ExportXmlViewer.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

Section "ExportXmlViewer" Section1

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "..\release\icudt52.dll"
	File "..\release\icuin52.dll"
	File "..\release\icuuc52.dll"
	File "..\release\Qt5Core.dll"
	File "..\release\Qt5Gui.dll"
	File "..\release\Qt5Widgets.dll"
	File "..\release\Qt5Xml.dll"
	File "..\release\ExportXmlViewer.exe"
	CreateShortCut "$DESKTOP\ExportXmlViewer.lnk" "$INSTDIR\ExportXmlViewer.exe"
	CreateDirectory "$SMPROGRAMS\ExportXmlViewer"
	CreateShortCut "$SMPROGRAMS\ExportXmlViewer\ExportXmlViewer.lnk" "$INSTDIR\ExportXmlViewer.exe"
	CreateShortCut "$SMPROGRAMS\ExportXmlViewer\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd

Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${Section1} ""
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	; Delete self
	Delete "$INSTDIR\uninstall.exe"

	; Delete Shortcuts
	Delete "$DESKTOP\ExportXmlViewer.lnk"
	Delete "$SMPROGRAMS\ExportXmlViewer\ExportXmlViewer.lnk"
	Delete "$SMPROGRAMS\ExportXmlViewer\Uninstall.lnk"

	; Clean up ExportXmlViewer
	Delete "$INSTDIR\icudt52.dll"
	Delete "$INSTDIR\icuin52.dll"
	Delete "$INSTDIR\icuuc52.dll"
	Delete "$INSTDIR\Qt5Core.dll"
	Delete "$INSTDIR\Qt5Gui.dll"
	Delete "$INSTDIR\Qt5Widgets.dll"
	Delete "$INSTDIR\Qt5Xml.dll"
	Delete "$INSTDIR\ExportXmlViewer.exe"

	; Remove remaining directories
	RMDir "$SMPROGRAMS\ExportXmlViewer"
	RMDir "$INSTDIR\"

SectionEnd

BrandingText "Mindray"

; eof