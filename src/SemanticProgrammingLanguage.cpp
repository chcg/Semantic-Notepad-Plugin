#include "PluginInterface.h"
#include "NotepadMessages.h"

#include <windows.h>
#include <shellapi.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <cwchar>

#pragma comment(lib, "Shell32.lib")

namespace
{
    HINSTANCE g_hInstance = nullptr;
    NppData g_nppData{};

    constexpr wchar_t PLUGIN_NAME[] = L"Semantic Programming Language";
    constexpr wchar_t WEBSITE[] =
        L"https://www.semantic-programming-language.com/";
    constexpr wchar_t GITHUB[] =
        L"https://github.com/SemanticProgrammingLanguage";
    constexpr wchar_t UDL_FILENAME[] =
        L"Semantic-Programming-Language.xml";

    constexpr char UDL_XML[] = R"SEMANTICUDL(<?xml version="1.0" encoding="UTF-8"?>
<NotepadPlus>
  <UserLang name="Semantic Programming Language" ext="se sp" udlVersion="2.1">
    <Settings>
      <Global caseIgnored="no" allowFoldOfComments="no" foldCompact="no" forcePureLC="0" decimalSeparator="0" />
      <Prefix Keywords1="no" Keywords2="no" Keywords3="yes" Keywords4="no" Keywords5="no" Keywords6="no" Keywords7="no" Keywords8="no" />
    </Settings>
    <KeywordLists>
      <Keywords name="Comments">00# 01 02 03 04</Keywords>
      <Keywords name="Numbers, prefix1">0x 0X</Keywords>
      <Keywords name="Numbers, prefix2"></Keywords>
      <Keywords name="Numbers, extras1">.</Keywords>
      <Keywords name="Numbers, extras2">e E</Keywords>
      <Keywords name="Numbers, suffix1"></Keywords>
      <Keywords name="Numbers, suffix2"></Keywords>
      <Keywords name="Numbers, range">-</Keywords>
      <Keywords name="Operators1">- &gt; = : , ; { } [ ] ( )</Keywords>
      <Keywords name="Operators2"></Keywords>
      <Keywords name="Folders in code1, open">{</Keywords>
      <Keywords name="Folders in code1, middle"></Keywords>
      <Keywords name="Folders in code1, close">}</Keywords>
      <Keywords name="Folders in code2, open"></Keywords>
      <Keywords name="Folders in code2, middle"></Keywords>
      <Keywords name="Folders in code2, close"></Keywords>
      <Keywords name="Folders in comment, open"></Keywords>
      <Keywords name="Folders in comment, middle"></Keywords>
      <Keywords name="Folders in comment, close"></Keywords>
      <Keywords name="Keywords1">program object list ranges types type scopes scope nodes node relations</Keywords>
      <Keywords name="Keywords2">true false null unknown</Keywords>
      <Keywords name="Keywords3">% @</Keywords>
      <Keywords name="Keywords4">se sp</Keywords>
      <Keywords name="Keywords5"></Keywords>
      <Keywords name="Keywords6"></Keywords>
      <Keywords name="Keywords7"></Keywords>
      <Keywords name="Keywords8"></Keywords>
      <Keywords name="Delimiters">00&quot; 01\ 02&quot; 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23</Keywords>
    </KeywordLists>
    <Styles>
      <WordsStyle name="DEFAULT" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="COMMENTS" fgColor="5F875F" bgColor="FFFFFF" fontStyle="2" nesting="0" colorStyle="1" />
      <WordsStyle name="LINE COMMENTS" fgColor="5F875F" bgColor="FFFFFF" fontStyle="2" nesting="0" colorStyle="1" />
      <WordsStyle name="NUMBERS" fgColor="C678DD" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="1" />
      <WordsStyle name="KEYWORDS1" fgColor="E67E22" bgColor="FFFFFF" fontStyle="1" nesting="0" colorStyle="1" />
      <WordsStyle name="KEYWORDS2" fgColor="9B59B6" bgColor="FFFFFF" fontStyle="1" nesting="0" colorStyle="1" />
      <WordsStyle name="KEYWORDS3" fgColor="2E86C1" bgColor="FFFFFF" fontStyle="1" nesting="0" colorStyle="1" />
      <WordsStyle name="KEYWORDS4" fgColor="D35400" bgColor="FFFFFF" fontStyle="1" nesting="0" colorStyle="1" />
      <WordsStyle name="KEYWORDS5" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="KEYWORDS6" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="KEYWORDS7" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="KEYWORDS8" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="OPERATORS" fgColor="C0392B" bgColor="FFFFFF" fontStyle="1" nesting="0" colorStyle="1" />
      <WordsStyle name="FOLDER IN CODE1" fgColor="C0392B" bgColor="FFFFFF" fontStyle="1" nesting="0" colorStyle="1" />
      <WordsStyle name="FOLDER IN CODE2" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="FOLDER IN COMMENT" fgColor="5F875F" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="1" />
      <WordsStyle name="DELIMITERS1" fgColor="2471A3" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="1" />
      <WordsStyle name="DELIMITERS2" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="DELIMITERS3" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="DELIMITERS4" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="DELIMITERS5" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="DELIMITERS6" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="DELIMITERS7" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
      <WordsStyle name="DELIMITERS8" fgColor="000000" bgColor="FFFFFF" fontStyle="0" nesting="0" colorStyle="0" />
    </Styles>
  </UserLang>
</NotepadPlus>
)SEMANTICUDL";

    FuncItem g_funcItems[5]{};
    int g_funcCount = 0;

    std::filesystem::path getNotepadConfigRoot()
    {
        if (g_nppData._nppHandle)
        {
            const LRESULT required = SendMessageW(
                g_nppData._nppHandle,
                NPPM_GETPLUGINSCONFIGDIR,
                0,
                0
            );

            if (required > 0)
            {
                std::vector<wchar_t> buffer(
                    static_cast<size_t>(required) + 1,
                    L'\0'
                );

                const LRESULT ok = SendMessageW(
                    g_nppData._nppHandle,
                    NPPM_GETPLUGINSCONFIGDIR,
                    static_cast<WPARAM>(buffer.size()),
                    reinterpret_cast<LPARAM>(buffer.data())
                );

                if (ok)
                {
                    std::filesystem::path pluginConfig(buffer.data());

                    // Normally:
                    // %APPDATA%\Notepad++\plugins\Config
                    auto pluginsDir = pluginConfig.parent_path();
                    auto nppRoot = pluginsDir.parent_path();

                    if (!nppRoot.empty())
                        return nppRoot;
                }
            }
        }

        // Fallback for normal installed Notepad++.
        wchar_t* appData = nullptr;
        size_t len = 0;
        _wdupenv_s(&appData, &len, L"APPDATA");

        std::filesystem::path result;
        if (appData && *appData)
            result = std::filesystem::path(appData) / L"Notepad++";

        free(appData);
        return result;
    }

    void addCommand(
        int index,
        const wchar_t* name,
        PFUNCPLUGINCMD callback
    )
    {
        if (index < 0 || index >= static_cast<int>(std::size(g_funcItems)))
            return;

        wcsncpy_s(
            g_funcItems[index]._itemName,
            name,
            _TRUNCATE
        );
        g_funcItems[index]._pFunc = callback;
        g_funcItems[index]._init2Check = false;
        g_funcItems[index]._pShKey = nullptr;
    }

    void separator()
    {
        if (g_funcCount >= static_cast<int>(std::size(g_funcItems)))
            return;

        g_funcItems[g_funcCount]._itemName[0] = L'\0';
        g_funcItems[g_funcCount]._pFunc = nullptr;
        ++g_funcCount;
    }

    void openUrl(const wchar_t* url)
    {
        ShellExecuteW(
            g_nppData._nppHandle,
            L"open",
            url,
            nullptr,
            nullptr,
            SW_SHOWNORMAL
        );
    }

    void installUdl()
    {
        try
        {
            auto root = getNotepadConfigRoot();

            if (root.empty())
            {
                MessageBoxW(
                    g_nppData._nppHandle,
                    L"Could not determine the Notepad++ configuration directory.",
                    PLUGIN_NAME,
                    MB_OK | MB_ICONERROR
                );
                return;
            }

            auto udlDir = root / L"userDefineLangs";
            std::filesystem::create_directories(udlDir);

            auto target = udlDir / UDL_FILENAME;

            std::ofstream file(
                target,
                std::ios::binary | std::ios::trunc
            );

            if (!file)
            {
                MessageBoxW(
                    g_nppData._nppHandle,
                    L"Could not create the Semantic UDL file.",
                    PLUGIN_NAME,
                    MB_OK | MB_ICONERROR
                );
                return;
            }

            file.write(
                UDL_XML,
                static_cast<std::streamsize>(sizeof(UDL_XML) - 1)
            );
            file.close();

            std::wstring msg =
                L"Semantic Programming Language UDL was installed/updated:\n\n";
            msg += target.wstring();
            msg +=
                L"\n\nRestart Notepad++ so the updated language definition is loaded.";

            MessageBoxW(
                g_nppData._nppHandle,
                msg.c_str(),
                PLUGIN_NAME,
                MB_OK | MB_ICONINFORMATION
            );
        }
        catch (const std::exception&)
        {
            MessageBoxW(
                g_nppData._nppHandle,
                L"An unexpected error occurred while installing the Semantic UDL.",
                PLUGIN_NAME,
                MB_OK | MB_ICONERROR
            );
        }
    }

    void openWebsite()
    {
        openUrl(WEBSITE);
    }

    void openGitHub()
    {
        openUrl(GITHUB);
    }

    void showAbout()
    {
        MessageBoxW(
            g_nppData._nppHandle,
            L"Semantic Programming Language for Notepad++\n"
            L"Version 1.0.0\n\n"
            L"Installs and updates the Semantic .se/.sp User Defined Language.\n\n"
            L"Publisher: Tarek Wasfy\n"
            L"License: MIT",
            PLUGIN_NAME,
            MB_OK | MB_ICONINFORMATION
        );
    }

    void initCommands()
    {
        g_funcCount = 0;

        addCommand(
            g_funcCount++,
            L"Install / Update Semantic UDL",
            installUdl
        );

        separator();

        addCommand(
            g_funcCount++,
            L"Open Semantic Website",
            openWebsite
        );

        addCommand(
            g_funcCount++,
            L"Open Semantic GitHub",
            openGitHub
        );

        addCommand(
            g_funcCount++,
            L"About Semantic Programming Language",
            showAbout
        );
    }
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD reasonForCall,
    LPVOID
)
{
    if (reasonForCall == DLL_PROCESS_ATTACH)
    {
        g_hInstance = hModule;
        DisableThreadLibraryCalls(hModule);
        initCommands();
    }

    return TRUE;
}

extern "C" __declspec(dllexport)
void setInfo(NppData notepadPlusData)
{
    g_nppData = notepadPlusData;
}

extern "C" __declspec(dllexport)
const wchar_t* getName()
{
    return PLUGIN_NAME;
}

extern "C" __declspec(dllexport)
FuncItem* getFuncsArray(int* nbF)
{
    if (nbF)
        *nbF = g_funcCount;

    return g_funcItems;
}

extern "C" __declspec(dllexport)
void beNotified(SCNotification*)
{
}

extern "C" __declspec(dllexport)
LRESULT messageProc(UINT, WPARAM, LPARAM)
{
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL isUnicode()
{
    return TRUE;
}
