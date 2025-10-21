; Script Inno Setup para Controle de Estoque
[Setup]
AppName=Controle de Estoque - Legado v1.0
AppVersion=1.0
DefaultDirName={pf}\ControleEstoque
DefaultGroupName=Controle de Estoque
OutputDir=instalador
OutputBaseFilename=Instalador_Estoque
Compression=lzma
SolidCompression=yes

[Files]
; Executável principal
Source: "build\ControleEstoque.exe"; DestDir: "{app}"; Flags: ignoreversion

; DLLs essenciais do MinGW
Source: "build\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion

; (Opcional) Arquivos de dados
Source: "build\estoque.txt"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Controle de Estoque"; Filename: "{app}\ControleEstoque.exe"
Name: "{commondesktop}\Controle de Estoque"; Filename: "{app}\ControleEstoque.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Criar atalho na área de trabalho"; GroupDescription: "Opções adicionais:"
