echo pass
RebaseTool.exe a
RebaseTool.exe -a b
RebaseTool.exe -ab c
RebaseTool.exe --a b
RebaseTool.exe --a b c
RebaseTool.exe --ab c
RebaseTool.exe --ab c d
RebaseTool.exe --ab -c d
RebaseTool.exe --ab -cd e
RebaseTool.exe -a b c
RebaseTool.exe -a b -c d e
RebaseTool.exe --a b --c d e

echo fail
RebaseTool.exe
RebaseTool.exe a b
RebaseTool.exe -ab c d
RebaseTool.exe --ab c d e
RebaseTool.exe -
RebaseTool.exe --
RebaseTool.exe - a
RebaseTool.exe -- b
RebaseTool.exe -ab
RebaseTool.exe --ab
RebaseTool.exe -a -b
RebaseTool.exe --ab -c
RebaseTool.exe --ab --c
RebaseTool.exe --ab --cd