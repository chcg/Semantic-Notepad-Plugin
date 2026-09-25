# Semantic Programming Language for Notepad++

Official Notepad++ plugin project for Semantic Programming Language.

## What the plugin does

The plugin adds a **Semantic Programming Language** menu under Notepad++ `Plugins`.

Commands:

- **Install / Update Semantic UDL**
- **Open Semantic Website**
- **Open Semantic GitHub**
- **About Semantic Programming Language**

The syntax highlighter itself is provided by the Semantic Notepad++ User Defined Language (UDL), which the plugin can install or update automatically.

Supported file extensions:

- `.se`
- `.sp`

## Build

Requirements:

- Windows
- Visual Studio 2022
- **Desktop development with C++** workload

Run:

```powershell
.\build-plugin.ps1
```

The x64 DLL will be created under:

```text
build\x64\Release\SemanticProgrammingLanguage.dll
```

The Plugins Admin release archive will be created under:

```text
dist\SemanticProgrammingLanguage-1.0.0-x64.zip
```

## Local installation

Create:

```text
Notepad++\plugins\SemanticProgrammingLanguage\
```

and copy:

```text
SemanticProgrammingLanguage.dll
```

into that folder, then restart Notepad++.

## Plugins Admin publication

The official Notepad++ plugin catalogue is:

https://github.com/notepad-plus-plus/nppPluginList

After publishing the release ZIP publicly:

1. Compute its SHA-256 (the build script prints it).
2. Put that hash into `nppPluginList-entry-template.json`.
3. Set the real GitHub release URL.
4. Add the entry to the correct Notepad++ plugin-list JSON.
5. Test with Plugins Admin.
6. Submit a PR to `notepad-plus-plus/nppPluginList`.

## Publisher

Tarek Wasfy

## Website

https://www.semantic-programming-language.com/

## GitHub

https://github.com/SemanticProgrammingLanguage

## License

MIT
