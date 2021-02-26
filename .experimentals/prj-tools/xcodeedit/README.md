# xcodeedit

xcode pbxproj file easy edit command. cmake Xcode generator is unsupported localizefile. xcodeedit is change pbxproj localize file.

## usage

```shell
xcodeedit pbxproj-file commands
```

## command

### -d

Enable debug log. After command output log to stderr.

### -o OUTPUTFILE

Default override source pbxproj file. If you set -o "OUTPUTFILE" output to OUTPUTFILE. If choise outputfile is '-' , output to stdout.

### -InfoPlistsStrings DEFAULT-REGEON SEARCH-DIRECTORY

xcodeedit search localize file (en.lproj, ja.lproj, etc...) in SEARCH-DIRECTORY.Match file add pbxproj.