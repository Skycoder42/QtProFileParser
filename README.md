# QtProFileParser
A small library to parse Qt Project Files and provide easy handling of those

#### Features:
 - can follow pri includes
 - organizes the file into a map of keys to a list of values
 - supports preset of "conditions" to parse conditinal statements (for example "win32", "unix", ...)
 - provides a set of default values that are used a base
 - can understand " in values and \" as an escape-character (but without replacing it)
 - understands all different "operators" (=, +=, -=, ...) and evaluates them correctly
 - Can handle functions and variables without failing, but won't replace those
 - Completly cross-platform (only Qt-Code, should work on any system that's supported by Qt)

#### Planned:
 - Auto-Replacement of basic variables, e.g. $$TARGET will be replaced by it's contents
 - Auto-Detection of additional conditions added/removed by the CONFIG variable
 - recursive include protection (If a file includes itself, it will lead to a crash)
